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
#include "ltb/sdf/geometry.hpp"

// external
#include <cuda_runtime.h>
#include <glm/gtx/hash.hpp>
#include <thrust/device_vector.h>

// standard
#include <algorithm>
#include <iterator>
#include <map>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ltb {
namespace dvh {

template <int L, typename T>
class DistanceVolumeHierarchyGpu {
public:
    using Cell    = glm::vec<L, int>;
    using CellSet = std::unordered_set<Cell>;
    template <typename V>
    using CellMap         = std::unordered_map<Cell, V>;
    using SparseVolumeMap = std::unordered_map<Cell, glm::vec<L + 1, T>>;
    template <typename V>
    using LevelMap = std::map<int, V, std::greater<int>>;

    explicit DistanceVolumeHierarchyGpu(T base_resolution, int max_level = std::numeric_limits<int>::max());

    void clear();

    /**
     * @brief All volumes added at the same time will be grouped together under the same root
     * @tparam Geometry - Must be derived from sdf::Geometry<L, T>.
     * @param geometries - the list of geometries to add.
     */
    template <typename Geom,
              typename = typename std::enable_if<std::is_base_of<sdf::Geometry<L, T>, Geom>::value>::type>
    void add_volume(std::vector<Geom> const& geometries);

    template <typename Geom,
              typename = typename std::enable_if<std::is_base_of<sdf::Geometry<L, T>, Geom>::value>::type>
    void subtract_volume(std::vector<Geom> const& geometries);

    auto levels() const -> LevelMap<SparseVolumeMap> const&;

    auto base_resolution() const -> T;

    auto resolution(int level_index) const -> T;

    constexpr static int base_level       = 0;
    constexpr static T   not_fully_inside = std::numeric_limits<T>::infinity();

private:
    T   base_resolution_;
    int max_level_;
    int lowest_level_ = 0;

    LevelMap<SparseVolumeMap> cpu_levels_;
    LevelMap<CellSet>         cpu_roots_;

    LevelMap<SparseVolumeMap> gpu_cells_;
    LevelMap<SparseVolumeMap> gpu_values_;

    auto actually_add_volume(sdf::Geometry<L, T> const* cpu_geometries,
                             sdf::Geometry<L, T> const* gpu_geometries,
                             std::size_t                num_geometries) -> void;

    auto add_roots_for_bounds(sdf::AABB<L, T> const& aabb) -> void;
    auto actually_subtract_volumes(sdf::Geometry<L, T> const* cpu_geometries,
                                   sdf::Geometry<L, T> const* gpu_geometries,
                                   std::size_t                num_geometries) -> void;
};

template <int L, typename T = float>
using DistanceVolumeHierarchy = DistanceVolumeHierarchyGpu<L, T>;

} // namespace dvh
} // namespace ltb
