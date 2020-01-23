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
#include "distance_volume_hierarchy.hpp"

// external
#include <doctest/doctest.h>

namespace ltb::dvh {

namespace {

template <typename Func>
void iterate(glm::ivec2 const& min_index, glm::ivec2 const& max_index, Func const& func) {
    for (int yi = min_index.y; yi <= max_index.y; ++yi) {
        for (int xi = min_index.x; xi <= max_index.x; ++xi) {
            func(glm::ivec2{xi, yi});
        }
    }
}

template <typename Func>
void iterate(glm::ivec3 const& min_index, glm::ivec3 const& max_index, Func const& func) {
    for (int zi = min_index.z; zi <= max_index.z; ++zi) {
        for (int yi = min_index.y; yi <= max_index.y; ++yi) {
            for (int xi = min_index.x; xi <= max_index.x; ++xi) {
                func(glm::ivec3{xi, yi, zi});
            }
        }
    }
}

} // namespace

template <int L, typename T>
DistanceVolumeHierarchy<L, T>::DistanceVolumeHierarchy(T base_resolution, int max_level)
    : base_resolution_(base_resolution), max_level_(max_level) {
    clear();
}

template <int L, typename T>
auto DistanceVolumeHierarchy<L, T>::levels() const -> LevelMap<SparseVolumeMap> const& {
    return levels_;
}

template <int L, typename T>
auto DistanceVolumeHierarchy<L, T>::lowest_level() const -> int {
    return levels_.begin()->first;
}

template <int L, typename T>
auto DistanceVolumeHierarchy<L, T>::base_resolution() const -> T {
    return base_resolution_;
}

template <int L, typename T>
auto DistanceVolumeHierarchy<L, T>::resolution(int level_index) const -> T {
    if (level_index == 0) {
        throw std::invalid_argument("level_index cannot be zero");
    }
    return base_resolution_ * (level_index > 0 ? T(level_index) : T(1) / level_index);
}

template <int L, typename T>
auto DistanceVolumeHierarchy<L, T>::gather_potential_cells(glm::vec<L, int> const& min_cell,
                                                           glm::vec<L, int> const& max_cell) -> LevelMap<CellSet> {

    LevelMap<CellSet> level_cells;

    auto cells = CellSet{};

    iterate(min_cell, max_cell, [&cells](auto const& cell) { cells.emplace(cell); });

    level_cells.emplace(1, std::move(cells));

    return level_cells;
}

// test compilation
template class DistanceVolumeHierarchy<2, float>;
template class DistanceVolumeHierarchy<3, float>;
template class DistanceVolumeHierarchy<2, double>;
template class DistanceVolumeHierarchy<3, double>;

} // namespace ltb::dvh
