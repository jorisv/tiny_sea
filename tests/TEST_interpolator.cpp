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
#include <tiny_sea/core/interpolator.h>
#include <tiny_sea/core/world_map.h>

using namespace tiny_sea;

TEST(INTEPOLATOR_TESTS, TEST_numeric)
{
    NumericInterpolator<double> interpolator;

    EXPECT_NEAR(interpolator(0., 10., scale_t(0.1)), 1., 1e-8);
    EXPECT_NEAR(interpolator(0., 10., scale_t(0.9)), 9., 1e-8);
}

namespace {

::testing::AssertionResult
testWorldMapInterp(double windBearingDeg1,
                   velocity_t windVel1,
                   double windBearingDeg2,
                   velocity_t windVel2,
                   scale_t percent,
                   double exptWindBearingDeg,
                   velocity_t exptWindVel)
{
    const double DEG_TO_RAD = PI / 180.;
    const double RAD_TO_DEG = 1. / DEG_TO_RAD;

    WorldMapData w1(radian_t(windBearingDeg1 * DEG_TO_RAD), windVel1);
    WorldMapData w2(radian_t(windBearingDeg2 * DEG_TO_RAD), windVel2);

    WorldMapDataInterpolator interpolator;

    auto res = interpolator(w1, w2, percent);
    double res_wind_bearing_deg = res.windBearing.t * RAD_TO_DEG;

    if (std::abs(res_wind_bearing_deg - exptWindBearingDeg) < 1e-8) {
        if (std::abs((res.windVelocity - exptWindVel).t) < 1e-8) {
            return ::testing::AssertionSuccess();
        } else {
            return ::testing::AssertionFailure()
                   << "interpolation(" << windVel1 << " " << windVel2 << ", "
                   << percent << ") should be " << exptWindVel << " not "
                   << res.windVelocity;
        }
    } else {
        return ::testing::AssertionFailure()
               << "interpolation(" << windBearingDeg1 << " " << windBearingDeg2
               << ", " << percent << ") should be " << exptWindBearingDeg
               << " not " << res_wind_bearing_deg;
    }
}

}

TEST(INTEPOLATOR_TESTS, TEST_world_map_data)
{
    EXPECT_TRUE(testWorldMapInterp(355.,
                                   velocity_t(0.),
                                   5.,
                                   velocity_t(10.),
                                   scale_t(0.1),
                                   356.,
                                   velocity_t(1.)));
    EXPECT_TRUE(testWorldMapInterp(355.,
                                   velocity_t(0.),
                                   5.,
                                   velocity_t(10.),
                                   scale_t(0.9),
                                   364.,
                                   velocity_t(9.)));
}
