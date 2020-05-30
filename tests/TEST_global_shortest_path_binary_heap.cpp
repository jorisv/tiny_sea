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
// GTest
#include <gtest/gtest.h>

// tiny_sea
#include <tiny_sea/gsp/binary_heap.h>

using namespace tiny_sea::gsp;

TEST(BINARY_HEAP_TESTS, TEST_push1)
{
    BinaryHeap<int> heap;

    // 3
    heap.push(3);
    EXPECT_EQ(heap.container(), std::vector<int>({ 3 }));

    //    3
    // 36
    heap.push(36);
    EXPECT_EQ(heap.container(), std::vector<int>({ 3, 36 }));

    //    3
    // 36   7
    heap.push(7);
    EXPECT_EQ(heap.container(), std::vector<int>({ 3, 36, 7 }));

    //     1
    //   3   7
    // 36
    heap.push(1);
    EXPECT_EQ(heap.container(), std::vector<int>({ 1, 3, 7, 36 }));

    //     1
    //   2   7
    // 36 3
    heap.push(2);
    EXPECT_EQ(heap.container(), std::vector<int>({ 1, 2, 7, 36, 3 }));

    //     1
    //   2   7
    // 36 3 25
    heap.push(25);
    EXPECT_EQ(heap.container(), std::vector<int>({ 1, 2, 7, 36, 3, 25 }));

    //     1
    //   2   4
    // 36 3 25 7
    heap.push(4);
    EXPECT_EQ(heap.container(), std::vector<int>({ 1, 2, 4, 36, 3, 25, 7 }));

    //      1
    //    2   7
    //   2 3 25 7
    // 36
    heap.push(2);
    EXPECT_EQ(heap.container(), std::vector<int>({ 1, 2, 4, 2, 3, 25, 7, 36 }));

    //      1
    //    2   7
    //   2 3 25 7
    // 36 19
    heap.push(19);
    EXPECT_EQ(heap.container(),
              std::vector<int>({ 1, 2, 4, 2, 3, 25, 7, 36, 19 }));
}

// Create the following binary heap:
//               0
//       10             5
//   35      15      45   25
// 55  40  30  25  70  50
class BinaryHeapFixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_heap.push(25);
        m_heap.push(30);
        m_heap.push(5);
        m_heap.push(40);
        m_heap.push(10);
        m_heap.push(50);
        m_heap.push(0);
        m_heap.push(55);
        m_heap.push(35);
        m_heap.push(15);
        m_heap.push(25);
        m_heap.push(70);
        m_heap.push(45);
    }

    BinaryHeap<int> m_heap;
};

TEST_F(BinaryHeapFixture, TEST_push2)
{
    EXPECT_EQ(
      m_heap.container(),
      std::vector<int>({ 0, 10, 5, 35, 15, 45, 25, 55, 40, 30, 25, 70, 50 }));
}

TEST_F(BinaryHeapFixture, TEST_empty)
{
    EXPECT_FALSE(m_heap.empty());
    EXPECT_TRUE(BinaryHeap<int>().empty());
}

TEST_F(BinaryHeapFixture, TEST_size)
{
    EXPECT_EQ(m_heap.size(), 13);
    EXPECT_EQ(BinaryHeap<int>().size(), 0);
}

TEST_F(BinaryHeapFixture, TEST_top)
{
    EXPECT_EQ(m_heap.top(), 0);
}

