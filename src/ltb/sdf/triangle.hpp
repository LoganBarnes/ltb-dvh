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

template <typename T = float>
struct Triangle {
    glm::vec<3, T> p0;
    glm::vec<3, T> p1;
    glm::vec<3, T> p2;

    LTB_CUDA_FUNC Triangle() = default;
    LTB_CUDA_FUNC Triangle(glm::vec<3, T> point0, glm::vec<3, T> point1, glm::vec<3, T> point2)
        : p0(point0), p1(point1), p2(point2) {}

    LTB_CUDA_FUNC auto vector_from(glm::vec<3, T> const& point) const -> glm::vec<3, T>;
    LTB_CUDA_FUNC auto distance_from(glm::vec<3, T> const& point) const -> T;
    LTB_CUDA_FUNC auto bounding_box() const -> AABB<3, T>;
};

template <typename T = float>
LTB_CUDA_FUNC auto make_triangle(glm::vec<3, T> p0, glm::vec<3, T> p1, glm::vec<3, T> p2) -> Triangle<T> {
    return {p0, p1, p2};
}

template <typename T>
LTB_CUDA_FUNC auto Triangle<T>::vector_from(glm::vec<3, T> const& point) const -> glm::vec<3, T> {
    return point;
}

template <typename T>
LTB_CUDA_FUNC auto Triangle<T>::distance_from(glm::vec<3, T> const& point) const -> T {
    auto dot2 = [](glm::vec<3, T> const& v) { return glm::dot(v, v); };

    auto const& p = point;
    auto const& a = p0;
    auto const& b = p1;
    auto const& c = p2;

    auto ba  = b - a;
    auto pa  = p - a;
    auto cb  = c - b;
    auto pb  = p - b;
    auto ac  = a - c;
    auto pc  = p - c;
    auto nor = glm::cross(ba, ac);

    return glm::sqrt((glm::sign(glm::dot(glm::cross(ba, nor), pa)) + glm::sign(glm::dot(glm::cross(cb, nor), pb))
                          + glm::sign(glm::dot(glm::cross(ac, nor), pc))
                      < T(2))
                         ? glm::min(glm::min(dot2(ba * glm::clamp(glm::dot(ba, pa) / dot2(ba), T(0), T(1)) - pa),
                                             dot2(cb * glm::clamp(glm::dot(cb, pb) / dot2(cb), T(0), T(1)) - pb)),
                                    dot2(ac * glm::clamp(glm::dot(ac, pc) / dot2(ac), T(0), T(1)) - pc))
                         : glm::dot(nor, pa) * glm::dot(nor, pa) / dot2(nor));
}

template <typename T>
LTB_CUDA_FUNC auto Triangle<T>::bounding_box() const -> AABB<3, T> {
    return {glm::min(p0, glm::min(p1, p2)), glm::max(p0, glm::max(p1, p2))};
}

} // namespace sdf
} // namespace ltb
