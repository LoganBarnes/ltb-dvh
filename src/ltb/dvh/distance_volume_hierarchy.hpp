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

// standard
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

template <int N, typename T = float>
class DistanceVolumeHierarchy {
public:
    DistanceVolumeHierarchy(glm::vec<N, int> field_min_index, glm::vec<N, int> field_max_index);

    template <typename Geom>
    void add_volumes(std::vector<Geom> const& geometries);

    auto min_index() const -> glm::vec<N, int> const& { return min_index_; };
    auto max_index() const -> glm::vec<N, int> const& { return max_index_; };
    auto sparse_distance_field() const -> std::unordered_map<glm::vec<N, int>, glm::vec<N + 1, T>> const& {
        return sparse_distance_field_;
    }

private:
    glm::vec<N, int>                                         min_index_;
    glm::vec<N, int>                                         max_index_;
    std::unordered_map<glm::vec<N, int>, glm::vec<N + 1, T>> sparse_distance_field_;
};

template <int N, typename T>
DistanceVolumeHierarchy<N, T>::DistanceVolumeHierarchy(glm::vec<N, int> field_min_index,
                                                       glm::vec<N, int> field_max_index)
    : min_index_(field_min_index), max_index_(field_max_index) {

    sparse_distance_field_.emplace(glm::vec<N, int>(-1), glm::vec<N + 1, T>(std::numeric_limits<T>::infinity()));
    sparse_distance_field_.emplace(glm::vec<N, int>(0), glm::vec<N + 1, T>(0));
}

template <int N, typename T>
template <typename Geom>
void DistanceVolumeHierarchy<N, T>::add_volumes(std::vector<Geom> const& geometries) {
    iterate(min_index_, max_index_, [this, &geometries](glm::vec<N, int> const& index) {
        auto const p = glm::vec<N, T>(index) + glm::vec<N, T>(0.5);

        for (auto const& geometry : geometries) {
            auto dist = sdf::distance_to_geometry(p, geometry);

            if (dist <= glm::length(glm::vec<N, T>(0.5))) {
                sparse_distance_field_.insert_or_assign(index, glm::vec<N + 1, T>(p, dist));
            }
        }
    });
}

} // namespace ltb::dvh