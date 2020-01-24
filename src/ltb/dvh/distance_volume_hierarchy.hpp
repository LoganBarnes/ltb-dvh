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
#include "ltb/sdf/sdf.hpp"
#include "ltb/util/comparison_utils.hpp"
#include "ltb/util/container_utils.hpp"

// external
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/string_cast.hpp>

// standard
#include <iostream>
#include <map>
#include <unordered_set>

//#define SHOW_ALL

namespace ltb::dvh {

template <int L, typename T>
auto cell_center(glm::vec<L, int> const& cell, const T& resolution) -> glm::vec<L, T> {
    return (glm::vec<L, T>(cell) + glm::vec<L, T>(0.5)) * resolution;
}

template <int L, typename T>
auto get_cell(glm::vec<L, T> const& world_point, const T& resolution) -> glm::vec<L, int> {
    return glm::vec<L, int>(glm::floor(world_point / resolution));
}

template <typename T>
auto should_replace_with(T previous_absolute_distance, T new_absolute_distance, T new_distance) -> bool {
    bool equal = util::almost_equal(new_absolute_distance, previous_absolute_distance);

    if (std::isinf(previous_absolute_distance)) {
        if (!(!equal && new_absolute_distance < previous_absolute_distance)) {
            throw std::runtime_error(std::to_string(previous_absolute_distance) + ", "
                                     + std::to_string(new_absolute_distance) + ", " + std::to_string(new_distance));
        }
        assert(!equal && new_absolute_distance < previous_absolute_distance);
    }

    return (!equal && new_absolute_distance < previous_absolute_distance) || (equal && new_distance >= T(0));
}

template <int L, typename T = float>
class DistanceVolumeHierarchy {
public:
    using SparseVolumeMap = std::unordered_map<glm::vec<L, int>, glm::vec<L + 1, T>>;
    using CellSet         = std::unordered_set<glm::vec<L, int>>;
    template <typename V>
    using LevelMap = std::map<int, V, std::greater<int>>;

    explicit DistanceVolumeHierarchy(T base_resolution, int max_level = std::numeric_limits<int>::max());

    void clear();

    /**
     * @brief All volumes added at the same time will be grouped together under the same root
     * @tparam Geom
     * @param geometries
     */
    template <typename Geom>
    void add_volume(std::vector<Geom> const& geometries);

    auto levels() const -> LevelMap<SparseVolumeMap> const&;

    auto base_resolution() const -> T;

    auto resolution(int level_index) const -> T;

    constexpr static int base_level = 0;

private:
    T   base_resolution_;
    int max_level_;
    int lowest_level_ = 0;

    LevelMap<SparseVolumeMap> levels_;
    LevelMap<CellSet>         roots_;

    auto gather_potential_cells(int level_index, glm::vec<L, int> const& min_cell, glm::vec<L, int> const& max_cell)
        -> LevelMap<CellSet>;

    auto add_roots_for_bounds(sdf::AABB<L, T> const& aabb) -> void;
};

#if 1

template <int L, typename T>
template <typename Geom>
void DistanceVolumeHierarchy<L, T>::add_volume(std::vector<Geom> const& geometries) {
    if (geometries.empty()) {
        return;
    }

    auto volume_bounds = sdf::AABB<L, T>();

    for (auto const& geometry : geometries) {
        auto aabb     = sdf::bounding_box(geometry);
        volume_bounds = sdf::expand(volume_bounds, aabb.min_point);
        volume_bounds = sdf::expand(volume_bounds, aabb.max_point);
    }

    add_roots_for_bounds(volume_bounds);

    // ///////////////////////////////////////////////// //

    auto level = roots_.begin()->first;
    auto cells = roots_.begin()->second;

    auto level_resolution = resolution(level);
    auto half_resolution  = level_resolution * T(0.5);
    auto cell_corner_dist = glm::length(glm::vec<L, T>(half_resolution));

    auto& distance_field = levels_[level];

    for (const auto& cell : cells) {
        auto const p = dvh::cell_center(cell, level_resolution);

        auto min_dist     = std::numeric_limits<T>::infinity();
        auto min_abs_dist = min_dist;

        for (auto const& geometry : geometries) {
            auto dist     = sdf::distance_to_geometry(p, geometry);
            auto abs_dist = std::abs(dist);

            if (should_replace_with(min_abs_dist, abs_dist, dist)) {
                min_dist     = dist;
                min_abs_dist = abs_dist;
            }
        }

        if (!util::has_key(distance_field, cell)
            || should_replace_with(std::abs(distance_field.at(cell)[L]), min_abs_dist, min_dist)) {
            if (min_dist <= cell_corner_dist) {
                distance_field.insert_or_assign(cell, glm::vec<L + 1, T>(p, min_dist));

                if (min_abs_dist <= cell_corner_dist && level > lowest_level_) {
                    // Visit children
                }
            }
        }
    }
}

#else

template <int L, typename T>
template <typename Geom>
void DistanceVolumeHierarchy<L, T>::add_volume(std::vector<Geom> const& geometries) {
    if (geometries.empty()) {
        return;
    }

    LevelMap<CellSet> potential_cells;

    for (auto const& geometry : geometries) {
        auto aabb = sdf::bounding_box(geometry);

        auto min_cell = glm::vec<L, int>(glm::floor((aabb.min_point - base_resolution_) / base_resolution_));
        auto max_cell = glm::vec<L, int>(glm::ceil((aabb.max_point) / base_resolution_));

        auto level_cells = gather_potential_cells(DistanceVolumeHierarchy<L, T>::base_level, min_cell, max_cell);

        for (auto& [level_index, cells] : level_cells) {
            potential_cells[level_index].insert(std::move_iterator(cells.begin()), std::move_iterator(cells.end()));
        }
    }

    for (const auto& [level_index, cells] : potential_cells) {

        auto& level            = levels_[level_index];
        auto  level_resolution = resolution(level_index);
#ifndef SHOW_ALL
        auto  half_resolution  = level_resolution * T(0.5);
#endif

        for (const auto& cell : cells) {
            auto const p = dvh::cell_center(cell, level_resolution);

            auto min_dist     = std::numeric_limits<T>::infinity();
            auto min_abs_dist = min_dist;

            for (auto const& geometry : geometries) {
                auto dist     = sdf::distance_to_geometry(p, geometry);
                auto abs_dist = std::abs(dist);

                if (should_replace_with(min_abs_dist, abs_dist, dist)) {
                    min_dist     = dist;
                    min_abs_dist = abs_dist;
                }
            }

            if (!util::has_key(level, cell)
                || should_replace_with(std::abs(level.at(cell)[L]), min_abs_dist, min_dist)) {
#ifndef SHOW_ALL
                if (min_dist <= glm::length(glm::vec<L, T>(half_resolution))) {
#endif
                    level.insert_or_assign(cell, glm::vec<L + 1, T>(p, min_dist));
#ifndef SHOW_ALL
                }
#endif
            }
        }
    }
}

#endif

} // namespace ltb::dvh