TEST_F(BinaryHeapFixture, TEST_pop_top)
{
    //               0
    //       10             5
    //   35      15      45   25
    // 55  40  30  25  70  50
    EXPECT_EQ(m_heap.top(), 0);

    //               5
    //       10             25
    //   35      15      45   50
    // 55  40  30  25  70
    m_heap.pop();
    EXPECT_EQ(m_heap.top(), 5);
    EXPECT_EQ(
      m_heap.container(),
      std::vector<int>({ 5, 10, 25, 35, 15, 45, 50, 55, 40, 30, 25, 70 }));

    //              10
    //       15             25
    //   35      25      45   50
    // 55  40  30  70
    m_heap.pop();
    EXPECT_EQ(m_heap.top(), 10);
    EXPECT_EQ(m_heap.container(),
              std::vector<int>({ 10, 15, 25, 35, 25, 45, 50, 55, 40, 30, 70 }));

    //              15
    //       25             25
    //   35      30      45   50
    // 55  40  70
    m_heap.pop();
    EXPECT_EQ(m_heap.top(), 15);
    EXPECT_EQ(m_heap.container(),
              std::vector<int>({ 15, 25, 25, 35, 30, 45, 50, 55, 40, 70 }));

    //              25
    //       30             25
    //   35      70      45   50
    // 55  40
    m_heap.pop();
    EXPECT_EQ(m_heap.top(), 25);
    EXPECT_EQ(m_heap.container(),
              std::vector<int>({ 25, 30, 25, 35, 70, 45, 50, 55, 40 }));

    //              25
    //       30             40
    //   35      70      45   50
    // 55
    m_heap.pop();
    EXPECT_EQ(m_heap.top(), 25);
    EXPECT_EQ(m_heap.container(),
              std::vector<int>({ 25, 30, 40, 35, 70, 45, 50, 55 }));

    //              30
    //       35             40
    //   55      70      45   50
    m_heap.pop();
    EXPECT_EQ(m_heap.top(), 30);
    EXPECT_EQ(m_heap.container(),
              std::vector<int>({ 30, 35, 40, 55, 70, 45, 50 }));

    //              35
    //       50             40
    //   55      70      45
    m_heap.pop();
    EXPECT_EQ(m_heap.top(), 35);
    EXPECT_EQ(m_heap.container(), std::vector<int>({ 35, 50, 40, 55, 70, 45 }));

    //              40
    //       50             45
    //   55      70
    m_heap.pop();
    EXPECT_EQ(m_heap.top(), 40);
    EXPECT_EQ(m_heap.container(), std::vector<int>({ 40, 50, 45, 55, 70 }));

    //              45
    //       50             70
    //   55
    m_heap.pop();
    EXPECT_EQ(m_heap.top(), 45);
    EXPECT_EQ(m_heap.container(), std::vector<int>({ 45, 50, 70, 55 }));

    //              50
    //       55             70
    m_heap.pop();
    EXPECT_EQ(m_heap.top(), 50);
    EXPECT_EQ(m_heap.container(), std::vector<int>({ 50, 55, 70 }));

    //              55
    //       70
    m_heap.pop();
    EXPECT_EQ(m_heap.top(), 55);
    EXPECT_EQ(m_heap.container(), std::vector<int>({ 55, 70 }));

    //              70
    m_heap.pop();
    EXPECT_EQ(m_heap.top(), 70);
    EXPECT_EQ(m_heap.container(), std::vector<int>({ 70 }));

    m_heap.pop();
    EXPECT_TRUE(m_heap.empty());
}

TEST_F(BinaryHeapFixture, TEST_decrease1)
{
    // Initial state
    //               0
    //       10             5
    //   35      15      45   25
    // 55  40  30  25  70  50

    //               0
    //       3             5
    //   10      15      45   25
    // 55  40  30  25  70  50
    // Decrease 35 to 3
    m_heap.decrease(3, 3);
    EXPECT_EQ(
      m_heap.container(),
      std::vector<int>({ 0, 3, 5, 10, 15, 45, 25, 55, 40, 30, 25, 70, 50 }));

    //               -1
    //       3              0
    //   10      15       5   25
    // 55  40  30  25  45  50
    // Decrease 70 to -1
    m_heap.decrease(11, -1);
    EXPECT_EQ(
      m_heap.container(),
      std::vector<int>({ -1, 3, 0, 10, 15, 5, 25, 55, 40, 30, 25, 45, 50 }));
}

TEST_F(BinaryHeapFixture, TEST_decrease2)
{
    // Same test than TEST_decrease1 but with decrease overload

    const_cast<std::vector<int>&>(m_heap.container())[3] = 3;
    m_heap.decrease(3);
    EXPECT_EQ(
      m_heap.container(),
      std::vector<int>({ 0, 3, 5, 10, 15, 45, 25, 55, 40, 30, 25, 70, 50 }));

    const_cast<std::vector<int>&>(m_heap.container())[11] = -1;
    m_heap.decrease(11);
    EXPECT_EQ(
      m_heap.container(),
      std::vector<int>({ -1, 3, 0, 10, 15, 5, 25, 55, 40, 30, 25, 45, 50 }));
}

struct TestBinaryHeapObserver
{
    void beforeErase(std::size_t index) { erased.push_back(index); }
    void afterEmplace(std::size_t index) { emplaced.push_back(index); }
    void beforeSwap(std::size_t index1, std::size_t index2)
    {
        swaped.emplace_back(index1, index2);
    }

    void clear() const
    {
        erased.clear();
        emplaced.clear();
        swaped.clear();
    }

