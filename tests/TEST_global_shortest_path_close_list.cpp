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
#include <tiny_sea/gsp/close_list.h>
#include <tiny_sea/gsp/state_factory.h>

using namespace tiny_sea;
using namespace tiny_sea::gsp;

class CloseListFixture : public ::testing::Test
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

TEST_F(CloseListFixture, TEST_insert_contains1)
{
    auto state =
      m_factory->build(NVector(Eigen::Vector3d(10, 200, 300).normalized()),
                       std::chrono::minutes(45));

    CloseList closeList;
    EXPECT_FALSE(closeList.contains(state));
    EXPECT_EQ(*closeList.insert(state), state);
    EXPECT_TRUE(closeList.contains(state));
}

TEST_F(CloseListFixture, TEST_insert_contains2)
{
    auto state1 =
      m_factory->build(NVector(Eigen::Vector3d(10, 200, 300).normalized()),
                       std::chrono::minutes(45));
    auto state2 =
      m_factory->build(NVector(Eigen::Vector3d(40, 230, 350).normalized()),
                       std::chrono::minutes(12));
    auto state3 =
      m_factory->build(NVector(Eigen::Vector3d(-10, 230, 350).normalized()),
                       std::chrono::minutes(12));

    std::vector<State> init_vec({ state1 });
    CloseList closeList(init_vec.begin(), init_vec.end());

    EXPECT_TRUE(closeList.contains(state1));
    EXPECT_TRUE(closeList.contains(state2));
    EXPECT_FALSE(closeList.contains(state3));

    auto insert_res = closeList.insert(state2);
    EXPECT_EQ(insert_res->position(), state1.position());
    EXPECT_NE(insert_res->position(), state2.position());

    EXPECT_EQ(*closeList.insert(state3), state3);
    EXPECT_TRUE(closeList.contains(state3));
}
