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

namespace ltb::dvh {

template <int L, typename T>
DistanceVolumeHierarchy<L, T>::DistanceVolumeHierarchy(T base_resolution, int max_level)
    : base_resolution_(base_resolution), max_level_(max_level) {
    clear();
}

template <int L, typename T>
auto DistanceVolumeHierarchy<L, T>::levels() const -> std::map<int, SparseDvhLevel> const& {
    return levels_;
}

template <int L, typename T>
auto DistanceVolumeHierarchy<L, T>::lowest_level() const -> int {
    return levels_.begin()->first;
}

template <int L, typename T>
auto DistanceVolumeHierarchy<L, T>::base_resolution() const -> T {
    return base_resolution_;
}

template <int L, typename T>
auto DistanceVolumeHierarchy<L, T>::level_resolution(int level_index) const -> T {
    if (level_index == 0) {
        throw std::invalid_argument("level_index cannot be zero");
    }
    return base_resolution_ * (level_index > 0 ? T(level_index) : T(1) / level_index);
}

} // namespace ltb::dvh

// test compilation
template class ltb::dvh::DistanceVolumeHierarchy<2, float>;
template class ltb::dvh::DistanceVolumeHierarchy<3, float>;
template class ltb::dvh::DistanceVolumeHierarchy<2, double>;
template class ltb::dvh::DistanceVolumeHierarchy<3, double>;
