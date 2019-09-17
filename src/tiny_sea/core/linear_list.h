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
#include <sstream>
#include <stdexcept>
#include <vector>

// tiny_sea
#include <tiny_sea/core/interpolator.h>
#include <tiny_sea/core/linear_space.h>

namespace tiny_sea {

namespace internal {

template<typename XUnit>
inline std::size_t
index1D(const XUnit& xSpace, std::size_t x)
{
    return x;
}

template<typename XUnit>
inline void
checkIndex1D(const XUnit& xSpace, std::size_t x)
{
    if (x < 0 || x >= xSpace.nrPoints()) {
        std::ostringstream str;
        str << "Index (" << x << ") is not in grid of shape ("
            << xSpace.nrPoints() << ")";
        throw std::out_of_range(str.str());
    }
}

}

/*! LinearList associate a value to all points of a linear space.
 * A LinearList can be see as follow:
 *
 *          [start()  1      2        3      4       stop()]
 * space  :  2.      2.5     3.      3.5     4.      4.5
 * values :  4.      -2.     5.      20.     33.     11.
 *
 *  4. is the value associated to the range [2.   2.5[
 * -2. is the value associated to the range [2.5  3. [
 *  5. is the value associated to the range [3.   3.5[
 * 20. is the value associated to the range [3.5  4. [
 * 33. is the value associated to the range [4.   4.5[
 * 11. is the value associated to 4.5
 */
template<typename XUnit,
         typename DataType,
         typename Interpolator = NumericInterpolator<DataType>>
class LinearList
{
public:
    using x_value_type = XUnit;
    using value_type = DataType;
    using interpolator_type = Interpolator;

    using x_space_type = LinearSpace<x_value_type>;
    using value_container_type = std::vector<value_type>;

public:
    /*! Create a LinearList from a LinearSpace and a vector of values.
     * \warning \p values must have the \p xSpace size + 1. The last value
     * should be duplicated.
     */
    LinearList(const x_space_type& xSpace, const value_container_type& values)
      : m_xSpace(xSpace)
      , m_values(values)
    {
        assert(m_values.size() == (m_xSpace.nrPoints() + 1));
    }

    /*! Getter from index.
     * \return Value associated to index x.
     * \warning \p x must be a valid index.
     */
    value_type operator()(std::size_t x) const { return m_values[index(x)]; }
    value_type safeAt(std::size_t x) const
    {
        internal::checkIndex1D(m_xSpace, x);
        return m_values[safeIndex(x)];
    }

    /*! Getter from linear space value.
     * \return Value associated linear space value.
     * \warning \p x must be inside the linear space.
     */
    value_type operator()(x_value_type x) const { return m_values[index(x)]; }
    value_type at(x_value_type x) const { return m_values[index(x)]; }

    /*! Safe getter from linear space
     * \param[in] x Clamped between [start(), stop()].
     */
    value_type safeAt(x_value_type x) const noexcept
    {
        return m_values[safeIndex(x)];
    }

    /*!
     * \return Interpolated value
     * \warning \p x must be inside the linear space.
     */
    value_type interpolated(x_value_type x) const
    {
        auto res = m_xSpace.interpolationWeight(x);
        return intepolate(res);
    }

    /*!
     * \param[in] x Clamped between [start(), stop()].
     * \return Interpolated value
     */
    value_type safeInterpolated(x_value_type x) const noexcept
    {
        auto res = m_xSpace.safeInterpolationWeight(x);
        return intepolate(res);
    }

    /// Linear space getter
    const x_space_type& xSpace() const noexcept { return m_xSpace; }

    /// Values getter
    const value_container_type& values() const noexcept { return m_values; };

private:
    /// \return Value index from linear space index
    std::size_t index(std::size_t x) const
    {
        assert(x >= 0);
        assert(x < m_xSpace.nrPoints());

        return internal::index1D(m_xSpace, x);
    }

    /// \return Value index from linear space index, don't check bounds
    std::size_t safeIndex(std::size_t x) const noexcept
    {
        return internal::index1D(m_xSpace, x);
    }

    /// \return Value index from linear space value
    std::size_t index(x_value_type x) const { return index(m_xSpace.index(x)); }

    /*! \return Value index from linear space value
     * \param[in] x Clamped between [start(), stop()].
     */
    std::size_t safeIndex(x_value_type x) const noexcept
    {
        return index(m_xSpace.safeIndex(x));
    }

    /// \return Linear interpolation
    value_type intepolate(
      const LinearSpaceInterpolationResult& res) const noexcept
    {
        interpolator_type interpolator;
        return interpolator(
          m_values[res.index], m_values[res.index + 1], res.percent);
    }

private:
    x_space_type m_xSpace;
    value_container_type m_values;
};

/*! Helper class to build LinearList.
 */
template<typename XUnit,
         typename DataType,
         typename Interpolator = NumericInterpolator<DataType>>
class LinearListBuilder
{
public:
    using x_value_type = XUnit;
    using value_type = DataType;
    using interpolator_type = Interpolator;

    using x_space_type = LinearSpace<x_value_type>;
    using value_container_type = std::vector<value_type>;

    using linear_list_type =
      LinearList<x_value_type, value_type, interpolator_type>;

public:
    /*! Create a LinearList from a LinearSpace and a vector of values.
     * \warning \p value must have the \p x_space size.
     */
    LinearListBuilder(const x_space_type& xSpace)
      : m_xSpace(xSpace)
      , m_values(xSpace.nrPoints() + 1)
    {}

    /*! Getter from index.
     * \return Value associated to index x.
     * \warning \p x must be a valid index.
     */
    value_type operator()(std::size_t x) const { return m_values[index(x)]; }
    value_type& operator()(std::size_t x) { return m_values[index(x)]; }
    value_type safeAt(std::size_t x) const
    {
        internal::checkIndex1D(m_xSpace, x);
        return m_values[safeIndex(x)];
    }
    value_type& safeAt(std::size_t x)
    {
        internal::checkIndex1D(m_xSpace, x);
        return m_values[safeIndex(x)];
    }

    linear_list_type build() const noexcept
    {
        // The last value of m_values is considered mutable.
        // Since is not possible to do that in C++ we use const_cast.
        const_cast<value_container_type&>(m_values).back() =
          m_values[m_values.size() - 2];
        return linear_list_type(m_xSpace, m_values);
    }

private:
    /// \return Value index from linear space index
    std::size_t index(std::size_t x) const
    {
        assert(x >= 0);
        assert(x < m_xSpace.nrPoints());

        return internal::index1D(m_xSpace, x);
    }

    /// \return Value index from linear space index, don't check bounds
    std::size_t safeIndex(std::size_t x) const noexcept
    {
        return internal::index1D(m_xSpace, x);
    }

private:
    x_space_type m_xSpace;
    value_container_type m_values;
};

} // namespace tiny_sea
