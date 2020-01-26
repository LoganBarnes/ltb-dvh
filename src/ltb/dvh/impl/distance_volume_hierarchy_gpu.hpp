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
#include "ltb/dvh/distance_volume_hierarchy_util.hpp"

// external
#include <glm/gtx/hash.hpp>
//#include <thrust/device_vector.hpp>

// standard
#include <iterator>
#include <map>
#include <unordered_map>
#include <unordered_set>

namespace ltb {
namespace dvh {

template <int L, typename T>
class DistanceVolumeHierarchyGpu {
public:
    //    struct VolumeCell {
    //        glm::vec<L, int> index;
    //        glm::vec<L + 1, T> direction_and_distance;
    //    }

    using SparseVolumeMap = std::unordered_map<glm::vec<L, int>, glm::vec<L + 1, T>>;
    using CellSet         = std::unordered_set<glm::vec<L, int>>;
    template <typename V>
    using LevelMap = std::map<int, V, std::greater<int>>;

    explicit DistanceVolumeHierarchyGpu(T base_resolution, int max_level = std::numeric_limits<int>::max());

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

    auto add_roots_for_bounds(sdf::AABB<L, T> const& aabb) -> void;
};

template <int L, typename T>
template <typename Geom>
void DistanceVolumeHierarchyGpu<L, T>::add_volume(std::vector<Geom> const& geometries) {
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

            for (auto const& geometry : geometries) {
                auto dist     = sdf::distance_to_geometry(p, geometry);
                auto abs_dist = std::abs(dist);

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
                    distance_field.insert_or_assign(cell, glm::vec<L + 1, T>(p, min_dist));

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

template <int L, typename T = float>
using DistanceVolumeHierarchy = DistanceVolumeHierarchyGpu<L, T>;

} // namespace dvh
} // namespace ltb
