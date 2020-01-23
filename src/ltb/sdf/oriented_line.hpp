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

namespace ltb::sdf {

template <typename T = float>
struct OrientedLine {
    glm::vec<2, T> start;
    glm::vec<2, T> end;
};

template <typename T = float>
auto make_oriented_line(glm::vec<2, T> start, glm::vec<2, T> end) -> OrientedLine<T> {
    return {start, end};
}

template <typename T = float>
auto vector_to_geometry(glm::vec<2, T> const& point, OrientedLine<T> const& line) -> glm::vec<2, T> {
    return vector_to_geometry(point, make_line(line.start, line.end));
}

template <typename T = float>
auto distance_to_geometry(glm::vec<2, T> const& point, OrientedLine<T> const& line) -> T {
    T negative_if_inside = glm::sign(glm::cross(point - line.start, line.end - line.start));
    return distance_to_geometry(point, make_line(line.start, line.end)) * negative_if_inside;
}

template <typename T = float>
auto bounding_box(OrientedLine<T> const& line) -> AABB<2, T> {
    return bounding_box(make_line(line.start, line.end));
}

} // namespace ltb::sdf
