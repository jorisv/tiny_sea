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
#include <tiny_sea/core/exception.h>
#include <tiny_sea/core/linear_grid.h>
#include <tiny_sea/core/linear_list.h>
#include <tiny_sea/core/numeric_constants.h>
#include <tiny_sea/core/units.h>

namespace tiny_sea {

/*! Data stored at each world map cell.
 */
struct WorldMapData
{
    WorldMapData() = default;
    WorldMapData(radian_t p_windBearing, velocity_t p_windVelocity)
      : windBearing(p_windBearing)
      , windVelocity(p_windVelocity)
    {}

    radian_t windBearing;    // Wind angle from north clockwise
    velocity_t windVelocity; // Wind velocity
};

struct WorldMapDataInterpolator
{
    WorldMapData operator()(const WorldMapData& t0,
                            const WorldMapData& t1,
                            scale_t percent) const
    {
        UnitsInterpolator<velocity_t> vel_interpolator;

        // Returned wind bearing is not normalized between [0, 2PI]
        return WorldMapData(
          t0.windBearing +
            minDistance(t0.windBearing, t1.windBearing) * percent,
          vel_interpolator(t0.windVelocity, t1.windVelocity, percent));
    }
};

/// World map grid
using WorldMapGrid =
  LinearGrid<latitude_t, longitude_t, WorldMapData, WorldMapDataInterpolator>;

/// World map grid builder
using WorldMapGridBuilder = LinearGridBuilder<latitude_t,
                                              longitude_t,
                                              WorldMapData,
                                              WorldMapDataInterpolator>;

/// World map data type
class WorldMap
{
public:
    WorldMap(const WorldMapGrid& grid)
      : m_worldGrid(grid)
    {}

    const WorldMapGrid& worldGrid() const { return m_worldGrid; }

private:
    WorldMapGrid m_worldGrid;
};

/// World map by time
class TimeWorldMap
  : public LinearList<time_t, WorldMap, NullInterpolator<WorldMap>>
{
public:
    using parent_type =
      LinearList<time_t, WorldMap, NullInterpolator<WorldMap>>;

public:
    // Using all parent constructors
    using parent_type::parent_type;
};

/// World map by time builder
class TimeWorldMapBuilder
{
public:
    TimeWorldMapBuilder(const LinearSpace<time_t>& xSpace)
      : m_xSpace(xSpace)
    {}

    /// Add new world map to next time
    void add(const WorldMap& worldMap)
    {
        if (m_worldMaps.size() >= m_xSpace.nrPoints()) {
            throw Exception("Impossible to add one more WorldMap");
        }
        m_worldMaps.emplace_back(worldMap);
    }

    TimeWorldMap build() const
    {
        if (m_worldMaps.size() == m_xSpace.nrPoints()) {
            // The last value of m_values is considered mutable.
            // Since is not possible to do that in C++ we use const_cast.
            const_cast<std::vector<WorldMap>&>(m_worldMaps)
              .emplace_back(m_worldMaps.back());
        }

        if (m_worldMaps.size() != (m_xSpace.nrPoints() + 1)) {
            throw Exception("No enough WorldMap");
        }

        return TimeWorldMap(m_xSpace, m_worldMaps);
    }

private:
    LinearSpace<time_t> m_xSpace;
    std::vector<WorldMap> m_worldMaps;
};

}
