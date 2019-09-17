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
// tiny_sea
#include <tiny_sea/fwd.h>
#include <tiny_sea/gsp/close_list.h>
#include <tiny_sea/gsp/state.h>

namespace tiny_sea {

namespace gsp {

class NeighborsFinder
{
public:
    NeighborsFinder(const StateFactory* state_factory,
                    const TimeWorldMap* timeWorldMap,
                    const BoatVelocityTable* speedStable,
                    meter_t moveDistance)
      : m_stateFactory(state_factory)
      , m_timeWorldMap(timeWorldMap)
      , m_speedTable(speedStable)
      , m_moveDistance(moveDistance)
    {}

    void search(CloseList::iterator it, std::vector<State>& neighbors) const;

private:
    const StateFactory* m_stateFactory;
    const TimeWorldMap* m_timeWorldMap;
    const BoatVelocityTable* m_speedTable;
    meter_t m_moveDistance;
};

}

}
