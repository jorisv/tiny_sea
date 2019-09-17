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

#pragma once

// includes
// std
#include <cmath>
#include <utility>

// tiny_sea
#include <tiny_sea/core/numeric_constants.h>
#include <tiny_sea/core/units.h>

// Eigen
#include <Eigen/Core>
#include <Eigen/Geometry>

namespace tiny_sea {

class NVector
{
public:
    NVector() = default;
    NVector(double x, double y, double z)
      : m_x(x)
      , m_y(y)
      , m_z(z)
    {}
    NVector(const Eigen::Vector3d& vec)
      : m_x(vec.x())
      , m_y(vec.y())
      , m_z(vec.z())
    {}

    static radian_t toAngle(meter_t distance);
    static NVector fromLatLon(latitude_t lat, longitude_t lon) noexcept
    {
        double clat = std::cos(lat.t);
        double slat = std::sin(lat.t);
        double clon = std::cos(lon.t);
        double slon = std::sin(lon.t);
        return NVector(clat * clon, clat * slon, slat);
    }

    std::pair<latitude_t, longitude_t> toLatLon() const noexcept
    {
        return std::make_pair(latitude_t(std::asin(m_z)),
                              longitude_t(std::atan2(m_y, m_x)));
    }

    /// \return Distance from the two vector
    meter_t distance(const NVector& o) const noexcept
    {
        return meter_t(EARTH_RADIUS *
                       std::atan2(toEigen().cross(o.toEigen()).norm(),
                                  toEigen().dot(o.toEigen())));
    }

    radian_t bearing(const NVector& o) const;

    /*! Compute a new vector based on bearing from north and a distance.
     * \param[in] bearing Orientation from north clockwise.
     * \param[in] distance Distance from origin.
     */
    NVector destination(radian_t bearing, meter_t distance) const
    {
        Eigen::Vector3d self = toEigen();

        // Compute frame around current position
        Eigen::Vector3d east_vec = Eigen::Vector3d::UnitZ().cross(self);
        Eigen::Vector3d north_vec = self.cross(east_vec);

        // Compute direction by rotating north vector by bearing radian around
        // current position vector
        Eigen::Vector3d direction =
          std::cos(bearing.t) * north_vec + std::sin(bearing.t) * east_vec;

        // Convert distance into angle and rotate current position around a
        // vector orthogonal to direction
        double angle = distance.t / EARTH_RADIUS;
        return NVector(std::cos(angle) * self + std::sin(angle) * direction);
    }

    Eigen::Vector3d toEigen() const { return Eigen::Vector3d(m_x, m_y, m_z); }

    double x() const noexcept { return m_x; }
    double y() const noexcept { return m_y; }
    double z() const noexcept { return m_z; }

    bool operator==(const NVector& o) const noexcept
    {
        return m_x == o.m_x && m_y == o.m_y && m_z == o.m_z;
    }

    bool operator!=(const NVector& o) const noexcept
    {
        return m_x != o.m_x || m_y != o.m_y || m_z != o.m_z;
    }

private:
    double m_x, m_y, m_z;
};

} // namespace tiny_sea
