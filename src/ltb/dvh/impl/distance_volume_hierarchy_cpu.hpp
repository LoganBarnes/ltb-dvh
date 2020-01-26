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
#include "ltb/util/container_utils.hpp"

// external
#include <glm/gtx/hash.hpp>

// standard
#include <map>
#include <unordered_set>

namespace ltb::dvh {

template <int L, typename T>
class DistanceVolumeHierarchyCpu {
public:
    using SparseVolumeMap = std::unordered_map<glm::vec<L, int>, glm::vec<L + 1, T>>;
    using CellSet         = std::unordered_set<glm::vec<L, int>>;
    template <typename V>
    using LevelMap = std::map<int, V, std::greater<int>>;

    explicit DistanceVolumeHierarchyCpu(T base_resolution, int max_level = std::numeric_limits<int>::max());

    void clear();

    /*
     * All volumes added at the same time will be grouped together under the same root
     */
    void add_volume(sdf::Geometry<L, T> const* geometry);
    void add_volume(sdf::Geometry<L, T> const* start, sdf::Geometry<L, T> const* end);

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

template <int L, typename T = float>
using DistanceVolumeHierarchy = DistanceVolumeHierarchyCpu<L, T>;

} // namespace ltb::dvh
