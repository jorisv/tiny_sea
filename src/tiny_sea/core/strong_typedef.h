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
#include <type_traits>

namespace tiny_sea {

template<typename... Elt>
struct ListT
{};

template<typename Type, typename RetType = Type>
struct OperationType
{
    using type = Type;
    using return_type = RetType;
};

template<typename T, typename L>
struct ContainsType;

template<template<typename...> typename L,
         typename... Elt,
         typename T,
         typename RT>
struct ContainsType<T, L<OperationType<T, RT>, Elt...>>
{
    using type = T;
    using return_type = RT;
};

template<template<typename...> typename L,
         typename... Elt,
         typename T,
         typename T2,
         typename RT>
struct ContainsType<T, L<OperationType<T2, RT>, Elt...>>
  : ContainsType<T, L<Elt...>>
{};

template<template<typename...> typename L, typename T>
struct ContainsType<T, L<>>
{};

template<typename T, typename RT, typename L>
struct ContainsTypeAndRetType;

template<template<typename...> typename L,
         typename... Elt,
         typename T,
         typename RT>
struct ContainsTypeAndRetType<T, RT, L<OperationType<T, RT>, Elt...>>
{
    using type = T;
    using return_type = RT;
};

template<template<typename...> typename L,
         typename... Elt,
         typename T,
         typename RT,
         typename T2,
         typename RT2>
struct ContainsTypeAndRetType<T, RT, L<OperationType<T2, RT2>, Elt...>>
  : ContainsTypeAndRetType<T, RT, L<Elt...>>
{};

template<template<typename...> typename L, typename T, typename RT>
struct ContainsTypeAndRetType<T, RT, L<>>
{};

template<typename Tag>
struct StrongTypedef
{
    using value_type = typename Tag::value_type;
    using _self = StrongTypedef<Tag>;
    using _enable_plus = typename Tag::plus;
    using _enable_minus = typename Tag::minus;
    using _enable_multiplication = typename Tag::multiplication;
    using _enable_division = typename Tag::division;

    StrongTypedef() noexcept {}
    explicit StrongTypedef(value_type p_t) noexcept
      : t(p_t)
    {}
    _self& operator=(_self o)
    {
        t = o.t;
        return *this;
    }

    explicit operator value_type() const { return t; }

    bool operator<(_self o) const noexcept { return t < o.t; }
    bool operator<=(_self o) const noexcept { return t <= o.t; }
    bool operator>(_self o) const noexcept { return t > o.t; }
    bool operator>=(_self o) const noexcept { return t >= o.t; }
    bool operator==(_self o) const noexcept { return t == o.t; }

    template<typename Type, typename Res = ContainsType<Type, _enable_plus>>
    typename Res::return_type operator+(Type o) const noexcept
    {
        return typename Res::return_type(t + o.t);
    }
    template<typename Type, typename Res = ContainsType<Type, _enable_minus>>
    typename Res::return_type operator-(Type o) const noexcept
    {
        return typename Res::return_type(t - o.t);
    }
    template<typename Type,
             typename Res = ContainsType<Type, _enable_multiplication>>
    typename Res::return_type operator*(Type o) const noexcept
    {
        return typename Res::return_type(t * o.t);
    }
    template<typename Type, typename Res = ContainsType<Type, _enable_division>>
    typename Res::return_type operator/(Type o) const noexcept
    {
        return typename Res::return_type(t / o.t);
    }

    template<typename Type,
             typename Res = ContainsTypeAndRetType<Type, _self, _enable_plus>>
    typename Res::return_type& operator+=(Type o) noexcept
    {
        t += o.t;
        return *this;
    }
    template<typename Type,
             typename Res = ContainsTypeAndRetType<Type, _self, _enable_minus>>
    typename Res::return_type& operator-=(Type o) noexcept
    {
        t -= o.t;
        return *this;
    }
    template<typename Type,
             typename Res =
               ContainsTypeAndRetType<Type, _self, _enable_multiplication>>
    typename Res::return_type& operator*=(Type o) noexcept
    {
        t *= o.t;
        return *this;
    }
    template<
      typename Type,
      typename Res = ContainsTypeAndRetType<Type, _self, _enable_division>>
    typename Res::return_type& operator/=(Type o) noexcept
    {
        t /= o.t;
        return *this;
    }

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
