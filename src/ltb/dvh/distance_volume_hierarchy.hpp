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

// external
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

// standard
#include <unordered_map>

namespace ltb::dvh {
namespace detail {

template <typename T, typename IVec, typename Vec>
class DistanceVolumeHierarchy {
public:
    DistanceVolumeHierarchy() = default;

    void add_node(IVec index);

    auto nodes() const -> std::unordered_map<IVec, Vec> const& { return sparse_distances_; }

private:
    std::unordered_map<IVec, Vec> sparse_distances_;
};

template <typename T, typename IVec, typename Vec>
void DistanceVolumeHierarchy<T, IVec, Vec>::add_node(IVec index) {
    sparse_distances_.emplace(index, Vec(std::numeric_limits<T>::infinity()));
}

} // namespace detail

template <std::size_t N = 3, typename T = float, typename = std::enable_if_t<std::is_floating_point_v<T>>>
class DistanceVolumeHierarchy;

template <typename T>
class DistanceVolumeHierarchy<2, T> : public detail::DistanceVolumeHierarchy<T, glm::ivec2, glm::tvec3<T>> {
    using detail::DistanceVolumeHierarchy<T, glm::ivec2, glm::tvec3<T>>::DistanceVolumeHierarchy;
};

template <typename T>
class DistanceVolumeHierarchy<3, T> : public detail::DistanceVolumeHierarchy<T, glm::ivec3, glm::tvec4<T>> {
    using detail::DistanceVolumeHierarchy<T, glm::ivec3, glm::tvec4<T>>::DistanceVolumeHierarchy;
};

} // namespace ltb::dvh