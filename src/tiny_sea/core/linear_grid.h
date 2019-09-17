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

template<typename XUnit, typename YUnit>
inline std::size_t
index2D(const XUnit& xSpace,
        const YUnit& ySpace,
        std::size_t x,
        std::size_t y) noexcept
{
    return x + y * (xSpace.nrPoints() + 1);
}

template<typename XUnit, typename YUnit>
inline void
checkIndex2D(const XUnit& xSpace,
             const YUnit& ySpace,
             std::size_t x,
             std::size_t y)
{
    if (x < 0 || y < 0 || x >= xSpace.nrPoints() || y >= ySpace.nrPoints()) {
        std::ostringstream str;
        str << "Index (" << x << ", " << y << ") is not in grid of shape ("
            << xSpace.nrPoints() << ", " << ySpace.nrPoints() << ")";
        throw std::out_of_range(str.str());
    }
}

}

/*! LinearGrid associate a value to all points of two linear space
 * A LinearGrid can be see as follow:
 *
 *          [start()   1   2   stop()]
 * yspace
 *   22        -4      18  9    8       stop()
 *   21        22      1   8    9         1
 *   20        2       4   5    17      start()]
 *
 * xspace  :   0       2   4    6
 *
 *  -4. is the value associated to x [0  2] and y 22
 *  22. is the value associated to x [0  2[ and y [21  22[
 *   5. is the value associated to x [4  6[ and y [20  21[
 *   9. is the value associated to x 6      and y [21  22[
 */
template<typename XUnit,
         typename YUnit,
         typename DataType,
         typename Interpolator = NumericInterpolator<DataType>>
class LinearGrid
{
public:
    using x_value_type = XUnit;
    using y_value_type = YUnit;
    using value_type = DataType;
    using interpolator_type = Interpolator;

    using x_space_type = LinearSpace<x_value_type>;
    using y_space_type = LinearSpace<y_value_type>;
    using value_container_type = std::vector<value_type>;

public:
    /*! Create a LinearGrid from two LinearSpace and a vector of values.
     * \warning \p values must have the \p (xSpace size + 1)*(ySpace size + 1)
     * size. The last column and row should be duplicated.
     */
    LinearGrid(const x_space_type& xSpace,
               const y_space_type& ySpace,
               const value_container_type& values)
      : m_xSpace(xSpace)
      , m_ySpace(ySpace)
      , m_values(values)
    {
        assert(m_values.size() ==
               ((m_xSpace.nrPoints() + 1) * (m_ySpace.nrPoints() + 1)));
    }

    /*! Getter from index.
     * \return Value associated to index x and y.
     * \warning \p x must be a valid index.
     * \warning \p y must be a valid index.
     */
    value_type operator()(std::size_t x, std::size_t y) const
    {
        return m_values[index(x, y)];
    }
    value_type safeAt(std::size_t x, std::size_t y) const
    {
        internal::checkIndex2D(m_xSpace, m_ySpace, x, y);
        // We can't use at since m_values is bigger than is supposed size
        return m_values[safeIndex(x, y)];
    }

    /*! Getter from linear space value.
     * \return Value associated linear space value.
     * \warning \p x must be inside the linear space.
     * \warning \p y must be inside the linear space.
     */
    value_type operator()(x_value_type x, y_value_type y) const
    {
        return m_values[index(x, y)];
    }
    value_type at(x_value_type x, y_value_type y) const
    {
        return m_values[index(x, y)];
    }

    /*! Safe getter from linear space
     * \param[in] x Clamped between [start(), stop()].
     * \param[in] y Clamped between [start(), stop()].
     */
    value_type safeAt(x_value_type x, y_value_type y) const noexcept
    {
        return m_values[safeIndex(x, y)];
    }

    /*!
     * \return Interpolated value
     * \warning \p x must be inside the linear space.
     * \warning \p y must be inside the linear space.
     */
    value_type interpolated(x_value_type x, y_value_type y) const
    {
        auto resX = m_xSpace.interpolationWeight(x);
        auto resY = m_ySpace.interpolationWeight(y);
        return intepolate(resX, resY);
    }

    /*!
     * \param[in] x Clamped between [start(), stop()].
     * \param[in] y Clamped between [start(), stop()].
     * \return Interpolated value
     */
    value_type safeInterpolated(x_value_type x, y_value_type y) const noexcept
    {
        auto resX = m_xSpace.safeInterpolationWeight(x);
        auto resY = m_ySpace.safeInterpolationWeight(y);
        return intepolate(resX, resY);
    }

    /// X linear space getter
    const x_space_type& xSpace() const noexcept { return m_xSpace; }

    /// Y linear space getter
    const y_space_type& ySpace() const noexcept { return m_ySpace; }

    /// Values getter
    const value_container_type& values() const noexcept { return m_values; }

private:
    /// \return Value index from linear space index
    std::size_t index(std::size_t x, std::size_t y) const
    {
        assert(x >= 0);
        assert(y >= 0);
        assert(x < m_xSpace.nrPoints());
        assert(y < m_ySpace.nrPoints());

        return internal::index2D(m_xSpace, m_ySpace, x, y);
    }

