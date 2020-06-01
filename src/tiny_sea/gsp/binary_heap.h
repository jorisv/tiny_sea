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
#include <vector>

namespace tiny_sea {

namespace gsp {

/*! Default binary heap observer.
 */
struct NullBinaryHeapObserver
{
    void beforeErase(std::size_t /* index */) {}
    void afterEmplace(std::size_t /* index */) {}
    void beforeSwap(std::size_t /* index1 */, std::size_t /* index2 */) {}
};

/*! Classic binary heap implementation.
 * \tparam Observer Observe index manipulation in the container.
 * This class should have the following definition:
 * \code{.cpp}
 * struct Observer
 * {
 *   void beforeErase(std::size_t);
 *   void afterEmplace(std::size_t);
 *   void beforeSwap(std::size_t, std::size_t);
 * };
 * \code
 */
template<typename Type,
         typename Compare = std::less<Type>,
         typename Observer = NullBinaryHeapObserver>
class BinaryHeap
{
public:
    using container_type = std::vector<Type>;
    using value_compare = Compare;
    using operation_observer = Observer;
    using value_type = Type;
    using reference = Type&;
    using const_reference = const Type&;

public:
    BinaryHeap(const value_compare& compare = value_compare(),
               const operation_observer& observer = operation_observer())
      : m_compare(compare)
      , m_observer(observer)
    {}

    /// \return Check if the container is empty
    bool empty() const { return m_container.empty(); }

    /// \return Number of element
    std::size_t size() const { return m_container.size(); }

    /*! \return Minimal element
     * \warning Container must not be empty
     */
    const_reference top() const
    {
        assert(size() > 0);
        return m_container.front();
    }

    /// Insert \p value to the Heap
    void push(const_reference value)
    {
        const std::size_t currentIndex = m_container.size();

        // Add the element at the end of the heap
        m_container.push_back(value);
        m_observer.afterEmplace(currentIndex);

        // Restore heap property by up-heap
        upHeap(currentIndex, value);
    }

    /*! Remove minimal element from the Heap
     * \warning Container must not be empty
     */
    void pop()
    {
        assert(size() > 0);

        value_type cur = m_container.back();
        std::size_t containerSize = m_container.size() - 1;

        // currentIndex is the maximal element (the last one)
        std::size_t currentIndex = m_container.size() - 1;
        // targetIndex is the minimal element (the root)
        std::size_t targetIndex = 0;

        // Swap currentIndex with his minimal child until the tree is no more
        // violated
        while (currentIndex != targetIndex) {
            m_observer.beforeSwap(currentIndex, targetIndex);
            m_container[currentIndex] = m_container[targetIndex];
            m_container[targetIndex] = cur;

            currentIndex = targetIndex;

            targetIndex =
              minElement(targetIndex, left(currentIndex), containerSize);
            targetIndex =
              minElement(targetIndex, right(currentIndex), containerSize);
        }

        m_observer.beforeErase(m_container.size() - 1);
        m_container.pop_back();
    }

    /*! Update the value of an element of the heap and restore heap property.
     * \param index Index of the element to update.
     * \param value New value.
     * \warning \p index must be in the heap scope.
     * \warning \p value must have a lower value than the actual value.
     */
    void decrease(std::size_t index, const_reference value)
    {
        assert(index < m_container.size());
        assert(m_compare(value, m_container[index]));

        m_container[index] = value;
        upHeap(index, value);
    }

    /*! Same as \see decrease but when the value in the container as already
     * been updated.
     * \param index Index of the element to update.
     * \warning \p index must be in the heap scope.
     */
    void decrease(std::size_t index)
    {
        assert(index < m_container.size());

        // We must copy the value to avoid pointing on the container that will
        // be modified in upHeap method
        value_type v = m_container[index];
        upHeap(index, v);
    }

    /// \return Container instance
    const container_type& container() const { return m_container; }
    /// \return Compare instance
    const value_compare& compare() const { return m_compare; }
    /// \return Observer instance
    const operation_observer& observer() const { return m_observer; }
    /// Set the observer
    void observer(const operation_observer& observer) { m_observer = observer; }

private:
    /// \return Parent index
    std::size_t parent(std::size_t index) const { return (index - 1) / 2; }
    /// \return Left child index
    std::size_t left(std::size_t index) const { return (2 * index) + 1; }
    /// \return Right child index
    std::size_t right(std::size_t index) const { return (2 * index) + 2; }

    /// \return Index of the minimal element
    std::size_t minElement(std::size_t parent,
                           std::size_t child,
                           std::size_t containerSize) const
    {
        if (child < containerSize &&
            m_compare(m_container[child], m_container[parent])) {
            return child;
        }
        return parent;
    }

    /*! Run up-heap operation.
     * \param index \p value actual position in the heap.
     * \param value Value at \p index.
     */
    void upHeap(std::size_t index, const_reference value)
    {
        std::size_t currentIndex = index;

        // while currentIndex is not at the tree root we do up-heap operation
        while (currentIndex != 0) {
            // If inserted value is less than parent one we swap value
            // In the other case push is over
            std::size_t parentIndex = parent(currentIndex);
            if (m_compare(value, m_container[parentIndex])) {
                m_observer.beforeSwap(currentIndex, parentIndex);
                m_container[currentIndex] = m_container[parentIndex];
                m_container[parentIndex] = value;
                currentIndex = parentIndex;
            } else {
                return;
            }
        }
    }

private:
    container_type m_container;
    value_compare m_compare;
    operation_observer m_observer;
};

}

}
