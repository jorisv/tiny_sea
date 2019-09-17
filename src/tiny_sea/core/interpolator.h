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

/*! Interpolator for standard numeric type
 */
template<typename T>
struct NumericInterpolator
{
    T operator()(const T& t0, const T& t1, scale_t percent) const
    {
        return t0 + (t1 - t0) * percent.t;
    }
};

/*! Interpolator for units type
 */
template<typename T>
struct UnitsInterpolator
{
    T operator()(const T& t0, const T& t1, scale_t percent) const
    {
        return t0 + (t1 - t0) * percent;
    }
};

/*! Interpolator for standard numeric type
 */
template<typename T>
struct NullInterpolator
{
    T operator()(const T& t0, const T& /* t1 */, scale_t /* percent */) const
    {
        return t0;
    }
};

}
