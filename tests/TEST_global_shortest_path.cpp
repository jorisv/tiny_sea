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

// includes
// std
#include <algorithm>
#include <optional>
#include <queue>
#include <unordered_map>
#include <unordered_set>

// GTest
#include <gtest/gtest.h>

// tiny_sea
#include <tiny_sea/gsp/global_shortest_path.h>

using namespace tiny_sea;
using namespace tiny_sea::gsp;

namespace {

struct StatePosition
{
    StatePosition(std::uint32_t p_x, std::uint32_t p_y)
      : x(p_x)
      , y(p_y)
    {}

    StatePosition operator+(const StatePosition& o) const
    {
        return StatePosition(x + o.x, y + o.y);
    }

    bool operator==(const StatePosition& o) const
    {
        return x == o.x && y == o.y;
    }

    std::uint64_t hash() const
    {
        return static_cast<std::uint64_t>(x) << 32 |
               static_cast<std::uint64_t>(y);
    }

    std::uint32_t x, y;
};

struct State
{
    State(StatePosition p_position, std::uint32_t p_g, StatePosition p_parent)
      : position(p_position)
      , g(p_g)
      , parent_position(p_parent)
    {}

    State(StatePosition p_position, std::uint32_t p_g)
      : position(p_position)
      , g(p_g)
    {}

    bool operator==(const State& o) const { return position == o.position; }

    bool same(const State& o) const { return position == o.position; }
    bool better(const State& o) const { return g < o.g; }

    std::uint64_t hash() const { return position.hash(); }

    StatePosition position;
    std::uint32_t g;
    std::optional<StatePosition> parent_position;
};

struct StateCompare : public std::greater<State>
{
    bool operator()(const State& s1, const State& s2) const
    {
        return s2.better(s1);
    }
};

using StateContainer = std::unordered_map<std::uint64_t, State>;
class StateContainerIterator
{
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = State;
    using difference_type = int;
    using pointer = State*;
    using reference = State&;

public:
    StateContainerIterator(StateContainer::iterator it)
      : m_it(it)
    {}

    reference operator*() { return m_it->second; }
    pointer operator->() { return &(m_it->second); }
    StateContainerIterator& operator++()
    {
        ++m_it;
        return *this;
    }

    StateContainerIterator operator++(int)
    {
        StateContainerIterator tmp(*this);
        ++m_it;
        return tmp;
    }
    bool operator==(const StateContainerIterator& o) const
    {
        return m_it == o.m_it;
    }
    bool operator!=(const StateContainerIterator& o) const
    {
        return m_it != o.m_it;
    }

private:
    StateContainer::iterator m_it;
};

class OpenList
{
public:
    static constexpr bool isUpdate = true;

public:
    bool empty() const { return m_store.empty(); }

    State pop()
    {
        auto min_func = [](const StateContainer::value_type& v1,
                           const StateContainer::value_type& v2) {
            return v1.second.better(v2.second);
        };
        auto it = std::min_element(m_store.begin(), m_store.end(), min_func);
        State ret = it->second;
        m_store.erase(it);
        return ret;
    }

    std::pair<StateContainerIterator, bool> insert(const State& s)
    {
        ++m_nr_insert;
        const auto& pair = m_store.emplace(s.hash(), s);
        return std::make_pair(StateContainerIterator(pair.first), pair.second);
    }

    void update(StateContainerIterator it, const State& s)
    {
        ++m_nr_update;
        *it = s;
    }

    const StateContainer& container() const { return m_store; }

    std::uint32_t nrInsert() const { return m_nr_insert; }
    std::uint32_t nrUpdate() const { return m_nr_update; }

private:
    StateContainer m_store;
    std::uint32_t m_nr_insert = 0;
    std::uint32_t m_nr_update = 0;
};

class NUOpenList
{
public:
    static constexpr bool isUpdate = false;

public:
    using container_type =
      std::priority_queue<State, std::vector<State>, StateCompare>;
    class Iterator
    {};

public:
    bool empty() const { return m_store.empty(); }

    State pop()
    {
        State ret = m_store.top();
        m_store.pop();
        return ret;
    }

    std::pair<Iterator, bool> insert(const State& s)
    {
        ++m_nr_insert;
        m_store.push(s);
        return std::make_pair(Iterator(), true);
    }

    const container_type& container() const { return m_store; }

    std::uint32_t nrInsert() const { return m_nr_insert; }

private:
    container_type m_store;
    std::uint32_t m_nr_insert = 0;
};

class CloseList
{
public:
    bool contains(const State& s) const
    {
        return m_store.find(s.hash()) != m_store.end();
    }

