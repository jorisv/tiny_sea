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
// std
#include <cstddef>
#include <type_traits>

namespace tiny_sea {

// Algorithm to combine two hash value from boost:
// https://www.boost.org/doc/libs/1_71_0/doc/html/hash/reference.html#boost.hash_combine
constexpr inline std::size_t
hashCombine(std::size_t hash1, std::size_t hash2)
{
    return hash1 ^ (hash2 * 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
}

template<typename... T>
constexpr inline std::
  enable_if_t<std::conjunction_v<std::is_same<T, std::size_t>...>, std::size_t>
  hashCombine(std::size_t hash1, std::size_t hash2, T... hashes)
{
    return hashCombine(hashCombine(hash1, hash2), hashes...);
}

}
