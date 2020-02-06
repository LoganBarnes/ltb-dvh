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

#define LTB_DVH_REGISTER_GEOMETRY_TYPE_2D(Type)                                                                        \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, float>::add_volume(                                        \
        const std::vector<Type<float>>& geometries);                                                                   \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, double>::add_volume(                                       \
        const std::vector<Type<double>>& geometries);                                                                  \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, float>::subtract_volume(                                   \
        const std::vector<Type<float>>& geometries);                                                                   \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, double>::subtract_volume(                                  \
        const std::vector<Type<double>>& geometries);

#define LTB_DVH_REGISTER_GEOMETRY_TYPE_3D(Type)                                                                        \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, float>::add_volume(                                        \
        const std::vector<Type<float>>& geometries);                                                                   \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, double>::add_volume(                                       \
        const std::vector<Type<double>>& geometries);                                                                  \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, float>::subtract_volume(                                   \
        const std::vector<Type<float>>& geometries);                                                                   \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, double>::subtract_volume(                                  \
        const std::vector<Type<double>>& geometries);

#define LTB_DVH_REGISTER_GEOMETRY_TYPE(Type)                                                                           \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, float>::add_volume(                                        \
        const std::vector<Type<2, float>>& geometries);                                                                \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, float>::add_volume(                                        \
        const std::vector<Type<3, float>>& geometries);                                                                \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, double>::add_volume(                                       \
        const std::vector<Type<2, double>>& geometries);                                                               \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, double>::add_volume(                                       \
        const std::vector<Type<3, double>>& geometries);                                                               \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, float>::add_volume(                                        \
        const std::vector<sdf::TransformedGeometry<Type, 2, float>>& geometries);                                      \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, float>::add_volume(                                        \
        const std::vector<sdf::TransformedGeometry<Type, 3, float>>& geometries);                                      \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, double>::add_volume(                                       \
        const std::vector<sdf::TransformedGeometry<Type, 2, double>>& geometries);                                     \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, double>::add_volume(                                       \
        const std::vector<sdf::TransformedGeometry<Type, 3, double>>& geometries);                                     \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, float>::subtract_volume(                                   \
        const std::vector<Type<2, float>>& geometries);                                                                \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, float>::subtract_volume(                                   \
        const std::vector<Type<3, float>>& geometries);                                                                \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, double>::subtract_volume(                                  \
        const std::vector<Type<2, double>>& geometries);                                                               \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, double>::subtract_volume(                                  \
        const std::vector<Type<3, double>>& geometries);                                                               \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, float>::subtract_volume(                                   \
        const std::vector<sdf::TransformedGeometry<Type, 2, float>>& geometries);                                      \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, float>::subtract_volume(                                   \
        const std::vector<sdf::TransformedGeometry<Type, 3, float>>& geometries);                                      \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, double>::subtract_volume(                                  \
        const std::vector<sdf::TransformedGeometry<Type, 2, double>>& geometries);                                     \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, double>::subtract_volume(                                  \
        const std::vector<sdf::TransformedGeometry<Type, 3, double>>& geometries);

namespace ltb {
namespace dvh {

template <int L, typename T>
template <typename Geometry, typename>
void DistanceVolumeHierarchyGpu<L, T>::add_volume(std::vector<Geometry> const& cpu_geometries) {
    //    thrust::device_vector<Geometry> gpu_geometries(cpu_geometries.begin(), cpu_geometries.end());

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

template <int L, typename T>
template <typename Geometry, typename>
void DistanceVolumeHierarchyGpu<L, T>::subtract_volume(std::vector<Geometry> const& cpu_geometries) {
    //    thrust::device_vector<Geometry> gpu_geometries(cpu_geometries.begin(), cpu_geometries.end());

    if (cpu_geometries.empty()) {
        return;
    }

    enum State : unsigned {
        DoesNotMatter    = 0u,
        PreviouslyInside = 1u,
    };

    CellSet        children_to_remove;
    CellSet        to_remove;
    CellMap<State> to_visit;
    CellMap<State> cells;

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
            for (const auto root_cell : root_cells) {
                cells.emplace(root_cell, State::DoesNotMatter);
            }
        }

        auto level_resolution = resolution(level);
        auto half_resolution  = level_resolution * T(0.5);
        auto cell_corner_dist = glm::length(glm::vec<L, T>(half_resolution));

        for (const auto& cell_and_state : cells) {
            const auto& cell  = cell_and_state.first;
            const auto& state = cell_and_state.second;

            auto const p = dvh::cell_center(cell, level_resolution);

            auto min_dist = std::numeric_limits<T>::infinity();

            for (auto const& cpu_geometry : cpu_geometries) {
                min_dist = std::min(min_dist, cpu_geometry.distance_from(p));
            }

            if (min_dist < -cell_corner_dist) {
                distance_field.erase(cell);

                auto children = children_cells(cell);
                children_to_remove.insert(std::make_move_iterator(children.begin()),
                                          std::make_move_iterator(children.end()));
            } else if (min_dist < cell_corner_dist) {
                State children_state = state;
                auto  iter           = distance_field.find(cell);

                if (iter != distance_field.end() && iter->second[L] < 0.f) {
                    children_state = State::PreviouslyInside;
                }

                if (iter != distance_field.end()) {
                    distance_field[cell] = glm::vec<L + 1, T>(DistanceVolumeHierarchyGpu<L, T>::not_fully_inside);
                }

                auto children = children_cells(cell);
                for (const auto& child_cell : children) {
                    to_visit[child_cell] = children_state;
                }
            } else {
                auto previous = distance_field.find(cell);

                if (previous == distance_field.end() || previous->second[L] < -min_dist) {
                    if (state == State::PreviouslyInside) {
                        distance_field[cell] = glm::vec<L + 1, T>(p, -min_dist);
                    }
                }
            }
        }
        cells.clear();
    }

#if 0
    /*
     * Clean-up
     */
    CellMap<int> total_counts;
    CellMap<int> num_outside;

    for (int level = lowest_level_; level < cpu_roots_.begin()->first; ++level) {

        auto& distance_field = cpu_levels_[level];

        for (auto iter = distance_field.begin(); iter != distance_field.end();) {
            const auto& cell = iter->first;

            if (auto counts_iter = total_counts.find(cell); counts_iter != total_counts.end()) {
                if (auto outside_iter = num_outside.find(cell);
                    outside_iter != num_outside.end() && outside_iter->second == counts_iter->second) {
                    iter = distance_field.erase(iter);
                    continue;
                }
            }
            ++iter;
        }
        total_counts.clear();
        num_outside.clear();

        auto level_resolution = resolution(level);
        auto half_resolution  = level_resolution * T(0.5);
        auto cell_corner_dist = glm::length(glm::vec<L, T>(half_resolution));

        for (const auto& [cell, vector_and_distance] : distance_field) {
            auto parent = parent_cell(cell);
            total_counts[parent]++;

            if (std::isinf(vector_and_distance[L])) {
                num_outside[parent]++;
            }
        }

        // TODO: Delete the actual child nodes too
    }
#endif
}

} // namespace dvh
} // namespace ltb
