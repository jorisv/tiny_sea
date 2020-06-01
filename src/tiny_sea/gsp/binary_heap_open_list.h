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
#include <memory>
#include <unordered_map>

// tiny_sea
#include <tiny_sea/gsp/binary_heap.h>
#include <tiny_sea/gsp/state.h>

namespace tiny_sea {

namespace gsp {

/*! Open list implementation for State.
 * This implementation store state in an unordered dict and
 * sort them with a binary heap.
 * The synchronisation of both data structure is made with the binary heap
 * observer.
 */
class BinaryHeapOpenList
{
public:
    /*! update method is defined.
     * insert method must return valid iterator.
     */
    static constexpr bool isUpdate = true;

    struct DualState
    {
        DualState(const State& p_state, std::size_t p_index)
          : state(p_state)
          , binaryHeapIndex(p_index)
        {}

        State state;
        std::size_t binaryHeapIndex;
    };

    using container_t = std::
      unordered_map<DiscretState, std::unique_ptr<DualState>, DiscretStateHash>;

    struct BinaryHeapComparator
    {
        bool operator()(const DualState* s1, const DualState* s2) const
        {
            return s1->state.better(s2->state);
        }
    };
    struct BinaryHeapObserver
    {
        using heap_container_t = std::vector<DualState*>;

        BinaryHeapObserver() = default;
        BinaryHeapObserver(const heap_container_t* p_heap)
          : heap(p_heap)
        {}

        void beforeErase(std::size_t /* index */) {}
        void afterEmplace(std::size_t index)
        {
            (*heap)[index]->binaryHeapIndex = index;
        }
        void beforeSwap(std::size_t index1, std::size_t index2)
        {
            (*heap)[index1]->binaryHeapIndex = index2;
            (*heap)[index2]->binaryHeapIndex = index1;
        }

        const heap_container_t* heap;
    };
    using binary_heap_t =
      BinaryHeap<DualState*, BinaryHeapComparator, BinaryHeapObserver>;

    class Iterator
    {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = State;
        using difference_type = int;
        using pointer = State*;
        using reference = State&;

    public:
        Iterator(container_t::iterator it)
          : m_it(it)
        {}

        reference operator*() { return m_it->second->state; }
        pointer operator->() { return &(m_it->second->state); }
        Iterator& operator++()
        {
            ++m_it;
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator tmp(*this);
            ++m_it;
            return tmp;
        }
        bool operator==(const Iterator& o) const { return m_it == o.m_it; }
        bool operator!=(const Iterator& o) const { return m_it != o.m_it; }

        std::size_t binaryHeapIndex() const
        {
            return m_it->second->binaryHeapIndex;
        }

    public:
        container_t::iterator m_it;
    };

    using iterator = Iterator;

public:
    BinaryHeapOpenList()
    {
        m_heap.observer(BinaryHeapObserver(&m_heap.container()));
    }
    BinaryHeapOpenList(const BinaryHeapOpenList&) = delete;
    BinaryHeapOpenList& operator=(const BinaryHeapOpenList&) = delete;

    /*! Constructor from a list of State.
     * \tparam It Must be an iterator to State
     */
    template<
      typename It,
      std::enable_if_t<std::is_same_v<typename It::value_type, State>, int> = 0>
    BinaryHeapOpenList(It begin, It end)
    {
        m_heap.observer(BinaryHeapObserver(&m_heap.container()));
        for (; begin != end; ++begin) {
            insert(*begin);
        }
    }

    bool empty() const { return m_store.empty(); }

    State pop()
    {
        DualState* dState = m_heap.top();
        State ret = dState->state;
        m_heap.pop();
        m_store.erase(ret.discretState());
        return ret;
    }

    std::pair<iterator, bool> insert(const State& state)
    {
        auto res = m_store.emplace(state.discretState(),
                                   std::make_unique<DualState>(state, 0));
        if (res.second) {
            m_heap.push(res.first->second.get());
        }
        return std::make_pair(iterator(res.first), res.second);
    }

    void update(iterator it, const State& state)
    {
        *it = state;
        m_heap.decrease(it.binaryHeapIndex());
        ++m_nrUpdate;
    }

    std::size_t nrUpdate() const { return m_nrUpdate; }

    const container_t& store() const { return m_store; }

private:
    container_t m_store;
    binary_heap_t m_heap;
    std::size_t m_nrUpdate = 0;
};

}

}
