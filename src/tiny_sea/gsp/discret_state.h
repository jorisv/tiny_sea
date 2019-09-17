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
#include <functional>
#include <tuple>

// tiny_sea
#include <tiny_sea/core/hash_utils.h>

namespace tiny_sea {

namespace gsp {

using DiscretState =
  std::tuple<std::uint64_t, std::int64_t, std::int64_t, std::int64_t>;

/*! Hash fonction for DiscretState
 */
struct DiscretStateHash
{
    std::size_t operator()(const DiscretState& d_state) const
    {
        return hashCombine(std::hash<std::uint64_t>()(std::get<0>(d_state)),
                           std::hash<std::int64_t>()(std::get<1>(d_state)),
                           std::hash<std::int64_t>()(std::get<2>(d_state)),
                           std::hash<std::int64_t>()(std::get<3>(d_state)));
    }
};

}

}
