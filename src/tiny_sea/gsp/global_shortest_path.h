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
#include <optional>
#include <type_traits>
#include <vector>

namespace tiny_sea {

namespace gsp {

template<typename State>
struct Result
{
    Result(State p_state)
      : state(p_state)
    {}

    State state;
};

/*! Find a global shortest path using Hybrid A* algorithm.
 * \tparam State
 * \code{.cpp}
 * struct State
 * {
 *    State& operator=(State);
 *    bool same(State) const;
 *    bool better(State) const;
 * };
 * \code
 *
 * \tparam OpenList
 * \code{.cpp}
 * struct OpenList
 * {
 *   static bool isUpdate;
 *
 *   bool empty() const;
 *   State pop();
 *   std::pair<iterator, bool> insert(State);
 *   void update(iterator, State); [optional]
 * };
 * \code
 *
 * \tparam CloseList
 * \code{.cpp}
 * struct CloseList
 * {
 *   bool contains(State) const;
 *   std::pair<iterator, bool> insert(State);
 * };
 * \code
 *
 * \tparam NeighborsFinder
 * \code{.cpp}
 * struct NeighborsFinder
 * {
 *   void search(CloseList::iterator, std::vector<State>& neighbors) const;
 * };
 * \code
 */
template<typename State,
         typename OpenList,
         typename CloseList,
         typename NeighborsFinder,
         std::enable_if_t<
           std::remove_cv_t<std::remove_reference_t<OpenList>>::isUpdate,
           int> = 0>
std::optional<Result<std::remove_cv_t<std::remove_reference_t<State>>>>
findGlobalShortestPath(State&& finalState,
                       OpenList&& openList,
                       CloseList&& closeList,
                       NeighborsFinder&& neighborsFinder)
{
    using state_type = std::remove_cv_t<std::remove_reference_t<State>>;
    std::vector<state_type> neighbors;

    while (!openList.empty()) {
        auto best = closeList.insert(openList.pop());

        // Quit on a success on final state
        if (best.first->same(finalState)) {
            return Result<state_type>(*(best.first));
        }

        // Find neighbors and add it to the open list
        neighbors.clear();
        neighborsFinder.search(best.first, neighbors);
        for (const state_type& s : neighbors) {
            if (!closeList.contains(s)) {
                auto is_insert = openList.insert(s);

                // If already in the open list but the new neighbor is better we
                // update the open list
                if (!is_insert.second && s.better(*is_insert.first)) {
                    openList.update(is_insert.first, s);
                }
            }
        }
    }
    return std::nullopt;
}

/*! Same function as \see findGlobalShortestPath but without OpenList::update
 * method.
 */
template<typename State,
         typename OpenList,
         typename CloseList,
         typename NeighborsFinder,
         std::enable_if_t<
           !std::remove_cv_t<std::remove_reference_t<OpenList>>::isUpdate,
           int> = 0>
std::optional<Result<std::remove_cv_t<std::remove_reference_t<State>>>>
findGlobalShortestPath(State&& finalState,
                       OpenList&& openList,
                       CloseList&& closeList,
                       NeighborsFinder&& neighborsFinder)
{
    using state_type = std::remove_cv_t<std::remove_reference_t<State>>;
    std::vector<state_type> neighbors;

    while (!openList.empty()) {
        auto best = closeList.insert(openList.pop());

        // If best state is not already inside the close list we proceed it
        if (best.second) {
            // Quit on a success on final state
            if (best.first->same(finalState)) {
                return Result<state_type>(*(best.first));
            }

            // Find neighbors and add it to the open list
            neighbors.clear();
            neighborsFinder.search(best.first, neighbors);
            for (const state_type& s : neighbors) {
                if (!closeList.contains(s)) {
                    // Insert without checking if there is a better solution in
                    // the open list
                    openList.insert(s);
                }
            }
        }
    }
    return std::nullopt;
}

}

}
