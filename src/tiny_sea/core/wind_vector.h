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
// tiny_sea
#include <tiny_sea/core/units.h>

namespace tiny_sea {

/*! Wind velocity in longitude, latitude coordinate
 * X is west to east
 * Y is South to north
 */
struct WindVector
{
    WindVector() = default;
    WindVector(velocity_t x, velocity_t y)
      : m_x(x)
      , m_y(y)
    {}

    // TODO eigen conversion

    velocity_t m_x, m_y;
};

}
