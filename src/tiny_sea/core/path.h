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
#include <tiny_sea/core/n_vector.h>
#include <tiny_sea/core/units.h>

namespace tiny_sea {

struct Path
{
    Path() = default;
    Path(const NVector& p_nvector, radian_t p_bearing)
      : nvector(p_nvector)
      , bearing(p_bearing)
    {}

    static Path fromNVector(const NVector& start, const NVector& stop);

    NVector intersection(const Path& o) const;

    NVector nvector;
    radian_t bearing;
};
} // namespace tiny_sea
