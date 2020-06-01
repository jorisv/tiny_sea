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

// includes
// GTest
#include <gtest/gtest.h>

// tiny_sea
#include <tiny_sea/core/boat_velocity_table.h>
#include <tiny_sea/core/world_map.h>
#include <tiny_sea/gsp/binary_heap_nu_open_list.h>
#include <tiny_sea/gsp/binary_heap_open_list.h>
#include <tiny_sea/gsp/close_list.h>
#include <tiny_sea/gsp/global_shortest_path.h>
#include <tiny_sea/gsp/neighbors_finder.h>
#include <tiny_sea/gsp/state_factory.h>

using namespace tiny_sea;
using namespace tiny_sea::gsp;

namespace {

const double KNOT_TO_MS = 0.51444;
const double DEG_TO_RAD = PI / 180.;

}

/*! Create a word map with 3 time step and constant wind from north/east.
 * Simulate a sailing from Agde to Sète.
 * Agde: 0.75520397rad 0.06126106rad
 * Sète: 0.75764743rad 0.06457718rad
 */
template<typename OpenListType>
class OpenListBench : public ::testing::Test
{
protected:
    void SetUp() override
    {
        const std::size_t NR_WORLD = 7;

        TimeWorldMapBuilder timeWorldMapBuilder(
          makeLinearSpace(fromChrono(std::chrono::seconds(0)),
                          fromChrono(std::chrono::hours(1)),
                          NR_WORLD));

        // Create world map
        WorldMapGridBuilder gridBuilder(
          makeLinearSpace(latitude_t(0.75520397), latitude_t(0.00087266), 3),
          makeLinearSpace(longitude_t(0.06126106), longitude_t(0.00087266), 4));

        for (std::size_t i = 0; i < NR_WORLD; ++i) {
            for (std::size_t lat = 0; lat < 3; ++lat) {
                for (std::size_t lon = 0; lon < 4; ++lon) {
                    gridBuilder(lat, lon) = WorldMapData(
                      radian_t(PI / 4.), velocity_t(7. * KNOT_TO_MS));
                }
            }
            timeWorldMapBuilder.add(WorldMap(gridBuilder.build()));
        }

        // Create boat speed table
        BoatVelocityTableBuilder velocityTableBuilder(
          makeLinearSpace(velocity_t(0.), velocity_t(6. * KNOT_TO_MS), 4));
        velocityTableBuilder.addSymetric(radian_t(40. * DEG_TO_RAD),
                                         { velocity_t(0.),
                                           velocity_t(4.05 * KNOT_TO_MS),
                                           velocity_t(6.27 * KNOT_TO_MS),
                                           velocity_t(0.) });
        velocityTableBuilder.addSymetric(radian_t(90. * DEG_TO_RAD),
                                         { velocity_t(0.),
                                           velocity_t(6.14 * KNOT_TO_MS),
                                           velocity_t(7.47 * KNOT_TO_MS),
                                           velocity_t(0.) });
        velocityTableBuilder.add(radian_t(180. * DEG_TO_RAD),
                                 { velocity_t(0.),
                                   velocity_t(2.99 * KNOT_TO_MS),
                                   velocity_t(5.75 * KNOT_TO_MS),
                                   velocity_t(0.) });

        m_boatVelocityTable.reset(
          new BoatVelocityTable(velocityTableBuilder.build()));

        m_start =
          NVector::fromLatLon(latitude_t(0.75520397), longitude_t(0.06126106));
        m_target =
          NVector::fromLatLon(latitude_t(0.75764743), longitude_t(0.06457718));

        m_factory.reset(new StateFactory(std::chrono::minutes(10),
                                         meter_t(500.),
                                         meter_t(EARTH_RADIUS),
                                         m_target,
                                         m_boatVelocityTable->maxVelocity()));
        m_timeWorldMap.reset(new TimeWorldMap(timeWorldMapBuilder.build()));
        m_neighborsFinder.reset(new NeighborsFinder(m_factory.get(),
                                                    m_timeWorldMap.get(),
                                                    m_boatVelocityTable.get(),
                                                    meter_t(1000.)));
    }

    NVector m_start;
    NVector m_target;
    std::unique_ptr<StateFactory> m_factory;
    std::unique_ptr<TimeWorldMap> m_timeWorldMap;
    std::unique_ptr<BoatVelocityTable> m_boatVelocityTable;
    std::unique_ptr<NeighborsFinder> m_neighborsFinder;
    OpenListType m_openList;
};

using OpenListTypes =
  ::testing::Types<BinaryHeapNUOpenList, BinaryHeapOpenList>;
TYPED_TEST_SUITE(OpenListBench, OpenListTypes);

TYPED_TEST(OpenListBench, run)
{
    CloseList closeList;
    this->m_openList.insert(
      this->m_factory->build(this->m_start, std::chrono::seconds(0)));

    auto target =
      this->m_factory->build(this->m_target, std::chrono::seconds(0));
    auto res = findGlobalShortestPath(
      target, this->m_openList, closeList, *this->m_neighborsFinder);

    ASSERT_TRUE(res);
    auto state = res->state;

    // Test the result is in the same discret area
    EXPECT_LT(state.position().distance(this->m_target),
              meter_t(std::sqrt(2. * (500 * 500))));
}

