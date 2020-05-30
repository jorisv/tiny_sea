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
#include <tiny_sea/gsp/close_list.h>
#include <tiny_sea/gsp/neighbors_finder.h>
#include <tiny_sea/gsp/state_factory.h>

using namespace tiny_sea;
using namespace tiny_sea::gsp;

/*! Create a word map with 3 time step:
 * No wind in first time step
 * Wind from south in the second one
 * Too much wind from south
 */
class NeighborsFinderFixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Create a world with 4 time
        // [0h      1h      2h      3h]
        //  0 m/s   10 m/s  21 m/s  21 m/s
        TimeWorldMapBuilder timeWorldMapBuilder(
          makeLinearSpace(fromChrono(std::chrono::seconds(0)),
                          fromChrono(std::chrono::hours(1)),
                          4));

        // Create world map
        WorldMapGridBuilder gridBuilder(
          makeLinearSpace(latitude_t(0.), latitude_t(PI / 16.), 8),
          makeLinearSpace(longitude_t(0.), longitude_t(PI / 16.), 8));

        for (std::size_t lat = 0; lat < 8; ++lat) {
            for (std::size_t lon = 0; lon < 8; ++lon) {
                gridBuilder(lat, lon) =
                  WorldMapData(radian_t(0.), velocity_t(0.));
            }
        }
        timeWorldMapBuilder.add(WorldMap(gridBuilder.build()));

        for (std::size_t lat = 0; lat < 8; ++lat) {
            for (std::size_t lon = 0; lon < 8; ++lon) {
                gridBuilder(lat, lon) =
                  WorldMapData(radian_t(PI), velocity_t(10.));
            }
        }
        timeWorldMapBuilder.add(WorldMap(gridBuilder.build()));

        for (std::size_t lat = 0; lat < 8; ++lat) {
            for (std::size_t lon = 0; lon < 8; ++lon) {
                gridBuilder(lat, lon) =
                  WorldMapData(radian_t(PI), velocity_t(21.));
            }
        }
        timeWorldMapBuilder.add(WorldMap(gridBuilder.build()));
        timeWorldMapBuilder.add(WorldMap(gridBuilder.build()));

        // Create boat speed table
        m_velocity = velocity_t(5.);
        BoatVelocityTableBuilder velocityTableBuilder(
          makeLinearSpace(velocity_t(0.), velocity_t(10.), 3));
        velocityTableBuilder.addSymetric(
          radian_t(PI / 4.), { velocity_t(0.), m_velocity, velocity_t(0.) });
        m_boatVelocityTable.reset(
          new BoatVelocityTable(velocityTableBuilder.build()));

        m_start = NVector::fromLatLon(latitude_t(6. * (PI / 16.)),
                                      longitude_t(2. * (PI / 16.)));
        m_target = NVector::fromLatLon(latitude_t(2. * (PI / 16.)),
                                       longitude_t(6. * (PI / 16.)));
        m_distance = meter_t(100);
        m_factory.reset(new StateFactory(std::chrono::minutes(10),
                                         meter_t(50.),
                                         meter_t(EARTH_RADIUS),
                                         m_target,
                                         m_boatVelocityTable->maxVelocity()));
        m_timeWorldMap.reset(new TimeWorldMap(timeWorldMapBuilder.build()));
        m_neighborsFinder.reset(new NeighborsFinder(m_factory.get(),
                                                    m_timeWorldMap.get(),
                                                    m_boatVelocityTable.get(),
                                                    m_distance));
    }

    CloseList m_closeList;
    NVector m_start;
    NVector m_target;
    meter_t m_distance;
    velocity_t m_velocity;
    std::unique_ptr<StateFactory> m_factory;
    std::unique_ptr<TimeWorldMap> m_timeWorldMap;
    std::unique_ptr<BoatVelocityTable> m_boatVelocityTable;
    std::unique_ptr<NeighborsFinder> m_neighborsFinder;
};

/*! Expand a state after stop time.
 * No neighbors should be computed.
 */
TEST_F(NeighborsFinderFixture, TEST_search1)
{
    auto it =
      m_closeList.insert(m_factory->build(m_start, std::chrono::hours(3)));

    std::vector<State> res;
    m_neighborsFinder->search(it.first, res);
    EXPECT_TRUE(res.empty());
}

/*! Expand a state with 0 wind
 * One neighbor at same position and next time should be computed
 */
TEST_F(NeighborsFinderFixture, TEST_search2)
{
    auto it =
      m_closeList.insert(m_factory->build(m_start, std::chrono::hours(0)));

    std::vector<State> res;
    m_neighborsFinder->search(it.first, res);
    ASSERT_EQ(res.size(), 1);
    EXPECT_EQ(res[0].position(), it.first->position());
    EXPECT_EQ(res[0].time(), fromChrono(std::chrono::hours(1)));
    EXPECT_EQ(res[0].g(),
              it.first->g() + cost_t(fromChrono(std::chrono::hours(1)).t));
    EXPECT_EQ(res[0].h(), it.first->h());
    EXPECT_EQ(res[0].parentState(), it.first->discretState());
}

/*! Expand a state with wind
 * One static and 2 moved neighbors should be computed
 */
TEST_F(NeighborsFinderFixture, TEST_search3)
{
    auto it =
      m_closeList.insert(m_factory->build(m_start, std::chrono::hours(1)));

    std::vector<State> res;
    m_neighborsFinder->search(it.first, res);
    ASSERT_EQ(res.size(), 3);

    // Test static position
    EXPECT_EQ(res[0].position(), it.first->position());
    EXPECT_EQ(res[0].time(), fromChrono(std::chrono::hours(2)));
    EXPECT_EQ(res[0].g(),
              it.first->g() + cost_t(fromChrono(std::chrono::hours(1)).t));
    EXPECT_EQ(res[0].parentState(), it.first->discretState());

    // Test first position
    NVector pos1(it.first->position()
                   .destination(radian_t((PI / 4.) + PI), m_distance)
                   .toEigen());
    EXPECT_EQ(res[1].position(), pos1);
    EXPECT_EQ(res[1].time(), it.first->time() + (m_distance / m_velocity));
    EXPECT_EQ(res[1].parentState(), it.first->discretState());

    // Test second position
    NVector pos2(it.first->position()
                   .destination(radian_t((-PI / 4.) + PI), m_distance)
                   .toEigen());
    EXPECT_EQ(res[2].position(), pos2);
    EXPECT_EQ(res[2].time(), it.first->time() + (m_distance / m_velocity));
    EXPECT_EQ(res[2].parentState(), it.first->discretState());
}

/*! Expand a state with too much wind
 * One neighbor at same position and next time should be computed
 */
TEST_F(NeighborsFinderFixture, TEST_search4)
{
    auto it =
      m_closeList.insert(m_factory->build(m_start, std::chrono::hours(2)));

    std::vector<State> res;
    m_neighborsFinder->search(it.first, res);
    ASSERT_EQ(res.size(), 1);
    EXPECT_EQ(res[0].position(), it.first->position());
    EXPECT_EQ(res[0].time(), fromChrono(std::chrono::hours(3)));
    EXPECT_EQ(res[0].g(),
              it.first->g() + cost_t(fromChrono(std::chrono::hours(1)).t));
    EXPECT_EQ(res[0].h(), it.first->h());
    EXPECT_EQ(res[0].parentState(), it.first->discretState());
}
