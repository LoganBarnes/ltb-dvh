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

// project
#include "ltb/dvh/distance_volume_hierarchy_util.hpp"
#include "ltb/sdf/sdf.hpp"

// external
#include <doctest/doctest.h>
#include <thrust/device_vector.h>
#include <thrust/host_vector.h>

//namespace ltb {
//namespace dvh {
//
//template <int L, typename T>
//struct VolumeCell {
//    Cell   index;
//    glm::vec<L + 1, T> direction_and_distance = glm::vec<L + 1, T>(std::numeric_limits<T>::infinity());
//
//    explicit VolumeCell(Cell cell) : index(cell) {}
//
//    explicit VolumeCell(Cell cell, glm::vec<L + 1, T> dir_and_dist)
//        : index(cell), direction_and_distance(dir_and_dist) {}
//};
//
//template <int L, typename T>
//LTB_CUDA_FUNC auto operator==(VolumeCell<L, T> const& lhs, VolumeCell<L, T> const& rhs) -> bool {
//    return lhs.index == rhs.index;
//}
//
//} // namespace dvh
//} // namespace ltb
//
//namespace std {
//
//template <int L, typename T>
//struct hash<ltb::dvh::VolumeCell<L, T>> {
//    size_t operator()(ltb::dvh::VolumeCell<L, T> const& cell) const { return hash<Cell>{}(cell.index); }
//};
//
//} // namespace std

namespace ltb {
namespace dvh {

template <int L, typename T>
DistanceVolumeHierarchyGpu<L, T>::DistanceVolumeHierarchyGpu(T base_resolution, int max_level)
    : base_resolution_(base_resolution), max_level_(max_level) {
    clear();
}

template <int L, typename T>
void DistanceVolumeHierarchyGpu<L, T>::clear() {
    cpu_levels_.clear();
}

template <int L, typename T>
auto DistanceVolumeHierarchyGpu<L, T>::levels() const -> LevelMap<SparseVolumeMap> const& {
    return cpu_levels_;
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

    auto& roots = cpu_roots_[root_level];

    iterate(min_cell, max_cell, [&roots](auto const& cell) { roots.emplace(cell); });
}

template class DistanceVolumeHierarchyGpu<2, float>;
template class DistanceVolumeHierarchyGpu<3, float>;
template class DistanceVolumeHierarchyGpu<2, double>;
template class DistanceVolumeHierarchyGpu<3, double>;

} // namespace dvh
} // namespace ltb
