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

// std
#include <memory>

// tiny_sea
#include <tiny_sea/gsp/open_list.h>
#include <tiny_sea/gsp/state_factory.h>

using namespace tiny_sea;
using namespace tiny_sea::gsp;

class OpenListFixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_factory.reset(new StateFactory(std::chrono::hours(1),
                                         meter_t(100.),
                                         meter_t(1000.),
                                         NVector(1., 0., 0.),
                                         velocity_t(2.)));
    }

    std::unique_ptr<StateFactory> m_factory;
};

TEST_F(OpenListFixture, TEST_insert)
{
    auto state1 =
      m_factory->build(NVector(Eigen::Vector3d(10, 200, 300).normalized()),
                       std::chrono::minutes(45));
    auto state2 =
      m_factory->build(NVector(Eigen::Vector3d(20, 220, 330).normalized()),
                       std::chrono::minutes(39));
    auto state3 =
      m_factory->build(NVector(Eigen::Vector3d(110, 300, 400).normalized()),
                       std::chrono::minutes(45));

    OpenList openList;
    // Insert success
    auto it1 = openList.insert(state1);
    EXPECT_TRUE(it1.second);
    EXPECT_EQ(*it1.first, state1);

    // Insert fail (state1 and state2 are have the same DiscretState)
    auto it2 = openList.insert(state2);
    EXPECT_FALSE(it2.second);
    EXPECT_EQ(*it2.first, state1);

    // Insert success
    auto it3 = openList.insert(state3);
    EXPECT_TRUE(it3.second);
    EXPECT_EQ(*it3.first, state3);
}

TEST_F(OpenListFixture, TEST_empty)
{
    OpenList openList;
    EXPECT_TRUE(openList.empty());

    auto state =
      m_factory->build(NVector(Eigen::Vector3d(10, 200, 300).normalized()),
                       std::chrono::minutes(45));
    openList.insert(state);
    EXPECT_FALSE(openList.empty());
}

TEST_F(OpenListFixture, TEST_pop)
{
    auto state1 =
      m_factory->build(NVector(Eigen::Vector3d(10, 200, 300).normalized()),
                       std::chrono::minutes(45),
                       DiscretState());
    auto state2 =
      m_factory->build(NVector(Eigen::Vector3d(110, 300, 400).normalized()),
                       std::chrono::minutes(45),
                       DiscretState());

    std::vector<State> states({ state1, state2 });
    OpenList openList(states.begin(), states.end());

    // Best state is state 2
    EXPECT_EQ(openList.pop(), state2);
    EXPECT_FALSE(openList.empty());

    // Best state is state 1
    EXPECT_EQ(openList.pop(), state1);
    EXPECT_TRUE(openList.empty());
}

TEST_F(OpenListFixture, TEST_update)
{
    auto state1 =
      m_factory->build(NVector(Eigen::Vector3d(10, 200, 300).normalized()),
                       std::chrono::minutes(45),
                       DiscretState());
    auto state2 =
      m_factory->build(NVector(Eigen::Vector3d(20, 210, 310).normalized()),
                       std::chrono::minutes(45),
                       DiscretState());

    OpenList openList;

    auto it = openList.insert(state1);
    openList.update(it.first, state2);

    // Check state1 is replaced by state2
    EXPECT_EQ(openList.pop(), state2);
}