    mutable std::vector<std::size_t> erased;
    mutable std::vector<std::size_t> emplaced;
    mutable std::vector<std::pair<std::size_t, std::size_t>> swaped;
};

TEST(BINARY_HEAP_TESTS, TEST_observer1)
{
    BinaryHeap<int, std::less<int>, TestBinaryHeapObserver> heap;

    // 25
    heap.push(25);
    EXPECT_EQ(heap.observer().erased, std::vector<std::size_t>({}));
    EXPECT_EQ(heap.observer().emplaced, std::vector<std::size_t>({ 0 }));
    EXPECT_EQ(heap.observer().swaped,
              (std::vector<std::pair<std::size_t, std::size_t>>({})));
    heap.observer().clear();

    //   25
    // 30
    heap.push(30);
    EXPECT_EQ(heap.observer().erased, std::vector<std::size_t>({}));
    EXPECT_EQ(heap.observer().emplaced, std::vector<std::size_t>({ 1 }));
    EXPECT_EQ(heap.observer().swaped,
              (std::vector<std::pair<std::size_t, std::size_t>>({})));
    heap.observer().clear();

    //   5
    // 30  25
    heap.push(5);
    EXPECT_EQ(heap.observer().erased, std::vector<std::size_t>({}));
    EXPECT_EQ(heap.observer().emplaced, std::vector<std::size_t>({ 2 }));
    EXPECT_EQ(heap.observer().swaped,
              (std::vector<std::pair<std::size_t, std::size_t>>({ { 2, 0 } })));
    heap.observer().clear();

    //     5
    //   30  25
    // 40
    heap.push(40);
    EXPECT_EQ(heap.observer().erased, std::vector<std::size_t>({}));
    EXPECT_EQ(heap.observer().emplaced, std::vector<std::size_t>({ 3 }));
    EXPECT_EQ(heap.observer().swaped,
              (std::vector<std::pair<std::size_t, std::size_t>>({})));
    heap.observer().clear();

    //     0
    //   5    25
    // 40  30
    heap.push(0);
    EXPECT_EQ(heap.observer().erased, std::vector<std::size_t>({}));
    EXPECT_EQ(heap.observer().emplaced, std::vector<std::size_t>({ 4 }));
    EXPECT_EQ(heap.observer().swaped,
              (std::vector<std::pair<std::size_t, std::size_t>>(
                { { 4, 1 }, { 1, 0 } })));
    heap.observer().clear();

    //      5
    //   30    25
    // 40
    heap.pop();
    EXPECT_EQ(heap.observer().erased, std::vector<std::size_t>({ 4 }));
    EXPECT_EQ(heap.observer().emplaced, std::vector<std::size_t>({}));
    EXPECT_EQ(heap.observer().swaped,
              (std::vector<std::pair<std::size_t, std::size_t>>(
                { { 4, 0 }, { 0, 1 } })));
    heap.observer().clear();

    //      25
    //   30    40
    heap.pop();
    EXPECT_EQ(heap.observer().erased, std::vector<std::size_t>({ 3 }));
    EXPECT_EQ(heap.observer().emplaced, std::vector<std::size_t>({}));
    EXPECT_EQ(heap.observer().swaped,
              (std::vector<std::pair<std::size_t, std::size_t>>(
                { { 3, 0 }, { 0, 2 } })));
    heap.observer().clear();

    //      30
    //   40
    heap.pop();
    EXPECT_EQ(heap.observer().erased, std::vector<std::size_t>({ 2 }));
    EXPECT_EQ(heap.observer().emplaced, std::vector<std::size_t>({}));
    EXPECT_EQ(heap.observer().swaped,
              (std::vector<std::pair<std::size_t, std::size_t>>(
                { { 2, 0 }, { 0, 1 } })));
    heap.observer().clear();

    //   40
    heap.pop();
    EXPECT_EQ(heap.observer().erased, std::vector<std::size_t>({ 1 }));
    EXPECT_EQ(heap.observer().emplaced, std::vector<std::size_t>({}));
    EXPECT_EQ(heap.observer().swaped,
              (std::vector<std::pair<std::size_t, std::size_t>>({ { 1, 0 } })));
    heap.observer().clear();

    // Empty
    heap.pop();
    EXPECT_EQ(heap.observer().erased, std::vector<std::size_t>({ 0 }));
    EXPECT_EQ(heap.observer().emplaced, std::vector<std::size_t>({}));
    EXPECT_EQ(heap.observer().swaped,
              (std::vector<std::pair<std::size_t, std::size_t>>({})));
    heap.observer().clear();
}
