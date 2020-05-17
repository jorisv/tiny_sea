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
#include <tiny_sea/gsp/binary_heap_nu_open_list.h>
#include <tiny_sea/gsp/binary_heap_open_list.h>
#include <tiny_sea/gsp/open_list.h>
#include <tiny_sea/gsp/state_factory.h>

using namespace tiny_sea;
using namespace tiny_sea::gsp;

template<typename OpenListType>
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
    OpenListType m_openList;
};

using OpenListTypes =
  ::testing::Types<OpenList, BinaryHeapNUOpenList, BinaryHeapOpenList>;
TYPED_TEST_SUITE(OpenListFixture, OpenListTypes);

TYPED_TEST(OpenListFixture, TEST_insert)
{
    auto state1 = this->m_factory->build(
      NVector(Eigen::Vector3d(10, 200, 300).normalized()),
      std::chrono::minutes(45));
    auto state2 = this->m_factory->build(
      NVector(Eigen::Vector3d(20, 220, 330).normalized()),
      std::chrono::minutes(39));
    auto state3 = this->m_factory->build(
      NVector(Eigen::Vector3d(110, 300, 400).normalized()),
      std::chrono::minutes(45));

    // Insert success
    auto it1 = this->m_openList.insert(state1);
    EXPECT_TRUE(it1.second);
    if constexpr (TypeParam::isUpdate) {
        EXPECT_EQ(*it1.first, state1);
    }

    // Insert fail (state1 and state2 are have the same DiscretState)
    auto it2 = this->m_openList.insert(state2);
    if constexpr (TypeParam::isUpdate) {
        EXPECT_FALSE(it2.second);
        EXPECT_EQ(*it2.first, state1);
    } else {
        EXPECT_TRUE(it2.second);
    }

    // Insert success
    auto it3 = this->m_openList.insert(state3);
    EXPECT_TRUE(it3.second);
    if constexpr (TypeParam::isUpdate) {
        EXPECT_EQ(*it3.first, state3);
    }
}

TYPED_TEST(OpenListFixture, TEST_empty)
{
    EXPECT_TRUE(this->m_openList.empty());

    auto state = this->m_factory->build(
      NVector(Eigen::Vector3d(10, 200, 300).normalized()),
      std::chrono::minutes(45));
    this->m_openList.insert(state);
    EXPECT_FALSE(this->m_openList.empty());
}

TYPED_TEST(OpenListFixture, TEST_pop)
{
    auto state1 = this->m_factory->build(
      NVector(Eigen::Vector3d(10, 200, 300).normalized()),
      std::chrono::minutes(45),
      DiscretState());
    auto state2 = this->m_factory->build(
      NVector(Eigen::Vector3d(110, 300, 400).normalized()),
      std::chrono::minutes(45),
      DiscretState());

    this->m_openList.insert(state1);
    this->m_openList.insert(state2);

    // Best state is state 2
    EXPECT_EQ(this->m_openList.pop(), state2);
    EXPECT_FALSE(this->m_openList.empty());

    // Best state is state 1
    EXPECT_EQ(this->m_openList.pop(), state1);
    EXPECT_TRUE(this->m_openList.empty());
}

TYPED_TEST(OpenListFixture, TEST_update)
{
    if constexpr (TypeParam::isUpdate) {
        auto state1 = this->m_factory->build(
          NVector(Eigen::Vector3d(10, 200, 300).normalized()),
          std::chrono::minutes(45));
        auto state2 = this->m_factory->build(
          NVector(Eigen::Vector3d(20, 210, 310).normalized()),
          std::chrono::minutes(45));

        auto it = this->m_openList.insert(state1);
        this->m_openList.update(it.first, state2);

        // Check state1 is replaced by state2
        EXPECT_EQ(this->m_openList.pop(), state2);
    }
}
