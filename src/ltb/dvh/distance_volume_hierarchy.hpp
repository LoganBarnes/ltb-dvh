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
#include <ltb/sdf/sdf.hpp>

// external
#include "ltb/util/comparison_utils.hpp"
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/string_cast.hpp>

// standard
#include <iostream>
#include <map>
#include <unordered_set>

namespace ltb::dvh {

template <int L, typename T>
auto cell_center(glm::vec<L, int> const& cell, const T& resolution) -> glm::vec<L, T> {
    return (glm::vec<L, T>(cell) + T(0.5)) * resolution;
}

template <int L, typename T = float>
class DistanceVolumeHierarchy {
public:
    using SparseVolumeMap = std::unordered_map<glm::vec<L, int>, glm::vec<L + 1, T>>;
    using CellSet         = std::unordered_set<glm::vec<L, int>>;
    template <typename V>
    using LevelMap = std::map<int, V>;

    explicit DistanceVolumeHierarchy(T base_resolution, int max_level = std::numeric_limits<int>::max());

    void clear() {
        levels_ = {{1, SparseVolumeMap{}}}; // The empty base level
    }

    /**
     * @brief All volumes added at the same time will be grouped together under the same root
     * @tparam Geom
     * @param geometries
     */
    template <typename Geom>
    void add_volumes(std::vector<Geom> const& geometries);

    auto               levels() const -> LevelMap<SparseVolumeMap> const&;
    [[nodiscard]] auto lowest_level() const -> int;

    auto base_resolution() const -> T;

    auto resolution(int level_index) const -> T;

private:
    T   base_resolution_;
    int max_level_;

    LevelMap<SparseVolumeMap> levels_;

    auto gather_potential_cells(glm::vec<L, int> const& min_cell, glm::vec<L, int> const& max_cell)
        -> LevelMap<CellSet>;
};

template <int L, typename T>
template <typename Geom>
void DistanceVolumeHierarchy<L, T>::add_volumes(std::vector<Geom> const& geometries) {
    LevelMap<CellSet> potential_cells;

    for (auto const& geometry : geometries) {
        auto aabb = sdf::bounding_box(geometry);

        auto min_cell = glm::vec<L, int>(glm::floor((aabb.min_point - base_resolution_) / base_resolution_));
        auto max_cell = glm::vec<L, int>(glm::ceil((aabb.max_point + base_resolution_) / base_resolution_));

        auto level_cells = gather_potential_cells(min_cell, max_cell);

        for (auto& [level_index, cells] : level_cells) {
            potential_cells[level_index].insert(std::move_iterator(cells.begin()), std::move_iterator(cells.end()));
        }
    }

    // TODO: Fix this logic

    for (const auto& [level_index, cells] : potential_cells) {

        auto& level            = levels_.at(level_index);
        auto  level_resolution = resolution(level_index);
        auto  half_resolution  = level_resolution * T(0.5);

        for (const auto& cell : cells) {
            // level.emplace(cell, glm::vec<L, T>(std::numeric_limits<T>::infinity()));

            auto const p = dvh::cell_center(cell, level_resolution);

            auto min_dist     = std::numeric_limits<T>::infinity();
            auto min_abs_dist = min_dist;

            for (auto const& geometry : geometries) {
                auto dist     = sdf::distance_to_geometry(p, geometry);
                auto abs_dist = std::abs(dist);

                bool equal = util::almost_equal(abs_dist, min_abs_dist);

                if ((!equal && abs_dist < min_abs_dist) || (equal && dist >= T(0))) {
                    min_dist     = dist;
                    min_abs_dist = abs_dist;
                }
            }

            if (min_dist <= glm::length(glm::vec<L, T>(half_resolution))) {
                level.insert_or_assign(cell, glm::vec<L + 1, T>(p, min_dist));
            }
        }
    }
}

} // namespace ltb::dvh