    std::pair<StateContainerIterator, bool> insert(const State& s)
    {
        ++m_nr_insert;
        const auto& pair = m_store.emplace(s.hash(), s);
        return std::make_pair(StateContainerIterator(pair.first), pair.second);
    }

    const StateContainer& container() const { return m_store; }

    std::uint32_t nrInsert() const { return m_nr_insert; }

private:
    StateContainer m_store;
    std::uint32_t m_nr_insert = 0;
};

class NeighborsFinder
{
public:
    NeighborsFinder(std::uint32_t x_size,
                    std::uint32_t y_size,
                    const std::vector<StatePosition> obstacles)
      : m_x_size(x_size)
      , m_y_size(y_size)
    {
        for (const auto& o : obstacles) {
            m_obstacles_hash.emplace(o.hash());
        }
    }

    void search(StateContainerIterator it, std::vector<State>& neighbors)
    {
        for (auto offset : { StatePosition(1, 0),
                             StatePosition(0, 1),
                             StatePosition(-1, 0),
                             StatePosition(0, -1) }) {
            auto new_pos = it->position + offset;
            // If new_pos is inside map bound and
            if (new_pos.x >= 0 && new_pos.x < m_x_size && new_pos.y >= 0 &&
                new_pos.y < m_y_size) {
                // if new_pos is not on an obstacle
                if (m_obstacles_hash.find(new_pos.hash()) ==
                    m_obstacles_hash.end()) {

                    neighbors.emplace_back(new_pos, it->g + 1, it->position);
                }
            }
        }
    }

private:
    std::uint32_t m_x_size, m_y_size;
    std::unordered_set<std::uint64_t> m_obstacles_hash;
};

}

/*! Test that we find a solution on a simple 3x3 grid
 *   | | |g|
 *   | | | |
 *   |s| | |
 *
 *   Result should be
 *
 *   |c(2)|c(3)|c(4)|
 *   |c(1)|c(2)|c(3)|
 *   |c(0)|c(1)|c(2)|
 */
TEST(GLOBAL_SHORTEST_PATH_TESTS, TEST_short1)
{
    // Init the open list with the start position
    OpenList openList;
    openList.insert(State(StatePosition(0, 0), 0));
    // Create close list
    CloseList closeList;

    // Create neighbors finder with a 3x3 grid and no obstacle
    NeighborsFinder neighbor(3, 3, {});

    auto res = findGlobalShortestPath(
      State(StatePosition(2, 2), 0), openList, closeList, neighbor);

    EXPECT_TRUE(res);

    EXPECT_EQ(closeList.container().size(), 9);
    EXPECT_EQ(closeList.nrInsert(), 9);
    EXPECT_EQ(openList.container().size(), 0);
    EXPECT_EQ(openList.nrInsert(), 12 + 1); // 1 initial insert
    EXPECT_EQ(openList.nrUpdate(), 0);
}

/*! No update version
 * Test that we find a solution on a simple 3x3 grid
 *   | | |g|
 *   | | | |
 *   |s| | |
 *
 *   Result should be
 *
 *   |c(2)|c(3)|c(4)|
 *   |c(1)|c(2)|c(3)|
 *   |c(0)|c(1)|c(2)|
 */
TEST(GLOBAL_SHORTEST_PATH_TESTS, TEST_short_nu_1)
{
    // Init the open list with the start position
    NUOpenList openList;
    openList.insert(State(StatePosition(0, 0), 0));
    // Create close list
    CloseList closeList;

    // Create neighbors finder with a 3x3 grid and no obstacle
    NeighborsFinder neighbor(3, 3, {});

    auto res = findGlobalShortestPath(
      State(StatePosition(2, 2), 0), openList, closeList, neighbor);

    EXPECT_TRUE(res);

    EXPECT_EQ(closeList.container().size(), 9);
    // 3 insert of state already in the list (1,1), (1,2) and (2,1)
    EXPECT_EQ(closeList.nrInsert(), 9 + 3);
    // (2,2)
    EXPECT_EQ(openList.container().size(), 1);
    EXPECT_EQ(openList.nrInsert(), 12 + 1); // 1 initial insert
}

/*! Test that we find a solution on a 3x3 grid with 3 obstacles
 *   | | |g|
 *   |X|X| |
 *   |s| | |
 *
 *   Result should be
 *
 *   |    |    |c(4)|
 *   |    |    |c(3)|
 *   |c(0)|c(1)|c(2)|
 */
