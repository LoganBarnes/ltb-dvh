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

namespace ltb {
namespace sdf {

template <typename T = float>
struct OrientedLine : public Geometry<2, T> {
    glm::vec<2, T> start = {};
    glm::vec<2, T> end   = {};

    LTB_CUDA_FUNC OrientedLine() = default;
    LTB_CUDA_FUNC OrientedLine(glm::vec<2, T> from, glm::vec<2, T> to) : start(from), end(to) {}

    LTB_CUDA_FUNC ~OrientedLine() = default;

    LTB_CUDA_FUNC auto vector_from(glm::vec<2, T> const& point) const -> glm::vec<2, T>;
    LTB_CUDA_FUNC auto distance_from(glm::vec<2, T> const& point) const -> T;
    LTB_CUDA_FUNC auto bounding_box() const -> AABB<2, T>;
};

template <typename T = float>
LTB_CUDA_FUNC auto make_oriented_line(glm::vec<2, T> start, glm::vec<2, T> end) -> OrientedLine<T> {
    return OrientedLine<T>{start, end};
}

template <typename T>
LTB_CUDA_FUNC auto OrientedLine<T>::vector_from(glm::vec<2, T> const& point) const -> glm::vec<2, T> {
    return make_line(start, end).vector_from(point);
}

template <typename T>
LTB_CUDA_FUNC auto OrientedLine<T>::distance_from(glm::vec<2, T> const& point) const -> T {
    T    negative_if_inside = glm::sign(glm::cross(point - start, end - start));
    auto abs_distance       = make_line(start, end).distance_from(point);
    return abs_distance * negative_if_inside + abs_distance * (T(1) - std::abs(negative_if_inside));
}

template <typename T>
LTB_CUDA_FUNC auto OrientedLine<T>::bounding_box() const -> AABB<2, T> {
    return make_line(start, end).bounding_box();
}

} // namespace sdf
} // namespace ltb
