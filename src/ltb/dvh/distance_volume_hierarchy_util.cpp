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
#include "distance_volume_hierarchy_util.hpp"

// external
#include <doctest/doctest.h>

namespace ltb::dvh {

template <>
auto children_cells(glm::ivec2 const& cell) -> std::vector<glm::ivec2> {
    auto cell2 = cell * 2;
    return {
        cell2 + glm::ivec2(0, 0),
        cell2 + glm::ivec2(1, 0),
        cell2 + glm::ivec2(0, 1),
        cell2 + glm::ivec2(1, 1),
    };
}

template <>
auto children_cells(glm::ivec3 const& cell) -> std::vector<glm::ivec3> {
    auto cell2 = cell * 2;
    return {
        cell2 + glm::ivec3(0, 0, 0),
        cell2 + glm::ivec3(1, 0, 0),
        cell2 + glm::ivec3(0, 1, 0),
        cell2 + glm::ivec3(1, 1, 0),
        cell2 + glm::ivec3(0, 0, 1),
        cell2 + glm::ivec3(1, 0, 1),
        cell2 + glm::ivec3(0, 1, 1),
        cell2 + glm::ivec3(1, 1, 1),
    };
}

TEST_CASE("parent_cell 2d around origin [dvh]") {
    CHECK(parent_cell<2>({0, 0}) == glm::ivec2{0, 0});
    CHECK(parent_cell<2>({1, 0}) == glm::ivec2{0, 0});
    CHECK(parent_cell<2>({0, 1}) == glm::ivec2{0, 0});
    CHECK(parent_cell<2>({1, 1}) == glm::ivec2{0, 0});

    CHECK(parent_cell<2>({-1, 0}) == glm::ivec2{-1, 0});
    CHECK(parent_cell<2>({-2, 0}) == glm::ivec2{-1, 0});
    CHECK(parent_cell<2>({-1, 1}) == glm::ivec2{-1, 0});
    CHECK(parent_cell<2>({-2, 1}) == glm::ivec2{-1, 0});

    CHECK(parent_cell<2>({-1, -1}) == glm::ivec2{-1, -1});
    CHECK(parent_cell<2>({-2, -1}) == glm::ivec2{-1, -1});
    CHECK(parent_cell<2>({-1, -2}) == glm::ivec2{-1, -1});
    CHECK(parent_cell<2>({-2, -2}) == glm::ivec2{-1, -1});

    CHECK(parent_cell<2>({0, -1}) == glm::ivec2{0, -1});
    CHECK(parent_cell<2>({0, -1}) == glm::ivec2{0, -1});
    CHECK(parent_cell<2>({1, -2}) == glm::ivec2{0, -1});
    CHECK(parent_cell<2>({1, -2}) == glm::ivec2{0, -1});
}

TEST_CASE_TEMPLATE("parent_cell diagnals [dvh]", V, glm::ivec2, glm::ivec3) {
    CHECK(parent_cell(V{0}) == V{0});
    CHECK(parent_cell(V{1}) == V{0});
    CHECK(parent_cell(V{2}) == V{1});
    CHECK(parent_cell(V{3}) == V{1});
    CHECK(parent_cell(V{4}) == V{2});

    CHECK(parent_cell(V{-1}) == V{-1});
    CHECK(parent_cell(V{-2}) == V{-1});
    CHECK(parent_cell(V{-3}) == V{-2});
    CHECK(parent_cell(V{-4}) == V{-2});
    CHECK(parent_cell(V{-5}) == V{-3});
}

TEST_CASE("children_cells 2d around origin [dvh]") {
    CHECK(children_cells<2>({0, 0})
          == std::vector<glm::ivec2>{
              {0, 0},
              {1, 0},
              {0, 1},
              {1, 1},
          });

    CHECK(children_cells<2>({-1, 0})
          == std::vector<glm::ivec2>{
              {-2, 0},
              {-1, 0},
              {-2, 1},
              {-1, 1},
          });

    CHECK(children_cells<2>({-1, -1})
          == std::vector<glm::ivec2>{
              {-2, -2},
              {-1, -2},
              {-2, -1},
              {-1, -1},
          });

    CHECK(children_cells<2>({0, -1})
          == std::vector<glm::ivec2>{
              {0, -2},
              {1, -2},
              {0, -1},
              {1, -1},
          });
}

} // namespace ltb::dvh
