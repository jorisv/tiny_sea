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
#include <optional>

// tiny_sea
#include <tiny_sea/core/n_vector.h>
#include <tiny_sea/core/units.h>
#include <tiny_sea/gsp/discret_state.h>

namespace tiny_sea {

namespace gsp {

/*! State (Global shortest path state) represent a state of the global
 * shortest path computation process.
 *
 * It hold a position and a associated time.
 */
class State
{
public:
    State(const NVector& position,
          time_t time,
          DiscretState discretState,
          cost_t g,
          cost_t h,
          std::optional<DiscretState> parentState)
      : m_position(position)
      , m_time(time)
      , m_discretState(discretState)
      , m_g(g)
      , m_h(h)
      , m_f(m_g + m_h)
      , m_parentState(parentState)
    {}

    State(const NVector& position,
          std::chrono::seconds time,
          DiscretState discretState,
          cost_t g,
          cost_t h,
          std::optional<DiscretState> parentState)
      : State(position, fromChrono(time), discretState, g, h, parentState)
    {}

    State(const NVector& position,
          std::chrono::seconds time,
          DiscretState discretState)
      : State(position,
              time,
              discretState,
              cost_t(0.),
              cost_t(0.),
              std::nullopt)
    {}

    bool operator==(const State& o) const
    {
        return m_discretState == o.m_discretState;
    }

    bool same(const State& o) const
    {
        return std::get<1>(m_discretState) == std::get<1>(o.m_discretState) &&
               std::get<2>(m_discretState) == std::get<2>(o.m_discretState) &&
               std::get<3>(m_discretState) == std::get<3>(o.m_discretState);
    }
    bool better(const State& o) const { return m_f < o.m_f; }

    const NVector& position() const { return m_position; }
    time_t time() const { return m_time; }
    std::chrono::seconds seconds() const
    {
        return std::chrono::seconds(std::chrono::seconds::rep(m_time.t));
    }
    const DiscretState& discretState() const { return m_discretState; }
    std::optional<DiscretState> parentState() const { return m_parentState; }

    cost_t g() const { return m_g; }
    cost_t h() const { return m_h; }
    cost_t f() const { return m_f; }

private:
    NVector m_position;
    time_t m_time;
    DiscretState m_discretState;

    cost_t m_g, m_h, m_f;

    std::optional<DiscretState> m_parentState;
};

}

}
