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
auto DistanceVolumeHierarchyCpu<L, T>::add_roots_for_bounds(const sdf::AABB<L, T>& aabb) -> void {

    auto root_level = lowest_level_;
    auto min_cell   = Cell();
    auto max_cell   = Cell();
    auto dimensions = Cell(std::numeric_limits<int>::max());

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

template class DistanceVolumeHierarchyCpu<2, float>;
template class DistanceVolumeHierarchyCpu<3, float>;
template class DistanceVolumeHierarchyCpu<2, double>;
template class DistanceVolumeHierarchyCpu<3, double>;

} // namespace ltb::dvh