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
#include <glm/gtx/hash.hpp>

// standard
#include <algorithm>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ltb::dvh {

template <int L, typename T>
class DistanceVolumeHierarchyCpu {
public:
    using Cell    = glm::vec<L, int>;
    using CellSet = std::unordered_set<Cell>;
    template <typename V>
    using CellMap         = std::unordered_map<Cell, V>;
    using SparseVolumeMap = std::unordered_map<Cell, glm::vec<L + 1, T>>;
    template <typename V>
    using LevelMap = std::map<int, V, std::greater<int>>;

    explicit DistanceVolumeHierarchyCpu(T base_resolution, int max_level = std::numeric_limits<int>::max());

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

    LevelMap<SparseVolumeMap> levels_;
    LevelMap<CellSet>         roots_;

    auto actually_add_volume(std::vector<sdf::Geometry<L, T> const*> const& geometries) -> void;
    auto add_roots_for_bounds(sdf::AABB<L, T> const& aabb) -> void;
    auto actually_subtract_volumes(std::vector<sdf::Geometry<L, T> const*> const& geometries) -> void;
};

template <int L, typename T>
template <typename Geometry, typename>
void DistanceVolumeHierarchyCpu<L, T>::add_volume(std::vector<Geometry> const& geometries) {
    std::vector<sdf::Geometry<L, T> const*> geometry_pointers(geometries.size(), nullptr);

    std::transform(geometries.begin(), geometries.end(), geometry_pointers.begin(), [](const auto& geometry) {
        return &geometry;
    });

    actually_add_volume(geometry_pointers);
}

template <int L, typename T>
template <typename Geometry, typename>
void DistanceVolumeHierarchyCpu<L, T>::subtract_volume(std::vector<Geometry> const& geometries) {
    std::vector<sdf::Geometry<L, T> const*> geometry_pointers(geometries.size(), nullptr);

    std::transform(geometries.begin(), geometries.end(), geometry_pointers.begin(), [](const auto& geometry) {
        return &geometry;
    });

    actually_subtract_volumes(geometry_pointers);
}

template <int L, typename T = float>
using DistanceVolumeHierarchy = DistanceVolumeHierarchyCpu<L, T>;

} // namespace ltb::dvh