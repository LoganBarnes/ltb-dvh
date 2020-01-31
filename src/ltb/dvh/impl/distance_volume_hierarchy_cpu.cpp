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
#include "distance_volume_hierarchy_cpu.hpp"

// project
#include "ltb/dvh/distance_volume_hierarchy_util.hpp"

// external
#include <doctest/doctest.h>

namespace ltb::dvh {

template <int L, typename T>
DistanceVolumeHierarchyCpu<L, T>::DistanceVolumeHierarchyCpu(T base_resolution, int max_level)
    : base_resolution_(base_resolution), max_level_(max_level) {
    clear();
}

template <int L, typename T>
void DistanceVolumeHierarchyCpu<L, T>::clear() {
    levels_.clear();
}

template <int L, typename T>
auto DistanceVolumeHierarchyCpu<L, T>::levels() const -> LevelMap<SparseVolumeMap> const& {
    return levels_;
}

template <int L, typename T>
auto DistanceVolumeHierarchyCpu<L, T>::base_resolution() const -> T {
    return base_resolution_;
}

template <int L, typename T>
auto DistanceVolumeHierarchyCpu<L, T>::resolution(int level_index) const -> T {
    return base_resolution_ * std::pow<T>(2, level_index);
}

template <int L, typename T>
auto DistanceVolumeHierarchyCpu<L, T>::actually_add_volume(std::vector<sdf::Geometry<L, T> const*> const& geometries)
    -> void {

    if (geometries.empty()) {
        return;
    }

    auto volume_bounds = sdf::AABB<L, T>();

    for (auto const* geometry : geometries) {
        auto aabb     = geometry->bounding_box();
        volume_bounds = sdf::expand(volume_bounds, aabb.min_point);
        volume_bounds = sdf::expand(volume_bounds, aabb.max_point);
    }

    add_roots_for_bounds(volume_bounds);

    // ///////////////////////////////////////////////// //

    CellSet to_visit;
    CellSet cells;
    CellSet children_to_remove;

    for (int level = roots_.begin()->first; level >= lowest_level_; --level) {

        cells = std::move(to_visit);
        to_visit.clear(); // Just to make sure
        children_to_remove.clear();

        if (roots_.find(level) != roots_.end()) {
            auto const& root_cells = roots_.at(level);
            cells.insert(root_cells.begin(), root_cells.end());
        }

        auto level_resolution = resolution(level);
        auto half_resolution  = level_resolution * T(0.5);
        auto cell_corner_dist = glm::length(glm::vec<L, T>(half_resolution));

        auto& distance_field = levels_[level];

        for (const auto& cell : cells) {
            auto const p = dvh::cell_center(cell, level_resolution);

            auto min_dist     = std::numeric_limits<T>::infinity();
            auto min_abs_dist = min_dist;

            for (auto const* geometry : geometries) {
                auto const dist     = geometry->distance_from(p);
                auto const abs_dist = std::abs(dist);

                if (should_replace_with(min_abs_dist, abs_dist, dist)) {
                    min_dist     = dist;
                    min_abs_dist = abs_dist;
                }
            }

            bool inside_volume = (min_dist < 0.f);

            auto value_to_store
                = (min_abs_dist <= cell_corner_dist ? DistanceVolumeHierarchyCpu<L, T>::not_fully_inside : min_dist);

            if (value_to_store == DistanceVolumeHierarchyCpu<L, T>::not_fully_inside) {
                // Cell may not be fully inside the volume, but it is close to the
                // border so descendants might be inside the volume.

                // Add if the cell does not already exist. If it does exist it is either an
                // inside value we should not overwrite or it is already a 'not_fully_inside' value.
                distance_field.emplace(cell, glm::vec<L + 1, T>(DistanceVolumeHierarchyCpu<L, T>::not_fully_inside));

            } else if (inside_volume) {
                // This is entirely contained by the volume

                // Add/replace the cell if it doesn't exist or the current value contains a smaller distance
                if (distance_field.find(cell) == distance_field.end()
                    || std::abs(distance_field.at(cell)[L]) < min_abs_dist) {
                    distance_field[cell] = glm::vec<L + 1, T>(p, value_to_store);
                }
            }

            if (distance_field.find(cell) != distance_field.end()
                && distance_field.at(cell)[L] == DistanceVolumeHierarchyCpu<L, T>::not_fully_inside) {

                auto children = children_cells(cell);
                to_visit.insert(std::make_move_iterator(children.begin()), std::make_move_iterator(children.end()));
            }
        }
    }
}

template <int L, typename T>
auto DistanceVolumeHierarchyCpu<L, T>::add_roots_for_bounds(const sdf::AABB<L, T>& aabb) -> void {

    auto root_level = lowest_level_;
    auto min_cell   = glm::vec<L, int>();
    auto max_cell   = glm::vec<L, int>();
    auto dimensions = glm::vec<L, int>(std::numeric_limits<int>::max());

    for (int level = root_level; level < max_level_; ++level) {
        auto level_resolution = resolution(level);

        auto level_min_cell = get_cell(aabb.min_point, level_resolution);
        auto level_max_cell = get_cell(aabb.max_point, level_resolution);

        auto level_dimensions = level_max_cell - level_min_cell;

        if (level_dimensions == dimensions) {
            break;
        }

        root_level = level;
        min_cell   = level_min_cell;
        max_cell   = level_max_cell;
        dimensions = level_dimensions;
    }

    auto& roots = roots_[root_level];

    iterate(min_cell, max_cell, [&roots](auto const& cell) { roots.emplace(cell); });
}

template <int L, typename T>
auto DistanceVolumeHierarchyCpu<L, T>::actually_subtract_volumes(
    std::vector<sdf::Geometry<L, T> const*> const& geometries) -> void {

    if (geometries.empty()) {
        return;
    }

    CellSet to_visit;
    CellSet cells;

    for (int level = roots_.begin()->first; level >= lowest_level_; --level) {

        cells = std::move(to_visit);
        to_visit.clear(); // Just to make sure

        if (roots_.find(level) != roots_.end()) {
            auto const& root_cells = roots_.at(level);
            cells.insert(root_cells.begin(), root_cells.end());
        }

        auto level_resolution = resolution(level);
        // auto half_resolution  = level_resolution * T(0.5);
        // auto cell_corner_dist = glm::length(glm::vec<L, T>(half_resolution));

        // auto& distance_field = levels_[level];

        for (const auto& cell : cells) {
            auto const p = dvh::cell_center(cell, level_resolution);

            auto min_dist = std::numeric_limits<T>::infinity();

            for (auto const* geometry : geometries) {
                min_dist = std::min(min_dist, geometry->distance_from(p));
            }
        }
    }
}

template class DistanceVolumeHierarchyCpu<2, float>;
template class DistanceVolumeHierarchyCpu<3, float>;
template class DistanceVolumeHierarchyCpu<2, double>;
template class DistanceVolumeHierarchyCpu<3, double>;

} // namespace ltb::dvh