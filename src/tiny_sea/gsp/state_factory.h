// TinySea: sailing boat routing library
// Copyright (C) 2019 Joris Vaillant
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

// includes
// std
#include <chrono>
#include <tuple>

// tiny_sea
#include <tiny_sea/core/n_vector.h>
#include <tiny_sea/core/units.h>
#include <tiny_sea/gsp/state.h>

namespace tiny_sea {

namespace gsp {

class StateFactory
{
public:
    StateFactory(time_t discretTime,
                 meter_t discretDistance,
                 meter_t earthRadius,
                 const NVector& targetPos,
                 velocity_t maxVelocity)
      : m_discretTime(discretTime)
      , m_discretDistance(discretDistance)
      , m_earthRadius(earthRadius)
      , m_targetPos(targetPos)
      , m_maxVelocity(maxVelocity)
    {}

    StateFactory(std::chrono::seconds discretTime,
                 meter_t discretDistance,
                 meter_t earthRadius,
                 const NVector& targetPos,
                 velocity_t maxVelocity)
      : m_discretTime(fromChrono(discretTime))
      , m_discretDistance(discretDistance)
      , m_earthRadius(earthRadius)
      , m_targetPos(targetPos)
      , m_maxVelocity(maxVelocity)
    {}

    State build(const NVector& position,
                time_t time,
                DiscretState parentState) const
    {
        return State(position,
                     time,
                     buildDiscretState(position, time),
                     cost_t(time.t),
                     computeHeuristic(position),
                     std::make_optional(parentState));
    }

    State build(const NVector& position,
                std::chrono::seconds time,
                DiscretState parentState) const
    {
        return build(position, fromChrono(time), parentState);
    }

    State build(const NVector& position, time_t time) const
    {
        return State(position,
                     time,
                     buildDiscretState(position, time),
                     cost_t(time.t),
                     computeHeuristic(position),
                     std::nullopt);
    }

    State build(const NVector& position, std::chrono::seconds time) const
    {
        return build(position, fromChrono(time));
    }

    meter_t distanceToTarget(const State& state) const
    {
        return meter_t(state.h().t * m_maxVelocity.t);
    }

private:
    DiscretState buildDiscretState(const NVector& position, time_t time) const
    {
        std::uint64_t d_time = std::floor((time / m_discretTime).t);
        Eigen::Vector3d range_pos =
          (position.toEigen() * (m_earthRadius / m_discretDistance).t)
            .array()
            .floor();
        return std::make_tuple(d_time,
                               std::int64_t(range_pos.x()),
                               std::int64_t(range_pos.y()),
                               std::int64_t(range_pos.z()));
    }

    cost_t computeHeuristic(const NVector& position) const
    {
        auto dist = position.distance(m_targetPos);
        return cost_t((dist / m_maxVelocity).t);
    }

private:
    time_t m_discretTime;
    meter_t m_discretDistance;
    meter_t m_earthRadius;
    NVector m_targetPos;
    velocity_t m_maxVelocity;
};

}

}
