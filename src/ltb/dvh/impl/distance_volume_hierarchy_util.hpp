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
#include "ltb/sdf/sdf.hpp"
#include "ltb/util/comparison_utils.hpp"

// external
#include <glm/geometric.hpp>

// standard
#include <vector>

namespace ltb::dvh {

template <int L>
auto parent_cell(glm::vec<L, int> const& cell) -> glm::vec<L, int> {
    return (cell + glm::min(glm::sign(cell), 0)) / 2; // TODO test this
}

template <int L>
auto children_cells(glm::vec<L, int> const& cell) -> std::vector<glm::vec<L, int>>;

template <int L, typename T>
auto cell_center(glm::vec<L, int> const& cell, const T& resolution) -> glm::vec<L, T> {
    return (glm::vec<L, T>(cell) + glm::vec<L, T>(0.5)) * resolution;
}

template <int L, typename T>
auto get_cell(glm::vec<L, T> const& world_point, const T& resolution) -> glm::vec<L, int> {
    return glm::vec<L, int>(glm::floor(world_point / resolution));
}

template <typename T>
auto should_replace_with(T previous_absolute_distance, T new_absolute_distance, T new_distance) -> bool {
    bool equal = util::almost_equal(new_absolute_distance, previous_absolute_distance);
    return (!equal && new_absolute_distance < previous_absolute_distance) || (equal && new_distance >= T(0));
}

} // namespace ltb::dvh
