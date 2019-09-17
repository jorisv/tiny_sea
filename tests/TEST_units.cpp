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
#include <tiny_sea/core/units.h>

using namespace tiny_sea;

TEST(UNITS_TESTS, TEST_units_plus)
{
    EXPECT_EQ(latitude_t(1.) + latitude_t(2.), latitude_t(3.));
}

TEST(UNITS_TESTS, TEST_units_plus_eq)
{
    latitude_t t(1.);
    t += latitude_t(2.);
    EXPECT_EQ(t, latitude_t(3.));
}

TEST(UNITS_TESTS, TEST_units_minus)
{
    EXPECT_EQ(latitude_t(1.) - latitude_t(2.), latitude_t(-1.));
}

TEST(UNITS_TESTS, TEST_units_minus_eq)
{
    latitude_t t(1.);
    t -= latitude_t(2.);
    EXPECT_EQ(t, latitude_t(-1.));
}

TEST(UNITS_TESTS, TEST_units_multiplication)
{
    EXPECT_EQ(latitude_t(3.) * scale_t(2.), latitude_t(6.));
    EXPECT_EQ(scale_t(3.) * latitude_t(2.), latitude_t(6.));
}

TEST(UNITS_TESTS, TEST_units_multiplication_eq)
{
    latitude_t t(3.);
    t *= scale_t(2.);
    EXPECT_EQ(t, latitude_t(6.));
}

TEST(UNITS_TESTS, TEST_units_division)
{
    EXPECT_EQ(latitude_t(6.) / scale_t(2.), latitude_t(3.));
}

TEST(UNITS_TESTS, TEST_units_division_eq)
{
    latitude_t t(6.);
    t /= scale_t(2.);
    EXPECT_EQ(t, latitude_t(3.));
}

TEST(UNITS_TESTS, TEST_units_division_scale)
{
    EXPECT_EQ(latitude_t(6.) / latitude_t(2.), scale_t(3.));
}

namespace {

::testing::AssertionResult
testMinDist(double deg1, double deg2, double exptDeg)
{
    const double DEG_TO_RAD = PI / 180.;
    const double RAD_TO_DEG = 1. / DEG_TO_RAD;

    radian_t res =
      minDistance(radian_t(deg1 * DEG_TO_RAD), radian_t(deg2 * DEG_TO_RAD));
    double res_deg = res.t * RAD_TO_DEG;
    if (std::abs(res_deg - exptDeg) < 1e-8) {
        return ::testing::AssertionSuccess();
    } else {
        return ::testing::AssertionFailure()
               << deg1 << " to " << deg2 << " should be " << exptDeg << " not "
               << res_deg;
    }
}

::testing::AssertionResult
testMinDistTwoWay(double deg1, double deg2, double exptDeg)
{
    auto res1 = testMinDist(deg1, deg2, exptDeg);
    if (!res1) {
        return res1;
    }
    auto res2 = testMinDist(deg2, deg1, -exptDeg);
    return res2;
}

}

TEST(UNITS_TESTS, TEST_min_dist)
{
    EXPECT_TRUE(testMinDistTwoWay(350., 10., 20.));
    EXPECT_TRUE(testMinDistTwoWay(10., 100., 90.));
    EXPECT_TRUE(testMinDistTwoWay(100., 200., 100.));
    EXPECT_TRUE(testMinDistTwoWay(200., 300., 100.));
    EXPECT_TRUE(testMinDistTwoWay(300., 360., 60.));
    EXPECT_TRUE(testMinDistTwoWay(370., 420., 50.));
}
