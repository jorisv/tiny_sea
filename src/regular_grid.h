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
#include <cassert>
#include <vector>

namespace tiny_sea {

template<typename XUnit, typename YUnit, typename DataType>
class RegularGrid
{
public:
    using x_value_type = XUnit;
    using y_value_type = YUnit;
    using value_type = DataType;

    using x_container = std::vector<x_value_type>;
    using y_container = std::vector<y_value_type>;
    using value_container = std::vector<value_type>;

public:
    RegularGrid(const x_container& x_values, const y_container& y_values)
      : m_x_values(x_values)
      , m_y_values(y_values)
    {
        assert(m_x_values.size() > 1);
        assert(m_y_values.size() > 1);
    }

    RegularGrid(const x_container& x_values,
                const y_container& y_values,
                const value_container& values)
      : RegularGrid(x_values, y_values)
      , m_values(values)
    {
        assert(m_values.size() ==
               ((m_x_values.size() - 1) * (m_y_values.size() - 1)));
    }

    value_type operator()(std::size_t x, std::size_t y) const
    {
        return m_values[index(x, y)];
    }

    value_type& operator()(std::size_t x, std::size_t y)
    {
        return m_values[index(x, y)];
    }

    value_type operator()(x_value_type x, y_value_type y) const
    {
        return m_values[index(x, y)];
    }

    value_type& operator()(x_value_type x, y_value_type y)
    {
        return m_values[index(x, y)];
    }

    const x_value_type& x_values() const noexcept { return m_x_values; }

    const y_value_type& y_values() const noexcept { return m_y_values; }

private:
    std::size_t index(std::size_t x, std::size_t y) const
    {
        assert(x < m_x_values.size());
        assert(y < m_y_values.size());

        return x + y * m_x_values.size();
    }

    std::size_t index(x_value_type x, y_value_type y) const
    {
        assert(x >= m_x_values.front());
        assert(x <= m_x_values.back());
        assert(y >= m_y_values.front());
        assert(y <= m_y_values.back());

        auto it_x = std::lower_bound(m_x_values.begin(), m_x_values.end(), x);
        auto it_y = std::lower_bound(m_y_values.begin(), m_y_values.end(), y);

        return index(std::distance(m_x_values.begin(), it_x),
                     std::distance(m_y_values.begin(), it_y));
    }

private:
    x_container m_x_values;
    y_container m_y_values;
    value_container m_values;
};

} // namespace tiny_sea
