// ///////////////////////////////////////////////////////////////////////////////////////
// LTB Distance Volume Hierarchy
// Copyright (c) 2020 Logan Barnes - All Rights Reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "distance_volume_hierarchy_cpu.hpp"
#include "ltb/dvh/distance_volume_hierarchy_util.hpp"

namespace ltb {
namespace dvh {

template <int L, typename T>
template <typename Geometry>
void DistanceVolumeHierarchyCpu<L, T>::subtract_volumes(std::vector<Geometry> const& geometries) {
    if (geometries.empty()) {
        return;
    }

    enum State : unsigned {
        DoesNotMatter    = 0u,
        PreviouslyInside = 1u,
    };

    CellSet        children_to_remove;
    CellSet        to_remove;
    CellMap<State> to_visit;
    CellMap<State> cells;

    for (int level = roots_.begin()->first; level >= lowest_level_; --level) {

        auto& distance_field = levels_[level];

        std::swap(to_remove, children_to_remove);

        for (const auto& cell_to_remove : to_remove) {
            distance_field.erase(cell_to_remove);

            auto children = children_cells(cell_to_remove);
            children_to_remove.insert(std::make_move_iterator(children.begin()),
                                      std::make_move_iterator(children.end()));
        }
        to_remove.clear();

        std::swap(cells, to_visit);

        if (roots_.find(level) != roots_.end()) {
            auto const& root_cells = roots_.at(level);
            for (const auto root_cell : root_cells) {
                cells.try_emplace(root_cell, State::DoesNotMatter);
            }
        }

        auto level_resolution = resolution(level);
        auto half_resolution  = level_resolution * T(0.5);
        auto cell_corner_dist = glm::length(glm::vec<L, T>(half_resolution));

        for (const auto& [cell, state] : cells) {
            auto const p = dvh::cell_center(cell, level_resolution);

            auto min_dist = std::numeric_limits<T>::infinity();

            for (auto const& geometry : geometries) {
                min_dist = std::min(min_dist, geometry.distance_from(p));
            }

            if (min_dist < -cell_corner_dist) {
                distance_field.erase(cell);

                auto children = children_cells(cell);
                children_to_remove.insert(std::make_move_iterator(children.begin()),
                                          std::make_move_iterator(children.end()));
            } else if (min_dist < cell_corner_dist) {
                State children_state = state;
                if (auto iter = distance_field.find(cell); iter != distance_field.end() && iter->second[L] < 0.f) {
                    children_state = State::PreviouslyInside;
                }

                if (auto dist_iter = distance_field.find(cell); dist_iter != distance_field.end()) {
                    distance_field[cell] = glm::vec<L + 1, T>(DistanceVolumeHierarchyCpu<L, T>::not_fully_inside);
                }

                auto children = children_cells(cell);
                for (const auto& child_cell : children) {
                    to_visit.insert_or_assign(child_cell, children_state);
                }
            } else {
                if (auto previous = distance_field.find(cell);
                    previous == distance_field.end() || previous->second[L] < -min_dist) {
                    if (state == State::PreviouslyInside) {
                        distance_field[cell] = glm::vec<L + 1, T>(p, -min_dist);
                    }
                }
            }
        }
        cells.clear();
    }

#if 0
    /*
     * Clean-up
     */
    CellMap<int> total_counts;
    CellMap<int> num_outside;

    for (int level = lowest_level_; level < roots_.begin()->first; ++level) {

        auto& distance_field = levels_[level];

        for (auto iter = distance_field.begin(); iter != distance_field.end();) {
            const auto& cell = iter->first;

            if (auto counts_iter = total_counts.find(cell); counts_iter != total_counts.end()) {
                if (auto outside_iter = num_outside.find(cell);
                    outside_iter != num_outside.end() && outside_iter->second == counts_iter->second) {
                    iter = distance_field.erase(iter);
                    continue;
                }
            }
            ++iter;
        }
        total_counts.clear();
        num_outside.clear();

        auto level_resolution = resolution(level);
        auto half_resolution  = level_resolution * T(0.5);
        auto cell_corner_dist = glm::length(glm::vec<L, T>(half_resolution));

        for (const auto& [cell, vector_and_distance] : distance_field) {
            auto parent = parent_cell(cell);
            total_counts[parent]++;

            if (std::isinf(vector_and_distance[L])) {
                num_outside[parent]++;
            }
        }

        // TODO: Delete the actual child nodes too
    }
#endif
}

} // namespace dvh
} // namespace ltb
