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

template <int L, typename T = float>
struct OrientedTriangle : public Geometry<L, T> {
    glm::vec<L, T> p0;
    glm::vec<L, T> p1;
    glm::vec<L, T> p2;

    explicit OrientedTriangle(glm::vec<L, T> point0, glm::vec<L, T> point1, glm::vec<L, T> point2)
        : p0(point0), p1(point1), p2(point2) {}
    ~OrientedTriangle() override = default;

    LTB_CUDA_FUNC auto vector_from(glm::vec<L, T> const& point) const -> glm::vec<L, T> override;
    LTB_CUDA_FUNC auto distance_from(glm::vec<L, T> const& point) const -> T override;
    LTB_CUDA_FUNC auto bounding_box() const -> AABB<L, T> override;
};

template <int L, typename T = float>
auto make_oriented_triangle(glm::vec<L, T> start, glm::vec<L, T> end) -> OrientedTriangle<L, T> {
    return OrientedTriangle<L, T>{start, end};
}

template <int L, typename T>
LTB_CUDA_FUNC auto OrientedTriangle<L, T>::vector_from(glm::vec<L, T> const& point) const -> glm::vec<L, T> {
    return make_triangle(start, end).vector_from(point);
}

template <int L, typename T>
LTB_CUDA_FUNC auto OrientedTriangle<L, T>::distance_from(glm::vec<L, T> const& point) const -> T {
    return std::runtime_error("Unimplemented");
}

template <int L, typename T>
LTB_CUDA_FUNC auto OrientedTriangle<L, T>::bounding_box() const -> AABB<L, T> {
    return make_triangle(start, end).bounding_box();
}

} // namespace sdf
} // namespace ltb
