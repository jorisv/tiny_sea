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
#include <cmath>

// tiny_sea
#include <tiny_sea/core/exception.h>
#include <tiny_sea/core/units.h>

namespace tiny_sea {

/*! Result of a LinearSpace interpolationWeight methods
 */
struct LinearSpaceInterpolationResult
{
    LinearSpaceInterpolationResult() = default;
    LinearSpaceInterpolationResult(scale_t p_percent, std::size_t p_index)
      : percent(p_percent)
      , index(p_index)
    {}

    scale_t percent;   //< Percent between two index.
    std::size_t index; //< Index of the smallest element.
};

/*! Compact representation of a set of points linearly spaced.
 * By example the following list will show a linear space that start at 2. and
 * contains 6 points spaced by 0.5:
 *
 *  [start()  1      2        3      4       stop()]
 *   2.      2.5     3.      3.5     4.      4.5
 */
template<typename Unit>
class LinearSpace
{
public:
    using value_type = Unit;

    /*!
     * \param[in] start Start of the linear space.
     * \param[in] delta Delta between linear space point.
     * \param[in] nrPoints Number of points.
     * \warning \p delta should be strictly positive.
     * \warning \p nrPoints should be at least 2.
     */
    LinearSpace(value_type start, value_type delta, std::size_t nrPoints)
      : m_start(start)
      , m_delta(delta)
      , m_nrPoints(nrPoints)
    {
        assert(m_delta.t > 0.);
        assert(m_nrPoints > 1);
    }

    value_type start() const noexcept { return m_start; }
    value_type delta() const noexcept { return m_delta; }
    value_type stop() const noexcept { return value(m_nrPoints - 1); }
    std::size_t nrPoints() const noexcept { return m_nrPoints; }

    // \return Linear space value at a current index
    value_type value(std::size_t index) const noexcept
    {
        return m_start + m_delta * scale_t(index);
    }

    /*! \return Linear space index associated to a linear space value.
     * \warning \p t must be in the space range [start, stop]
     */
    std::size_t index(value_type t) const
    {
        assert(inside(t));
        return std::min(
          std::size_t(std::floor(double((t - m_start) / m_delta))),
          m_nrPoints - 1);
    }

    /*! Safe version of \see index.
     * \param[in] t Clamped between [start(), stop()].
     */
    std::size_t safeIndex(value_type t) const noexcept
    {
        return index(clamp(t));
    }

    /// \return Interpolation weight of \p t
    LinearSpaceInterpolationResult interpolationWeight(value_type t) const
    {
        assert(inside(t));
        std::size_t idx = index(t);
        value_type t0 = value(idx);
        return LinearSpaceInterpolationResult((t - t0) / m_delta, idx);
    }

    /*! Safe version of \see interpolationWeight.
     * \param[in] t Clamped between [start(), stop()].
     */
    LinearSpaceInterpolationResult safeInterpolationWeight(
      value_type t) const noexcept
    {
        return interpolationWeight(clamp(t));
    }

    /// \return Check if a value is inside the linear space [start, stop]
    bool inside(value_type t) const noexcept
    {
        return t >= m_start && t <= stop();
    }

private:
    /// \return \p t clamped in the linear space [start, stop]
    value_type clamp(value_type t) const noexcept
    {
        return std::clamp(t, m_start, stop());
    }

private:
    value_type m_start;
    value_type m_delta;
    std::size_t m_nrPoints;
};

/// Helper method to create a LinearSpace
template<typename Unit>
inline LinearSpace<Unit>
makeLinearSpace(Unit start, Unit delta, std::size_t nrPoints)
{
    return LinearSpace<Unit>(start, delta, nrPoints);
}

/*! Create a linear space from start, stop and point number.
 * delta will be computed automatically.
 * \warning stop should superior to start.
 */
template<typename Unit>
inline LinearSpace<Unit>
makeLinearSpaceFromBound(Unit start, Unit stop, std::size_t nrPoints)
{
    assert(stop > start);
    return LinearSpace<Unit>(
      start, (stop - start) / scale_t(nrPoints - 1), nrPoints);
}

/// Safe version of \see makeLinearSpace
template<typename Unit>
inline LinearSpace<Unit>
safeMakeLinearSpace(Unit start, Unit delta, std::size_t nrPoints)
{
    if (delta <= Unit(0.)) {
        throw Exception("delta must be positive");
    }
    if (nrPoints <= 1) {
        throw Exception("nrPoints should at least be 2");
    }
    return makeLinearSpace(start, delta, nrPoints);
}

/// Safe version of \see makeLinearSpaceFromBound
template<typename Unit>
inline LinearSpace<Unit>
safeMakeLinearSpaceFromBound(Unit start, Unit stop, std::size_t nrPoints)
{
    if (start >= stop) {
        throw Exception("stop must be superior than start");
    }
    if (nrPoints <= 1) {
        throw Exception("nrPoints should at least be 2");
    }
    return makeLinearSpaceFromBound(start, stop, nrPoints);
}

}
