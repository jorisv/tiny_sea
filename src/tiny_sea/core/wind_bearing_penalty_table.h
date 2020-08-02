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
#include <tiny_sea/core/boat_velocity_table.h>
#include <tiny_sea/core/linear_grid.h>
#include <tiny_sea/core/units.h>

namespace tiny_sea {

/*! Table that contains penalties to apply when switching from a wind bearing
 * configuration to another one.
 *
 * Configuration are index in the \see BoatVelocityTable.
 *
 * A special configuration at BoatVelocityTable::size + 1 represent the null
 * wind configuration.
 *
 * PenaltyTable from a 2 configuration BoatVelocityTable will look like this:
 *      to  0  1  2
 * from
 *   0      a  b  0
 *   1      c  d  0
 *   2      0  0  0
 *
 *   Where a, b, c and d are penalty user input.
 *   Index 2 is the null wind configuration.
 */
class PenaltyTable
{
public:
    using table_t =
      LinearGrid<index_t, index_t, time_t, NullInterpolator<time_t>>;

public:
    PenaltyTable(const table_t& table)
      : m_table(table)
    {}

    time_t penalty(std::size_t fromWind, std::size_t toWind) const
    {
        return m_table(fromWind, toWind);
    }

    time_t safePenalty(std::size_t fromWind, std::size_t toWind) const
    {
        return m_table.safeAt(fromWind, toWind);
    }

private:
    table_t m_table;
};

class PenaltyTableBuilder
{
public:
    using value_type = time_t;

public:
    PenaltyTableBuilder(const BoatVelocityTable& velocityTable)
      : m_setSize(velocityTable.size())
      , m_tableBuilder(makeLinearSpace(index_t(0), index_t(1), m_setSize + 1),
                       makeLinearSpace(index_t(0), index_t(1), m_setSize + 1))
    {
        for (std::size_t i = 0; i < m_setSize; ++i) {
            m_tableBuilder(m_setSize, i) = time_t(0.);
        }
        for (std::size_t i = 0; i < m_setSize; ++i) {
            m_tableBuilder(i, m_setSize) = time_t(0.);
        }
        m_tableBuilder(m_setSize, m_setSize) = time_t(0.);
    }

    value_type operator()(std::size_t x, std::size_t y) const
    {
        assert(x != m_setSize);
        assert(y != m_setSize);
        return m_tableBuilder(x, y);
    }
    void setSymetric(std::size_t x, std::size_t y, value_type v)
    {
        assert(x != m_setSize);
        assert(y != m_setSize);
        m_tableBuilder(x, y) = v;
        m_tableBuilder(y, x) = v;
    }

    value_type safeAt(std::size_t x, std::size_t y) const
    {
        checkIndex(x, y);
        return m_tableBuilder.safeAt(x, y);
    }
    void safeSetSymetric(std::size_t x, std::size_t y, value_type v)
    {
        checkIndex(x, y);
        m_tableBuilder.safeAt(x, y) = v;
        m_tableBuilder.safeAt(y, x) = v;
    }

    PenaltyTable build() const { return PenaltyTable(m_tableBuilder.build()); }

private:
    void checkIndex(std::size_t x, std::size_t y) const
    {
        if (x == m_setSize || y == m_setSize) {
            std::ostringstream str;
            str << "Index (" << x << ", " << y << ") is not in grid of shape ("
                << m_setSize << ", " << m_setSize << ")";
            throw std::out_of_range(str.str());
        }
    }

private:
    const std::size_t m_setSize;
    LinearGridBuilder<index_t, index_t, time_t, NullInterpolator<time_t>>
      m_tableBuilder;
};
}
