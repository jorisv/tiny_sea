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
#include <ostream>

namespace tiny_sea {

template<typename Tag>
struct StrongTypedef
{
    using value_type = typename Tag::value_type;
    using _self = StrongTypedef<Tag>;

    StrongTypedef() noexcept {}
    explicit StrongTypedef(value_type p_t) noexcept
      : t(p_t)
    {}

    operator value_type() const noexcept { return t; }

    bool operator<(_self o) const noexcept { return t < o.t; }
    bool operator<=(_self o) const noexcept { return t <= o.t; }
    bool operator>(_self o) const noexcept { return t > o.t; }
    bool operator>=(_self o) const noexcept { return t >= o.t; }
    bool operator==(_self o) const noexcept { return t == o.t; }

    value_type t;
};

template<typename Tag>
std::ostream&
operator<<(std::ostream& stream, StrongTypedef<Tag> s)
{
    return stream << s.t;
}

}

namespace std {

template<typename Tag>
struct hash<tiny_sea::StrongTypedef<Tag>>
{
    using argument_type = tiny_sea::StrongTypedef<Tag>;

    std::size_t operator()(argument_type s) const noexcept
    {
        return hash<typename argument_type::value_type>()(s);
    }
};

}