    /// \return Value index from linear space index, don't check bounds
    std::size_t safeIndex(std::size_t x, std::size_t y) const noexcept
    {
        return internal::index2D(m_xSpace, m_ySpace, x, y);
    }

    /// \return Value index from linear space value
    std::size_t index(x_value_type x, y_value_type y) const
    {
        return index(m_xSpace.index(x), m_ySpace.index(y));
    }

    /*! \return Value index from linear space value
     * \param[in] x Clamped between [start(), stop()].
     * \param[in] y Clamped between [start(), stop()].
     */
    std::size_t safeIndex(x_value_type x, y_value_type y) const noexcept
    {
        return index(m_xSpace.safeIndex(x), m_ySpace.safeIndex(y));
    }

    /// \return Bilinear interpolation
    value_type intepolate(
      const LinearSpaceInterpolationResult& resX,
      const LinearSpaceInterpolationResult& resY) const noexcept
    {
        // Extract indexes
        auto idx_x0y0 = safeIndex(resX.index, resY.index);
        auto idx_x1y0 = safeIndex(resX.index + 1, resY.index);
        auto idx_x0y1 = safeIndex(resX.index, resY.index + 1);
        auto idx_x1y1 = safeIndex(resX.index + 1, resY.index + 1);

        interpolator_type interpolator;

        // Compute linear interpolation over X on y0 and y1
        auto y0 =
          interpolator(m_values[idx_x0y0], m_values[idx_x1y0], resX.percent);
        auto y1 =
          interpolator(m_values[idx_x0y1], m_values[idx_x1y1], resX.percent);

        // Compute linear interpolation over Y
        return interpolator(y0, y1, resY.percent);
    }

private:
    x_space_type m_xSpace;
    y_space_type m_ySpace;
    value_container_type m_values;
};

/*! Helper class to build LinearGrid.
 */
template<typename XUnit,
         typename YUnit,
         typename DataType,
         typename Interpolator = NumericInterpolator<DataType>>
class LinearGridBuilder
{
public:
    using x_value_type = XUnit;
    using y_value_type = YUnit;
    using value_type = DataType;
    using interpolator_type = Interpolator;

    using x_space_type = LinearSpace<x_value_type>;
    using y_space_type = LinearSpace<y_value_type>;
    using value_container_type = std::vector<value_type>;

    using linear_grid_type =
      LinearGrid<x_value_type, y_value_type, value_type, interpolator_type>;

public:
    LinearGridBuilder(const x_space_type& x_space, const y_space_type& y_space)
      : m_xSpace(x_space)
      , m_ySpace(y_space)
      , m_values((m_xSpace.nrPoints() + 1) * (m_ySpace.nrPoints() + 1))
    {}

    /*! Getter from index.
     * \warning \p x must be a valid index.
     * \warning \p y must be a valid index.
     */
    value_type operator()(std::size_t x, std::size_t y) const
    {
        return m_values[index(x, y)];
    }
    value_type& operator()(std::size_t x, std::size_t y)
    {
        return m_values[index(x, y)];
    }
    value_type safeAt(std::size_t x, std::size_t y) const
    {
        internal::checkIndex2D(m_xSpace, m_ySpace, x, y);
        // We can't use at since m_values is bigger than is supposed size
        return m_values[safeIndex(x, y)];
    }
    value_type& safeAt(std::size_t x, std::size_t y)
    {
        internal::checkIndex2D(m_xSpace, m_ySpace, x, y);
        // We can't use at since m_values is bigger than is supposed size
        return m_values[safeIndex(x, y)];
    }

    linear_grid_type build() const noexcept
    {
        // The last column and the last row of m_values is considered mutable.
        // Since is not possible to do that in C++ we use const_cast.
        value_container_type& mut_v =
          const_cast<value_container_type&>(m_values);

        // Fill last column
        {
            std::size_t lastX = m_xSpace.nrPoints();
            for (std::size_t y = 0; y < m_ySpace.nrPoints(); ++y) {
                mut_v[safeIndex(lastX, y)] = mut_v[safeIndex(lastX - 1, y)];
            }
        }

        // Fill last row
        {
            std::size_t lastY = m_ySpace.nrPoints();
            for (std::size_t x = 0; x < m_xSpace.nrPoints() + 1; ++x) {
                mut_v[safeIndex(x, lastY)] = mut_v[safeIndex(x, lastY - 1)];
            }
        }

        return linear_grid_type(m_xSpace, m_ySpace, m_values);
    }

private:
    std::size_t index(std::size_t x, std::size_t y) const
    {
        assert(x >= 0);
        assert(y >= 0);
        assert(x < m_xSpace.nrPoints());
        assert(y < m_ySpace.nrPoints());

        return internal::index2D(m_xSpace, m_ySpace, x, y);
    }

    std::size_t safeIndex(std::size_t x, std::size_t y) const noexcept
    {
        return internal::index2D(m_xSpace, m_ySpace, x, y);
    }

private:
    x_space_type m_xSpace;
    y_space_type m_ySpace;
    value_container_type m_values;
};

} // namespace tiny_sea
