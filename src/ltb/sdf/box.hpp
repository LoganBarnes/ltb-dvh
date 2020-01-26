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
#include <glm/gtx/component_wise.hpp>

namespace ltb {
namespace sdf {

template <int L, typename T = float>
struct Box : public Geometry<L, T> {
    glm::vec<L, T> half_dimensions;

    explicit Box(glm::vec<L, T> dimensions) : half_dimensions(dimensions * T(0.5)) {}
    ~Box() override = default;

    LTB_CUDA_FUNC auto vector_from(glm::vec<L, T> const& point) const -> glm::vec<L, T> override;
    LTB_CUDA_FUNC auto distance_from(glm::vec<L, T> const& point) const -> T override;
    LTB_CUDA_FUNC auto bounding_box() const -> AABB<L, T> override;
};

template <int L, typename T = float>
auto make_box(glm::vec<L, T> dimensions) -> Box<L, T> {
    return Box<L, T>{dimensions};
}

template <int L, typename T>
LTB_CUDA_FUNC auto Box<L, T>::vector_from(glm::vec<L, T> const& point) const -> glm::vec<L, T> {
    auto positive_point      = glm::abs(point);
    auto positive_box_corner = half_dimensions;

    auto corner_to_point = positive_point - positive_box_corner;

    auto const        min    = glm::min(glm::compMax(corner_to_point), T(0));
    glm::vec<L, bool> is_min = glm::equal(corner_to_point, glm::vec<L, T>(min));

    auto box_to_outer_point = glm::max(corner_to_point, T(0));
    auto box_to_inner_point = corner_to_point * glm::vec<L, T>(is_min);

    auto point_to_box = -(box_to_outer_point + box_to_inner_point);

    return point_to_box * glm::sign(point);
}

template <int L, typename T>
LTB_CUDA_FUNC auto Box<L, T>::distance_from(glm::vec<L, T> const& point) const -> T {
    auto positive_point      = glm::abs(point);
    auto positive_box_corner = half_dimensions;

    auto corner_to_point = positive_point - positive_box_corner;

    auto dist_to_outer_point = glm::length(glm::max(corner_to_point, T(0)));
    auto dist_to_inner_point = glm::min(glm::compMax(corner_to_point), T(0));

    return dist_to_outer_point + dist_to_inner_point;
}

template <int L, typename T>
LTB_CUDA_FUNC auto Box<L, T>::bounding_box() const -> AABB<L, T> {
    return {-half_dimensions, half_dimensions};
}

} // namespace sdf
} // namespace ltb
