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

// associated header
#include <tiny_sea/gsp/neighbors_finder.h>

// includes
// tiny_sea
#include <tiny_sea/core/boat_velocity_table.h>
#include <tiny_sea/core/world_map.h>
#include <tiny_sea/gsp/state_factory.h>

namespace tiny_sea {

namespace gsp {

void
NeighborsFinder::search(CloseList::iterator it,
                        std::vector<State>& neighbors) const
{
    // If current time is after time space, we stop
    if (it->time() >= m_timeWorldMap->xSpace().stop()) {
        return;
    }

    // Take WorldMap at current time
    auto world_index = m_timeWorldMap->xSpace().index(it->time());
    const auto& worldMap = (*m_timeWorldMap)(world_index);

    // Take WorldMap data at current position
    const auto& latLon = it->position().toLatLon();
    const auto& worldMapData =
      worldMap.worldGrid().safeInterpolated(latLon.first, latLon.second);

    // Add a static configuration at the next time
    auto next_time = m_timeWorldMap->xSpace().value(world_index + 1);
    neighbors.push_back(
      m_stateFactory->build(it->position(), next_time, it->discretState()));

    // Take minimal distance between hard coded move distance and remaining
    // distance
    auto distToGo =
      std::min(m_moveDistance, m_stateFactory->distanceToTarget(*it));
    for (const auto& boatSpeed : m_speedTable->velocityTable()) {
        // Compute target velocity
        velocity_t targetVelocity =
          boatSpeed.windVelocityToBoatVelocity.safeInterpolated(
            worldMapData.windVelocity);

        // If velocity is not null we compute the new position and time
        if (targetVelocity > velocity_t(0.)) {
            // Compute target bearing, relative wind + current wind
            radian_t targetBearing =
              worldMapData.windBearing + boatSpeed.relativeWindBearing;

            auto newPos = it->position().destination(targetBearing, distToGo);
            auto timeOffset = (distToGo / targetVelocity);

            neighbors.push_back(m_stateFactory->build(
              newPos, it->time() + timeOffset, it->discretState()));
        }
    }
}

}

}
