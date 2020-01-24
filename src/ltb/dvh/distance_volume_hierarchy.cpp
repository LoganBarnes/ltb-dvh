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

//#define DEBUG_PRINT

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
void DistanceVolumeHierarchy<L, T>::clear() {
    levels_.clear();
}

template <int L, typename T>
auto DistanceVolumeHierarchy<L, T>::levels() const -> LevelMap<SparseVolumeMap> const& {
    return levels_;
}

template <int L, typename T>
auto DistanceVolumeHierarchy<L, T>::base_resolution() const -> T {
    return base_resolution_;
}

template <int L, typename T>
auto DistanceVolumeHierarchy<L, T>::resolution(int level_index) const -> T {
    return base_resolution_ * std::pow<T>(2, level_index);
}

template <int L, typename T>
auto DistanceVolumeHierarchy<L, T>::gather_potential_cells(int                     level_index,
                                                           glm::vec<L, int> const& min_cell,
                                                           glm::vec<L, int> const& max_cell) -> LevelMap<CellSet> {
#ifdef DEBUG_PRINT
    std::cout << "min_cell: " << glm::to_string(min_cell) << std::endl;
    std::cout << "max_cell: " << glm::to_string(max_cell) << std::endl;
#endif

    LevelMap<CellSet> level_cells;

    auto cells = CellSet{};
#ifdef DEBUG_PRINT
    std::cout << "Level: " << std::to_string(level_index) << std::endl;
#endif

    iterate(min_cell, max_cell, [&cells](auto const& cell) {
#ifdef DEBUG_PRINT
        std::cout << "\t" << glm::to_string(cell) << std::endl;
#endif
        cells.emplace(cell);
    });

    auto num_cells = cells.size();
    level_cells.emplace(level_index, std::move(cells));

    for (int li = level_index + 1; li < max_level_; ++li) {
        auto const& smaller_cells          = level_cells.at(li - 1);
        auto const  lower_level_resolution = resolution(li - 1);

        cells.clear();
        auto const level_resolution = resolution(li);
#ifdef DEBUG_PRINT
        std::cout << "Level: " << std::to_string(li) << std::endl;
        std::cout << "Lower Res: " << std::to_string(lower_level_resolution) << std::endl;
        std::cout << "      Res: " << std::to_string(level_resolution) << std::endl;
#endif

        for (const auto& cell : smaller_cells) {
            auto world_pos   = cell_center(cell, lower_level_resolution);
            auto bigger_cell = get_cell(world_pos, level_resolution);
#ifdef DEBUG_PRINT
            std::cout << "\t" << glm::to_string(bigger_cell) << " from " << glm::to_string(cell) << std::endl;
#endif
            cells.emplace(bigger_cell);
        }

        if (num_cells == cells.size()) {
            break;
        }

        num_cells = cells.size();
        level_cells.emplace(li, std::move(cells));
    }

    // TODO: Handle smaller indices

    return level_cells;
}

template <int L, typename T>
auto DistanceVolumeHierarchy<L, T>::add_roots_for_bounds(const sdf::AABB<L, T>& aabb) -> void {

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

template class DistanceVolumeHierarchy<2, float>;
template class DistanceVolumeHierarchy<3, float>;
template class DistanceVolumeHierarchy<2, double>;
template class DistanceVolumeHierarchy<3, double>;

} // namespace ltb::dvh
