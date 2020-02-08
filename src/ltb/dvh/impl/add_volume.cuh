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
#include "distance_volume_hierarchy_gpu.hpp"
#include "ltb/dvh/distance_volume_hierarchy_util.hpp"

namespace ltb {
namespace dvh {

template <int L, typename T>
template <typename Geometry>
void DistanceVolumeHierarchyGpu<L, T>::add_volume(std::vector<Geometry> const& cpu_geometries) {
    thrust::device_vector<Geometry> gpu_geometries(cpu_geometries.begin(), cpu_geometries.end());

    if (cpu_geometries.empty()) {
        return;
    }

    auto volume_bounds = sdf::AABB<L, T>();

    for (const auto& cpu_geometry : cpu_geometries) {
        auto aabb     = cpu_geometry.bounding_box();
        volume_bounds = sdf::expand(volume_bounds, aabb.min_point);
        volume_bounds = sdf::expand(volume_bounds, aabb.max_point);
    }

    add_roots_for_bounds(volume_bounds);

    // ///////////////////////////////////////////////// //

    CellSet to_visit;
    CellSet cells;
    CellSet children_to_remove;
    CellSet to_remove;

    for (int level = cpu_roots_.begin()->first; level >= lowest_level_; --level) {

        auto& distance_field = cpu_levels_[level];

        std::swap(to_remove, children_to_remove);

        for (const auto& cell_to_remove : to_remove) {
            distance_field.erase(cell_to_remove);

            auto children = children_cells(cell_to_remove);
            children_to_remove.insert(std::make_move_iterator(children.begin()),
                                      std::make_move_iterator(children.end()));
        }
        to_remove.clear();

        std::swap(cells, to_visit);

        if (cpu_roots_.find(level) != cpu_roots_.end()) {
            auto const& root_cells = cpu_roots_.at(level);
            cells.insert(root_cells.begin(), root_cells.end());
        }

        auto level_resolution = resolution(level);
        auto half_resolution  = level_resolution * T(0.5);
        auto cell_corner_dist = glm::length(glm::vec<L, T>(half_resolution));

        for (const auto& cell : cells) {
            auto const p = dvh::cell_center(cell, level_resolution);

            auto min_dist     = std::numeric_limits<T>::infinity();
            auto min_abs_dist = min_dist;

            for (auto const& cpu_geometry : cpu_geometries) {
                auto const dist     = cpu_geometry.distance_from(p);
                auto const abs_dist = std::abs(dist);

                if (should_replace_with(min_abs_dist, abs_dist, dist)) {
                    min_dist     = dist;
                    min_abs_dist = abs_dist;
                }
            }

            bool inside_volume = (min_dist < 0.f);

            auto value_to_store
                = (min_abs_dist <= cell_corner_dist ? DistanceVolumeHierarchyGpu<L, T>::not_fully_inside : min_dist);

            if (value_to_store == DistanceVolumeHierarchyGpu<L, T>::not_fully_inside) {
                // Cell may not be fully inside the volume, but it is close to the
                // border so descendants might be inside the volume.

                // Add if the cell does not already exist. If it does exist it is either an
                // inside value we should not overwrite or it is already a 'not_fully_inside' value.
                distance_field.emplace(cell, glm::vec<L + 1, T>(DistanceVolumeHierarchyGpu<L, T>::not_fully_inside));

            } else if (inside_volume) {
                // This is entirely contained by the volume

                // Add/replace the cell if it doesn't exist or the current value contains a smaller distance
                if (distance_field.find(cell) == distance_field.end()) {
                    distance_field[cell] = glm::vec<L + 1, T>(p, value_to_store);
                } else if (distance_field.at(cell)[L] > min_abs_dist) {
                    auto old_dist        = distance_field.at(cell)[L];
                    distance_field[cell] = glm::vec<L + 1, T>(p, value_to_store);

                    if (old_dist == DistanceVolumeHierarchyGpu<L, T>::not_fully_inside) {
                        auto children = children_cells(cell);
                        children_to_remove.insert(std::make_move_iterator(children.begin()),
                                                  std::make_move_iterator(children.end()));
                    }
                }
            }

            if (distance_field.find(cell) != distance_field.end()
                && distance_field.at(cell)[L] == DistanceVolumeHierarchyGpu<L, T>::not_fully_inside) {

                auto children = children_cells(cell);
                to_visit.insert(std::make_move_iterator(children.begin()), std::make_move_iterator(children.end()));
            }
        }
        cells.clear();
    }
}

} // namespace dvh
} // namespace ltb
