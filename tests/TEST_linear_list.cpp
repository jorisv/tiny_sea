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
#include <tiny_sea/core/linear_list.h>

using namespace tiny_sea;

TEST(LINEAR_LIST_TESTS, TEST_builder)
{
    LinearListBuilder<latitude_t, int> listBuilder(
      makeLinearSpace(latitude_t(-1.), latitude_t(0.5), 2));

    listBuilder(0) = 3;
    listBuilder(1) = 5;

    auto list = listBuilder.build();
    EXPECT_EQ(list.values(), std::vector<int>({ 3, 5, 5 }));
}

/*!
 *           3       5
 * lat      -1.    -0.5 lat
 */
class LinearListFixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_list.reset(new LinearList<latitude_t, double>(
          makeLinearSpace(latitude_t(-1.), latitude_t(0.5), 2),
          { 3., 5., 5. }));
    }

    std::unique_ptr<LinearList<latitude_t, double>> m_list;
};

TEST_F(LinearListFixture, TEST_at)
{
    EXPECT_EQ(m_list->at(latitude_t(-1.)), 3.);
    EXPECT_EQ(m_list->at(latitude_t(-0.5)), 5.);
}

TEST_F(LinearListFixture, TEST_safeAt)
{
    EXPECT_EQ(m_list->safeAt(latitude_t(-2.)), 3.);
    EXPECT_EQ(m_list->safeAt(latitude_t(-0.5)), 5.);
    EXPECT_EQ(m_list->safeAt(latitude_t(0.3)), 5.);
}

TEST_F(LinearListFixture, TEST_interpolated)
{
    EXPECT_NEAR(m_list->interpolated(latitude_t(-1.)), 3., 1e-8);
    EXPECT_NEAR(m_list->interpolated(latitude_t(-0.75)), 4., 1e-8);
    EXPECT_NEAR(m_list->interpolated(latitude_t(-0.5)), 5., 1e-8);
}

TEST_F(LinearListFixture, TEST_safeInterpolated)
{
    EXPECT_NEAR(m_list->safeInterpolated(latitude_t(-5.)), 3., 1e-8);
    EXPECT_NEAR(m_list->safeInterpolated(latitude_t(-0.75)), 4., 1e-8);
    EXPECT_NEAR(m_list->safeInterpolated(latitude_t(0.)), 5., 1e-8);
}
