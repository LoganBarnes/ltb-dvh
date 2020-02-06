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
#include "geometry.hpp"

// external
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

// standard
#include <stdexcept>

namespace ltb {
namespace sdf {

template <int L, typename T = float>
struct Triangle : public Geometry<L, T> {
    glm::vec<L, T> p0;
    glm::vec<L, T> p1;
    glm::vec<L, T> p2;

    LTB_CUDA_FUNC Triangle() = default;
    LTB_CUDA_FUNC Triangle(glm::vec<L, T> point0, glm::vec<L, T> point1, glm::vec<L, T> point2)
        : p0(point0), p1(point1), p2(point2) {}

    LTB_CUDA_FUNC ~Triangle() = default;

    LTB_CUDA_FUNC auto vector_from(glm::vec<L, T> const& point) const -> glm::vec<L, T> override;
    LTB_CUDA_FUNC auto distance_from(glm::vec<L, T> const& point) const -> T override;
    LTB_CUDA_FUNC auto bounding_box() const -> AABB<L, T> override;
};

template <int L, typename T = float>
LTB_CUDA_FUNC auto make_triangle(glm::vec<L, T> p0, glm::vec<L, T> p1, glm::vec<L, T> p2) -> Triangle<L, T> {
    return Triangle<L, T>{p0, p1, p2};
}

template <int L, typename T>
LTB_CUDA_FUNC auto Triangle<L, T>::vector_from(glm::vec<L, T> const& point) const -> glm::vec<L, T> {
    return point;
}

template <int L, typename T>
LTB_CUDA_FUNC auto Triangle<L, T>::distance_from(glm::vec<L, T> const& point) const -> T {
    return glm::length(vector_from(point));
}

template <int L, typename T>
LTB_CUDA_FUNC auto Triangle<L, T>::bounding_box() const -> AABB<L, T> {
    return {glm::min(p0, glm::min(p1, p2)), glm::max(p0, glm::max(p1, p2))};
}

} // namespace sdf
} // namespace ltb
