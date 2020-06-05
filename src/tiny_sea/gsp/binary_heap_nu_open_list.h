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
#include <tiny_sea/gsp/binary_heap.h>
#include <tiny_sea/gsp/state.h>

namespace tiny_sea {

namespace gsp {

/*! Open list implementation for State.
 * This implementation use a binary heap to store state.
 * update method is not implemented.
 */
class BinaryHeapNUOpenList
{
public:
    /*! update method is not defined.
     * insert method return no valid iterator.
     */
    static constexpr bool isUpdate = false;

    using container_t = BinaryHeap<State, StateComparator>;

    class Iterator
    {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = State;
        using difference_type = int;
        using pointer = const State*;
        using reference = const State&;

    public:
        Iterator(pointer ptr)
          : m_ptr(ptr)
        {}

        reference operator*() { return *m_ptr; }
        pointer operator->() { return m_ptr; }
        Iterator& operator++()
        {
            ++m_ptr;
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator tmp(*this);
            ++m_ptr;
            return tmp;
        }
        bool operator==(const Iterator& o) const { return m_ptr == o.m_ptr; }
        bool operator!=(const Iterator& o) const { return m_ptr != o.m_ptr; }

    public:
        pointer m_ptr;
    };

    using iterator = Iterator;

public:
    BinaryHeapNUOpenList() = default;

    /*! Constructor from a list of State.
     * \tparam It Must be an iterator to State
     */
    template<
      typename It,
      std::enable_if_t<std::is_same_v<typename It::value_type, State>, int> = 0>
    BinaryHeapNUOpenList(It begin, It end)
    {
        for (; begin != end; ++begin) {
            m_store.push(*begin);
        }
    }

    // findGlobalShortestPath part

    bool empty() const { return m_store.empty(); }

    State pop()
    {
        State ret = m_store.top();
        m_store.pop();
        return ret;
    }

    std::pair<iterator, bool> insert(const State& state)
    {
        m_store.push(state);
        return std::make_pair(iterator(nullptr), true);
    }

    // Inspection part

    Iterator begin() const { return Iterator(&(m_store.container().front())); }
    Iterator end() const { return Iterator(&(m_store.container().back()) + 1); }

    std::size_t size() const { return m_store.size(); }

    const container_t& store() const { return m_store; }

private:
    container_t m_store;
};

}

}

