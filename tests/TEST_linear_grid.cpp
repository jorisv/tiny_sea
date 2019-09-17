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
#include <tiny_sea/core/linear_grid.h>

using namespace tiny_sea;

TEST(LINEAR_GRID_TESTS, TEST_builder)
{
    LinearGridBuilder<latitude_t, longitude_t, int> gridBuilder(
      makeLinearSpace(latitude_t(2.), latitude_t(0.5), 3),
      makeLinearSpace(longitude_t(10.), longitude_t(0.5), 3));

    gridBuilder(0, 0) = 3;
    gridBuilder(1, 0) = 30;
    gridBuilder(2, 0) = 300;
    gridBuilder(0, 1) = 5;
    gridBuilder(1, 1) = 50;
    gridBuilder(2, 1) = 500;
    gridBuilder(0, 2) = 7;
    gridBuilder(1, 2) = 70;
    gridBuilder(2, 2) = 700;

    auto grid = gridBuilder.build();
    EXPECT_EQ(grid.values(),
              std::vector<int>({ 3,
                                 30,
                                 300,
                                 300,
                                 5,
                                 50,
                                 500,
                                 500,
                                 7,
                                 70,
                                 700,
                                 700,
                                 7,
                                 70,
                                 700,
                                 700 }));
}

/*!
 * lon
 *  11.     7       70       700
 *  10.5    5       50       500
 *  10.     3       30       300
 *           2.      2.5       3. lat
 */
class LinearGridFixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_grid.reset(new LinearGrid<latitude_t, longitude_t, double>(
          makeLinearSpace(latitude_t(2.), latitude_t(0.5), 3),
          makeLinearSpace(longitude_t(10.), longitude_t(0.5), 3),
          std::vector<double>({ 3.,
                                30.,
                                300.,
                                300.,
                                5.,
                                50.,
                                500.,
                                500.,
                                7.,
                                70.,
                                700.,
                                700.,
                                7.,
                                70.,
                                700.,
                                700. })));
    }

    std::unique_ptr<LinearGrid<latitude_t, longitude_t, double>> m_grid;
};

TEST_F(LinearGridFixture, TEST_at)
{
    EXPECT_EQ(m_grid->at(latitude_t(2.2), longitude_t(10.7)), 5.);
    EXPECT_EQ(m_grid->at(latitude_t(2.6), longitude_t(11.)), 70.);
    EXPECT_EQ(m_grid->at(latitude_t(2.7), longitude_t(10.1)), 30.);
}

TEST_F(LinearGridFixture, TEST_safeAt)
{
    EXPECT_EQ(m_grid->safeAt(latitude_t(1.9), longitude_t(10.7)), 5.);
    EXPECT_EQ(m_grid->safeAt(latitude_t(3.3), longitude_t(10.7)), 500.);
    EXPECT_EQ(m_grid->safeAt(latitude_t(2.7), longitude_t(9.1)), 30.);
    EXPECT_EQ(m_grid->safeAt(latitude_t(2.7), longitude_t(12.)), 70.);
    EXPECT_EQ(m_grid->safeAt(latitude_t(2.6), longitude_t(11.)), 70.);
}

TEST_F(LinearGridFixture, TEST_interpolated)
{
    EXPECT_NEAR(
      m_grid->interpolated(latitude_t(2.), longitude_t(10.)), 3., 1e-8);
    EXPECT_NEAR(
      m_grid->interpolated(latitude_t(2.25), longitude_t(10.25)), 22., 1e-8);
    EXPECT_NEAR(
      m_grid->interpolated(latitude_t(3.), longitude_t(11.)), 700., 1e-8);
    EXPECT_NEAR(
      m_grid->interpolated(latitude_t(2.6), longitude_t(10.9)), 184.8, 1e-8);
}

TEST_F(LinearGridFixture, TEST_safeInterpolated)
{
    EXPECT_NEAR(
      m_grid->safeInterpolated(latitude_t(-2.), longitude_t(10.)), 3., 1e-8);
    EXPECT_NEAR(
      m_grid->safeInterpolated(latitude_t(3.), longitude_t(14.)), 700., 1e-8);
    EXPECT_NEAR(m_grid->safeInterpolated(latitude_t(2.6), longitude_t(10.9)),
                184.8,
                1e-8);
}
