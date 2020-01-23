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
#include <ltb/sdf/sdf.hpp>

// external
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/string_cast.hpp>

// standard
#include <iostream>
#include <map>
#include <unordered_map>

namespace ltb::dvh {

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

template <int L, typename T = float>
class DistanceVolumeHierarchy {
public:
    using SparseDvhLevel = std::unordered_map<glm::vec<L, int>, glm::vec<L + 1, T>>;

    explicit DistanceVolumeHierarchy(T base_resolution, int max_level = std::numeric_limits<int>::max());

    void clear() {
        levels_ = {{1, SparseDvhLevel{}}}; // The empty base level
    }

    template <typename Geom>
    void add_volumes(std::vector<Geom> const& geometries);

    auto levels() const { return levels_; }
    auto lowest_level() const { return levels_.begin()->first; }

    auto base_resolution() const { return base_resolution_; }

    auto level_resolution(int level_index) const {
        if (level_index == 0) {
            throw std::invalid_argument("level_index cannot be zero");
        }
        return base_resolution_ * (level_index > 0 ? T(level_index) : T(1) / level_index);
    }

private:
    T   base_resolution_;
    int max_level_;

    std::map<int, SparseDvhLevel> levels_;
};

template <int L, typename T>
DistanceVolumeHierarchy<L, T>::DistanceVolumeHierarchy(T base_resolution, int max_level)
    : base_resolution_(base_resolution), max_level_(max_level) {
    clear();
}

template <int L, typename T>
template <typename Geom>
void DistanceVolumeHierarchy<L, T>::add_volumes(std::vector<Geom> const& geometries) {
    for (auto const& geometry : geometries) {
        auto aabb = sdf::bounding_box(geometry);

        auto min_cell = glm::vec<L, int>(glm::floor(aabb.min_point / base_resolution_));
        auto max_cell = glm::vec<L, int>(glm::ceil(aabb.max_point / base_resolution_));

        auto  level_index     = 1;
        auto& level           = levels_.at(level_index);
        auto  resolution      = level_resolution(level_index);
        auto  half_resolution = resolution * level_index * T(0.5);

        iterate(min_cell, max_cell, [&geometry, &level, &resolution, &half_resolution](auto const& index) {
            auto const p = glm::vec<L, T>(index) * resolution;

            auto dist = sdf::distance_to_geometry(p, geometry);

            if (dist <= glm::length(glm::vec<L, T>(half_resolution))) {
                level.insert_or_assign(index, glm::vec<L + 1, T>(p, dist));
            }
        });
    }
}

} // namespace ltb::dvh