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
#include <chrono>
#include <cmath>

// tiny_sea
#include <tiny_sea/core/numeric_constants.h>
#include <tiny_sea/core/strong_typedef.h>

namespace tiny_sea {

struct ScaleTag;
struct LatitudeTag;
struct LongitudeTag;
struct MeterTag;
struct VelocityTag;
struct RadianTag;
struct CostTag;
struct TimeTag;
struct IndexTag;

using scale_t = StrongTypedef<ScaleTag>;
using latitude_t = StrongTypedef<LatitudeTag>;
using longitude_t = StrongTypedef<LongitudeTag>;
using meter_t = StrongTypedef<MeterTag>;
using velocity_t = StrongTypedef<VelocityTag>;
using radian_t = StrongTypedef<RadianTag>;
using cost_t = StrongTypedef<CostTag>;
using time_t = StrongTypedef<TimeTag>;
using index_t = StrongTypedef<IndexTag>;

struct ScaleTag
{
    using value_type = double;
    using plus = ListT<OperationType<scale_t>>;
    using minus = ListT<OperationType<scale_t>>;
    using multiplication = ListT<OperationType<scale_t>,
                                 OperationType<latitude_t, latitude_t>,
                                 OperationType<longitude_t, longitude_t>,
                                 OperationType<meter_t, meter_t>,
                                 OperationType<time_t, time_t>>;
    using division = ListT<OperationType<scale_t>>;
};

struct LatitudeTag
{
    using value_type = double;
    using plus = ListT<OperationType<latitude_t>>;
    using minus = ListT<OperationType<latitude_t>>;
    using multiplication = ListT<OperationType<scale_t, latitude_t>>;
    using division = ListT<OperationType<latitude_t, scale_t>,
                           OperationType<scale_t, latitude_t>>;
};

struct LongitudeTag
{
    using value_type = double;
    using plus = ListT<OperationType<longitude_t>>;
    using minus = ListT<OperationType<longitude_t>>;
    using multiplication = ListT<OperationType<scale_t, longitude_t>>;
    using division = ListT<OperationType<longitude_t, scale_t>,
                           OperationType<scale_t, longitude_t>>;
};

struct MeterTag
{
    using value_type = double;
    using plus = ListT<OperationType<meter_t>>;
    using minus = ListT<OperationType<meter_t>>;
    using multiplication = ListT<OperationType<scale_t, meter_t>>;
    using division = ListT<OperationType<meter_t, scale_t>,
                           OperationType<scale_t, meter_t>,
                           OperationType<time_t, velocity_t>,
                           OperationType<velocity_t, time_t>>;
};

struct VelocityTag
{
    using value_type = double;
    using plus = ListT<OperationType<velocity_t>>;
    using minus = ListT<OperationType<velocity_t>>;
    using multiplication =
      ListT<OperationType<scale_t, velocity_t>, OperationType<time_t, meter_t>>;
    using division = ListT<OperationType<velocity_t, scale_t>,
                           OperationType<scale_t, velocity_t>>;
};

struct RadianTag
{
    using value_type = double;
    using plus = ListT<OperationType<radian_t>>;
    using minus = ListT<OperationType<radian_t>>;
    using multiplication = ListT<OperationType<scale_t, radian_t>>;
    using division =
      ListT<OperationType<radian_t, scale_t>, OperationType<scale_t, radian_t>>;
};

struct CostTag
{
    using value_type = double;
    using plus = ListT<OperationType<cost_t>>;
    using minus = ListT<OperationType<cost_t>>;
    using multiplication = ListT<>;
    using division = ListT<>;
};

struct TimeTag
{
    using value_type = double;
    using plus = ListT<OperationType<time_t>>;
    using minus = ListT<OperationType<time_t>>;
    using multiplication = ListT<OperationType<scale_t, time_t>>;
    using division =
      ListT<OperationType<time_t, scale_t>, OperationType<scale_t, time_t>>;
};

struct IndexTag
{
    using value_type = std::size_t;
    using plus = ListT<OperationType<index_t>>;
    using minus = ListT<OperationType<index_t>>;
    using multiplication = ListT<>;
    using division = ListT<>;
};

template<class Rep, class Period = std::ratio<1>>
inline time_t
fromChrono(std::chrono::duration<Rep, Period> d)
{
    return time_t(std::chrono::seconds(d).count());
}

inline radian_t
minDistance(radian_t from, radian_t to)
{
    return radian_t(
      std::fmod(std::fmod((to - from).t, 2. * PI) + 3. * PI, 2. * PI) - PI);
}

}
