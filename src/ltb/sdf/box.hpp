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
#include "aabb.hpp"

// external
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/component_wise.hpp>

namespace ltb::sdf {

template <int L, typename T = float>
struct Box {
    glm::vec<L, T> dimensions;
};

template <int L, typename T = float>
auto make_box(glm::vec<L, T> dimensions) -> Box<L, T> {
    return {dimensions};
}

template <int L, typename T = float>
auto vector_to_geometry(glm::vec<L, T> const& point, Box<L, T> const& box) -> glm::vec<L, T> {
    auto positive_point      = glm::abs(point);
    auto positive_box_corner = box.dimensions * T(0.5);

    auto corner_to_point = positive_point - positive_box_corner;

    auto const        min    = glm::min(glm::compMax(corner_to_point), T(0));
    glm::vec<L, bool> is_min = glm::equal(corner_to_point, glm::vec<L, T>(min));

    auto box_to_outer_point = glm::max(corner_to_point, T(0));
    auto box_to_inner_point = corner_to_point * glm::vec<L, T>(is_min);

    auto point_to_box = -(box_to_outer_point + box_to_inner_point);

    return point_to_box * glm::sign(point);
}

template <int L, typename T = float>
auto distance_to_geometry(glm::vec<L, T> const& point, Box<L, T> const& box) -> T {
    auto positive_point      = glm::abs(point);
    auto positive_box_corner = box.dimensions * T(0.5);

    auto corner_to_point = positive_point - positive_box_corner;

    auto dist_to_outer_point = glm::length(glm::max(corner_to_point, T(0)));
    auto dist_to_inner_point = glm::min(glm::compMax(corner_to_point), T(0));

    return dist_to_outer_point + dist_to_inner_point;
}

template <int L, typename T = float>
auto bounding_box(Box<L, T> const& box) -> AABB<L, T> {
    auto half_dimensions = box.dimensions * T(0.5);
    return {-half_dimensions, half_dimensions};
}

} // namespace ltb::sdf
