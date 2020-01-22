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

// external
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/exterior_product.hpp>

// standard
#include <type_traits>

namespace ltb::sdf {

template <int L, typename T = float>
struct Line {
    glm::vec<L, T> start;
    glm::vec<L, T> end;
};

template <int L, typename T = float>
auto make_line(glm::vec<L, T> start, glm::vec<L, T> end) -> Line<L, T> {
    return {start, end};
}

template <int L, typename T = float, typename = std::enable_if_t<std::is_floating_point_v<T>>>
auto vector_to_geometry(glm::vec<L, T> const& point, Line<L, T> const& line) -> glm::vec<L, T> {
    auto start_to_point = point - line.start;
    auto start_to_end   = line.end - line.start;

    auto t_along_infinite_line = glm::dot(start_to_point, start_to_end) / glm::dot(start_to_end, start_to_end);
    auto t_dist_along_segment  = glm::clamp(t_along_infinite_line, T(0), T(1));

    return start_to_end * t_dist_along_segment - start_to_point;
}

template <int L, typename T = float, typename = std::enable_if_t<std::is_floating_point_v<T>>>
auto distance_to_geometry(glm::vec<L, T> const& point, Line<L, T> const& line) -> T {
    return glm::length(vector_to_geometry(point, line));
}

template <typename T = float, typename = std::enable_if_t<std::is_floating_point_v<T>>>
auto distance_to_oriented_geometry(glm::vec<2, T> const& point, Line<2, T> const& line) -> T {
    T negative_if_inside = glm::sign(glm::cross(point - line.start, line.end - line.start));
    return distance_to_geometry(point, line) * negative_if_inside;
}

} // namespace ltb::sdf
