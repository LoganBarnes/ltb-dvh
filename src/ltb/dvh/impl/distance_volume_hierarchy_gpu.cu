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
#include "distance_volume_hierarchy_gpu.hpp"

// external
#include <doctest/doctest.h>

namespace ltb {
namespace dvh {

template <int L, typename T>
DistanceVolumeHierarchyGpu<L, T>::DistanceVolumeHierarchyGpu(T base_resolution, int max_level)
    : base_resolution_(base_resolution), max_level_(max_level) {
    clear();
}

template <int L, typename T>
void DistanceVolumeHierarchyGpu<L, T>::clear() {
    levels_.clear();
}

template <int L, typename T>
void DistanceVolumeHierarchyGpu<L, T>::actually_add_volume(std::vector<sdf::Geometry<L, T> const*> const& geometries) {
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

    for (int level = roots_.begin()->first; level >= lowest_level_; --level) {

        cells = std::move(to_visit);
        to_visit.clear(); // Just to make sure

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

            // TODO: double check this logic for already existing cells with smaller distances
            // (make sure children are still visited if necessary)
            if (distance_field.find(cell) == distance_field.end()
                || should_replace_with(std::abs(distance_field.at(cell)[L]), min_abs_dist, min_dist)) {
                if (min_dist <= cell_corner_dist) {
                    distance_field[cell] = glm::vec<L + 1, T>(p, min_dist);

                    if (min_abs_dist <= cell_corner_dist && level > lowest_level_) {
                        auto                                                     children = children_cells(cell);
                        typedef typename std::vector<glm::vec<L, int>>::iterator iter_t;
                        to_visit.insert(std::move_iterator<iter_t>(children.begin()),
                                        std::move_iterator<iter_t>(children.end()));
                    }
                }
            }
        }
    }
}

template <int L, typename T>
auto DistanceVolumeHierarchyGpu<L, T>::levels() const -> LevelMap<SparseVolumeMap> const& {
    return levels_;
}

template <int L, typename T>
auto DistanceVolumeHierarchyGpu<L, T>::base_resolution() const -> T {
    return base_resolution_;
}

template <int L, typename T>
auto DistanceVolumeHierarchyGpu<L, T>::resolution(int level_index) const -> T {
    return base_resolution_ * std::pow<T>(2, level_index);
}

template <int L, typename T>
auto DistanceVolumeHierarchyGpu<L, T>::add_roots_for_bounds(const sdf::AABB<L, T>& aabb) -> void {

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

template class DistanceVolumeHierarchyGpu<2, float>;
template class DistanceVolumeHierarchyGpu<3, float>;
template class DistanceVolumeHierarchyGpu<2, double>;
template class DistanceVolumeHierarchyGpu<3, double>;

} // namespace dvh
} // namespace ltb
