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
#include "line.hpp"
#include "offset.hpp"

namespace ltb {
namespace sdf {

template <int L, typename T = float>
struct OffsetLine : public Geometry<L, T> {
    glm::vec<L, T> start           = {};
    glm::vec<L, T> end             = {};
    T              offset_distance = 0.f;

    LTB_CUDA_FUNC OffsetLine() = default;
    LTB_CUDA_FUNC OffsetLine(glm::vec<L, T> from, glm::vec<L, T> to, T offset)
        : start(from), end(to), offset_distance(offset) {}

    LTB_CUDA_FUNC ~OffsetLine() = default;

    LTB_CUDA_FUNC auto vector_from(glm::vec<L, T> const& point) const -> glm::vec<L, T> override;
    LTB_CUDA_FUNC auto distance_from(glm::vec<L, T> const& point) const -> T override;
    LTB_CUDA_FUNC auto bounding_box() const -> AABB<L, T> override;
};

template <int L, typename T = float>
LTB_CUDA_FUNC auto make_offset_line(glm::vec<L, T> start, glm::vec<L, T> end, T offset) -> OffsetLine<L, T> {
    return OffsetLine<L, T>{start, end, offset};
}

template <int L, typename T>
LTB_CUDA_FUNC auto OffsetLine<L, T>::vector_from(glm::vec<L, T> const& point) const -> glm::vec<L, T> {
    auto vec  = make_line(start, end).vector_from(point);
    auto dist = offset(glm::length(vec), offset_distance);
    return point + glm::normalize(vec) * dist;
}

template <int L, typename T>
LTB_CUDA_FUNC auto OffsetLine<L, T>::distance_from(glm::vec<L, T> const& point) const -> T {
    return offset(make_line(start, end).distance_from(point), offset_distance);
}

template <int L, typename T>
LTB_CUDA_FUNC auto OffsetLine<L, T>::bounding_box() const -> AABB<L, T> {
    auto aabb = make_line(start, end).bounding_box();
    aabb.min_point -= offset_distance;
    aabb.max_point += offset_distance;
    return aabb;
}

} // namespace sdf
} // namespace ltb
