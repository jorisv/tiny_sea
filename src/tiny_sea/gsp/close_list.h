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
#include <type_traits>
#include <unordered_map>

// tiny_sea
#include <tiny_sea/gsp/state.h>

namespace tiny_sea {

namespace gsp {

/*! Close list implementation for State.
 * This implementation use an hash map with DiscretState as key.
 */
class CloseList
{
public:
    using container_t =
      std::unordered_map<DiscretState, State, DiscretStateHash>;

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

        reference operator*() { return m_it->second; }
        pointer operator->() { return &(m_it->second); }
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

    private:
        container_t::iterator m_it;
    };

    using iterator = Iterator;

public:
    CloseList() = default;

    /*! Constructor from a list of State.
     * \tparam It Must be an iterator to State
     */
    template<
      typename It,
      std::enable_if_t<std::is_same_v<typename It::value_type, State>, int> = 0>
    CloseList(It begin, It end)
    {
        for (; begin != end; ++begin) {
            m_store.emplace(begin->discretState(), *begin);
        }
    }

    // findGlobalShortestPath part

    bool contains(const State& state) const
    {
        return m_store.find(state.discretState()) != m_store.end();
    }

    std::pair<iterator, bool> insert(const State& state)
    {
        auto res = m_store.emplace(state.discretState(), state);
        return std::make_pair(iterator(res.first), res.second);
    }

    // Inspection part

    const State& at(const DiscretState& ds) const { return m_store.at(ds); }

    std::size_t size() const { return m_store.size(); }

    const container_t store() const { return m_store; }

private:
    container_t m_store;
};

}

}
