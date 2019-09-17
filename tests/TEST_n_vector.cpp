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
#include <tiny_sea/core/n_vector.h>

using namespace tiny_sea;

/*! Test latitude and longitude conversion to NVector by using frame reference
 * point
 */
TEST(NVECTOR_TESTS, TEST_from_lat_lon1)
{
    NVector vec = NVector::fromLatLon(latitude_t(0.), longitude_t(0.));
    EXPECT_NEAR((vec.toEigen() - Eigen::Vector3d(1., 0., 0.)).norm(), 0., 1e-8);
}

TEST(NVECTOR_TESTS, TEST_from_lat_lon2)
{
    NVector vec = NVector::fromLatLon(latitude_t(0.), longitude_t(PI / 2.));
    EXPECT_NEAR((vec.toEigen() - Eigen::Vector3d(0., 1., 0.)).norm(), 0., 1e-8);
}

TEST(NVECTOR_TESTS, TEST_from_lat_lon3)
{
    NVector vec = NVector::fromLatLon(latitude_t(PI / 2.), longitude_t(0.));
    EXPECT_NEAR((vec.toEigen() - Eigen::Vector3d(0., 0., 1.)).norm(), 0., 1e-8);
}

/*! Validate NVector to latitude and longitude conversion by using fromLatLon
 * method
 */
TEST(NVECTOR_TESTS, TEST_to_lat_lon)
{
    latitude_t lat(0.44);
    longitude_t lon(-0.95);
    NVector vec = NVector::fromLatLon(lat, lon);

    auto lat_lon_res = vec.toLatLon();
    EXPECT_NEAR(lat.t, lat_lon_res.first.t, 1e-8);
    EXPECT_NEAR(lon.t, lat_lon_res.second.t, 1e-8);
}

/*! Validate distance by computing distance between point from 1/4 of earth
 * perimeter
 */
TEST(NVECTOR_TESTS, TEST_distance)
{
    NVector vec1(1., 0., 0.);
    NVector vec2(0., 1., 0.);

    EXPECT_NEAR(vec1.distance(vec2).t, (EARTH_RADIUS * 2. * PI) / 4., 1e-8);
}

/*! Validate destination by computing point at 1/4 of earth perimeter
 */
TEST(NVECTOR_TESTS, TEST_destination)
{
    NVector vec(1., 0., 0.);
    NVector dest = vec.destination(radian_t(PI / 2.),
                                   meter_t((EARTH_RADIUS * 2. * PI) / 4.));

    EXPECT_NEAR(
      (dest.toEigen() - Eigen::Vector3d(0., 1., 0.)).norm(), 0., 1e-8);
}
