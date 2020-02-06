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
#include <glm/gtx/exterior_product.hpp>

namespace ltb {
namespace sdf {

template <int L, typename T = float>
struct Line : public Geometry<L, T> {
    glm::vec<L, T> start = {};
    glm::vec<L, T> end   = {};

    LTB_CUDA_FUNC Line() = default;
    LTB_CUDA_FUNC Line(glm::vec<L, T> from, glm::vec<L, T> to) : start(from), end(to) {}

    LTB_CUDA_FUNC ~Line();
    A_FUNC auto        vector_from(glm::vec<L, T> const& point) const -> glm::vec<L, T> override;
    LTB_CUDA_FUNC auto distance_from(glm::vec<L, T> const& point) const -> T override;
    LTB_CUDA_FUNC auto bounding_box() const -> AABB<L, T> override;
};

template <int L, typename T = float>
LTB_CUDA_FUNC auto make_line(glm::vec<L, T> start, glm::vec<L, T> end) -> Line<L, T> {
    return Line<L, T>{start, end};
}

template <int L, typename T>
LTB_CUDA_FUNC auto Line<L, T>::vector_from(glm::vec<L, T> const& point) const -> glm::vec<L, T> {
    auto start_to_point = point - start;
    auto start_to_end   = end - start;

    auto t_along_infinite_line = glm::dot(start_to_point, start_to_end) / glm::dot(start_to_end, start_to_end);
    auto t_dist_along_segment  = glm::clamp(t_along_infinite_line, T(0), T(1));

    return glm::mix(start, end, t_dist_along_segment) - point;
}

template <int L, typename T>
LTB_CUDA_FUNC auto Line<L, T>::distance_from(glm::vec<L, T> const& point) const -> T {
    return glm::length(vector_from(point));
}

template <int L, typename T>
LTB_CUDA_FUNC auto Line<L, T>::bounding_box() const -> AABB<L, T> {
    return {glm::min(start, end), glm::max(start, end)};
}

} // namespace sdf
} // namespace ltb
