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
#include <tiny_sea/core/linear_space.h>

using namespace tiny_sea;

TEST(LINEAR_SPACE_TESTS, TEST_stop)
{
    auto space = makeLinearSpace(latitude_t(-1.), latitude_t(0.5), 7);

    EXPECT_NEAR(space.stop().t, latitude_t(2.).t, 1e-8);
}

TEST(LINEAR_SPACE_TESTS, TEST_value)
{
    auto space = makeLinearSpace(latitude_t(-1.), latitude_t(0.5), 7);

    EXPECT_NEAR(space.value(0).t, latitude_t(-1.).t, 1e-8);
    EXPECT_NEAR(space.value(1).t, latitude_t(-0.5).t, 1e-8);
    EXPECT_NEAR(space.value(2).t, latitude_t(0.).t, 1e-8);
    EXPECT_NEAR(space.value(3).t, latitude_t(0.5).t, 1e-8);
    EXPECT_NEAR(space.value(4).t, latitude_t(1.).t, 1e-8);
    EXPECT_NEAR(space.value(5).t, latitude_t(1.5).t, 1e-8);
    EXPECT_NEAR(space.value(6).t, latitude_t(2.).t, 1e-8);
}

TEST(LINEAR_SPACE_TESTS, TEST_index)
{
    auto space = makeLinearSpace(latitude_t(-1.), latitude_t(0.5), 7);

    EXPECT_EQ(space.index(latitude_t(-1.)), 0);
    EXPECT_EQ(space.index(latitude_t(-0.8)), 0);
    EXPECT_EQ(space.index(latitude_t(1.3)), 4);
    EXPECT_EQ(space.index(latitude_t(1.5)), 5);
    EXPECT_EQ(space.index(latitude_t(2.)), 6);
}

TEST(LINEAR_SPACE_TESTS, TEST_safeIndex)
{
    auto space = makeLinearSpace(latitude_t(-1.), latitude_t(0.5), 7);

    EXPECT_EQ(space.safeIndex(latitude_t(-1.1)), 0);
    EXPECT_EQ(space.safeIndex(latitude_t(2.2)), 6);
}

TEST(LINEAR_SPACE_TESTS, TEST_from_bound)
{
    auto space = makeLinearSpaceFromBound(latitude_t(2.), latitude_t(4.5), 6);

    EXPECT_NEAR(space.start().t, latitude_t(2.).t, 1e-8);
    EXPECT_NEAR(space.stop().t, latitude_t(4.5).t, 1e-8);
    EXPECT_NEAR(space.delta().t, latitude_t(.5).t, 1e-8);
    EXPECT_EQ(space.nrPoints(), 6);
}

TEST(LINEAR_SPACE_TESTS, TEST_interpolation)
{
    auto space = makeLinearSpace(latitude_t(-1.), latitude_t(0.5), 7);

    auto res1 = space.interpolationWeight(latitude_t(-0.75));
    EXPECT_NEAR(res1.percent.t, 0.5, 1e-8);
    EXPECT_EQ(res1.index, 0);

    auto res2 = space.interpolationWeight(latitude_t(-1.));
    EXPECT_NEAR(res2.percent.t, 0., 1e-8);
    EXPECT_EQ(res2.index, 0);

    auto res3 = space.interpolationWeight(latitude_t(2.));
    EXPECT_NEAR(res3.percent.t, 0., 1e-8);
    EXPECT_EQ(res3.index, 6);
}

TEST(LINEAR_SPACE_TESTS, TEST_safeInterpolation)
{
    auto space = makeLinearSpace(latitude_t(-1.), latitude_t(0.5), 7);

    auto res1 = space.safeInterpolationWeight(latitude_t(-0.75));
    EXPECT_NEAR(res1.percent.t, 0.5, 1e-8);
    EXPECT_EQ(res1.index, 0);

    auto res2 = space.safeInterpolationWeight(latitude_t(-5.));
    EXPECT_NEAR(res2.percent.t, 0., 1e-8);
    EXPECT_EQ(res2.index, 0);

    auto res3 = space.safeInterpolationWeight(latitude_t(22.));
    EXPECT_NEAR(res3.percent.t, 0., 1e-8);
    EXPECT_EQ(res3.index, 6);
}

TEST(LINEAR_SPACE_TESTS, TEST_inside)
{
    auto space = makeLinearSpace(latitude_t(-1.), latitude_t(0.5), 7);

    EXPECT_TRUE(space.inside(latitude_t(0.)));
    EXPECT_FALSE(space.inside(latitude_t(-5.)));
    EXPECT_FALSE(space.inside(latitude_t(22.)));
}