TEST(GLOBAL_SHORTEST_PATH_TESTS, TEST_short2)
{
    // Init the open list with the start position
    OpenList openList;
    openList.insert(State(StatePosition(0, 0), 0));
    // Create close list
    CloseList closeList;

    // Create neighbors finder with a 3x3 grid and 2 obstacles
    NeighborsFinder neighbor(
      3, 3, { StatePosition(0, 1), StatePosition(1, 1) });

    findGlobalShortestPath(
      State(StatePosition(2, 2), 0), openList, closeList, neighbor);

    EXPECT_EQ(closeList.container().size(), 5);
    EXPECT_EQ(closeList.nrInsert(), 5);
    EXPECT_EQ(openList.container().size(), 0);
    EXPECT_EQ(openList.nrInsert(), 4 + 1); // 1 initial insert
    EXPECT_EQ(openList.nrUpdate(), 0);
}

/*! No update version
 * Test that we find a solution on a 3x3 grid with 3 obstacles
 *   | | |g|
 *   |X|X| |
 *   |s| | |
 *
 *   Result should be
 *
 *   |    |    |c(4)|
 *   |    |    |c(3)|
 *   |c(0)|c(1)|c(2)|
 */
TEST(GLOBAL_SHORTEST_PATH_TESTS, TEST_short_nu_2)
{
    // Init the open list with the start position
    NUOpenList openList;
    openList.insert(State(StatePosition(0, 0), 0));
    // Create close list
    CloseList closeList;

    // Create neighbors finder with a 3x3 grid and 2 obstacles
    NeighborsFinder neighbor(
      3, 3, { StatePosition(0, 1), StatePosition(1, 1) });

    findGlobalShortestPath(
      State(StatePosition(2, 2), 0), openList, closeList, neighbor);

    EXPECT_EQ(closeList.container().size(), 5);
    EXPECT_EQ(closeList.nrInsert(), 5);
    EXPECT_EQ(openList.container().size(), 0);
    EXPECT_EQ(openList.nrInsert(), 4 + 1); // 1 initial insert
}

/*! Test that we find a solution on a 3x3 grid with 2 start point
 *   |s(4)| |g|
 *   |    | | |
 *   |s(0)| | |
 *
 *   Result should be
 *
 *   |c(2)|c(3)|c(4)|
 *   |c(1)|c(2)|c(3)|
 *   |c(0)|c(1)|c(2)|
 *
 * the top start should be updated
 */
TEST(GLOBAL_SHORTEST_PATH_TESTS, TEST_short3)
{
    // Init the open list with the start position
    OpenList openList;
    openList.insert(State(StatePosition(0, 0), 0));
    openList.insert(State(StatePosition(0, 2), 4));
    // Create close list
    CloseList closeList;

    // Create neighbors finder with a 3x3 grid and no obstacle
    NeighborsFinder neighbor(3, 3, {});

    auto res = findGlobalShortestPath(
      State(StatePosition(2, 2), 0), openList, closeList, neighbor);

    EXPECT_TRUE(res);

    EXPECT_EQ(closeList.container().size(), 9);
    EXPECT_EQ(closeList.nrInsert(), 9);
    EXPECT_EQ(openList.container().size(), 0);
    EXPECT_EQ(openList.nrInsert(), 12 + 2); // 2 initial insert
    EXPECT_EQ(openList.nrUpdate(), 1);
}

/*! No update version
 * Test that we find a solution on a 3x3 grid with 2 start point
 *   |s(4)| |g|
 *   |    | | |
 *   |s(0)| | |
 *
 *   Result should be
 *
 *   |c(2)|c(3)|c(4)|
 *   |c(1)|c(2)|c(3)|
 *   |c(0)|c(1)|c(2)|
 */
TEST(GLOBAL_SHORTEST_PATH_TESTS, TEST_short_nu_3)
{
    // Init the open list with the start position
    NUOpenList openList;
    openList.insert(State(StatePosition(0, 0), 0));
    openList.insert(State(StatePosition(0, 2), 4));
    // Create close list
    CloseList closeList;

    // Create neighbors finder with a 3x3 grid and no obstacle
    NeighborsFinder neighbor(3, 3, {});

    auto res = findGlobalShortestPath(
      State(StatePosition(2, 2), 0), openList, closeList, neighbor);

    EXPECT_TRUE(res);

    EXPECT_EQ(closeList.container().size(), 9);
    // 4 insert of state already in the list (1,1), (1,2), (2,1) and (0,2)
    EXPECT_EQ(closeList.nrInsert(), 9 + 4);
    EXPECT_EQ(openList.container().size(), 1);
    EXPECT_EQ(openList.nrInsert(), 12 + 2); // 2 initial insert
}
