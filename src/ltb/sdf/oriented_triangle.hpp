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
#include "triangle.hpp"

namespace ltb {
namespace sdf {

template <typename T = float>
struct OrientedTriangle {
    glm::vec<3, T> p0;
    glm::vec<3, T> p1;
    glm::vec<3, T> p2;

    LTB_CUDA_FUNC OrientedTriangle() = default;
    LTB_CUDA_FUNC OrientedTriangle(glm::vec<3, T> point0, glm::vec<3, T> point1, glm::vec<3, T> point2)
        : p0(point0), p1(point1), p2(point2) {}

    LTB_CUDA_FUNC auto vector_from(glm::vec<3, T> const& point) const -> glm::vec<3, T>;
    LTB_CUDA_FUNC auto distance_from(glm::vec<3, T> const& point) const -> T;
    LTB_CUDA_FUNC auto bounding_box() const -> AABB<3, T>;
};

template <typename T = float>
LTB_CUDA_FUNC auto make_oriented_triangle(glm::vec<3, T> start, glm::vec<3, T> end) -> OrientedTriangle<T> {
    return OrientedTriangle<T>{start, end};
}

template <typename T>
LTB_CUDA_FUNC auto OrientedTriangle<T>::vector_from(glm::vec<3, T> const& point) const -> glm::vec<3, T> {
    return make_triangle(p0, p1, p2).vector_from(point);
}

template <typename T>
LTB_CUDA_FUNC auto OrientedTriangle<T>::distance_from(glm::vec<3, T> const& point) const -> T {
    return make_triangle(p0, p1, p2).distance_from(point);
}

template <typename T>
LTB_CUDA_FUNC auto OrientedTriangle<T>::bounding_box() const -> AABB<3, T> {
    return make_triangle(p0, p1, p2).bounding_box();
}

} // namespace sdf
} // namespace ltb
