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
#include <algorithm>

// tiny_sea
#include <tiny_sea/core/exception.h>
#include <tiny_sea/core/linear_list.h>
#include <tiny_sea/core/numeric_constants.h>
#include <tiny_sea/core/units.h>

namespace tiny_sea {

/*! Store boat velocities table for a specific relative wind.
 */
struct BoatVelocity
{
    using WindToBoatVelocity =
      LinearList<velocity_t, velocity_t, UnitsInterpolator<velocity_t>>;
    using WindToBoatVelocityBuilder =
      LinearListBuilder<velocity_t, velocity_t, UnitsInterpolator<velocity_t>>;

    BoatVelocity(radian_t p_relativeWindBearing,
                 const WindToBoatVelocity& p_windVelocityToBoatVelocity)
      : relativeWindBearing(p_relativeWindBearing)
      , windVelocityToBoatVelocity(p_windVelocityToBoatVelocity)
    {}

    radian_t relativeWindBearing; //< Relative wind bearing
    /// Boat velocity according a wind velocity
    WindToBoatVelocity windVelocityToBoatVelocity;
};

/*! Store the boat velocity table
 */
class BoatVelocityTable
{
public:
    BoatVelocityTable(const std::vector<BoatVelocity>& boatVelocities,
                      velocity_t maxVelocity)
      : m_boatVelocities(boatVelocities)
      , m_boatMaxVelocity(maxVelocity)
    {}

    const std::vector<BoatVelocity>& velocityTable() const noexcept
    {
        return m_boatVelocities;
    }

    std::size_t size() const noexcept { return m_boatVelocities.size(); }

    velocity_t maxVelocity() const noexcept { return m_boatMaxVelocity; }

private:
    std::vector<BoatVelocity> m_boatVelocities;
    velocity_t m_boatMaxVelocity;
};

/*! BoatVelocityTable builder
 */
class BoatVelocityTableBuilder
{
public:
    BoatVelocityTableBuilder(const LinearSpace<velocity_t>& velocitySpace)
      : m_boatVelocityBuilder(velocitySpace)
      , m_maxVelocity(0.)
    {
        if (velocitySpace.start() < velocity_t(0.)) {
            throw Exception("Wind velocity can't be negative");
        }
    }

    void add(radian_t relativeWindBearing,
             const std::vector<velocity_t>& boatVelocity)
    {
        checkVelocity(boatVelocity);
        findMaxVel(boatVelocity);
        addToTable(relativeWindBearing, boatVelocity);
    }

    void addSymetric(radian_t relativeWindBearing,
                     const std::vector<velocity_t>& boatVelocity)
    {
        checkVelocity(boatVelocity);
        findMaxVel(boatVelocity);
        addToTable(relativeWindBearing, boatVelocity);
        addToTable(radian_t(2. * PI) - relativeWindBearing, boatVelocity);
    }

    BoatVelocityTable build() const
    {
        return BoatVelocityTable(m_boatVelocities, m_maxVelocity);
    }

private:
    void checkVelocity(const std::vector<velocity_t>& boatVelocity) const
    {
        for (velocity_t v : boatVelocity) {
            if (v < velocity_t(0.)) {
                throw Exception("Boat velocity can't be negative");
            }
        }
    }

    void addToTable(radian_t relativeWindBearing,
                    const std::vector<velocity_t>& boatVelocity)
    {
        for (std::size_t i = 0; i < boatVelocity.size(); ++i) {
            m_boatVelocityBuilder(i) = boatVelocity[i];
        }
        m_boatVelocities.emplace_back(relativeWindBearing,
                                      m_boatVelocityBuilder.build());
    }

    void findMaxVel(const std::vector<velocity_t>& boatVelocity)
    {
        m_maxVelocity =
          std::max(m_maxVelocity,
                   *std::max_element(boatVelocity.begin(), boatVelocity.end()));
    }

private:
    BoatVelocity::WindToBoatVelocityBuilder m_boatVelocityBuilder;
    velocity_t m_maxVelocity;
    std::vector<BoatVelocity> m_boatVelocities;
};

}
