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
#include "ltb/gvs/core/types.hpp"
#include "ltb/sdf/aabb.hpp"

// external
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

// standard
#include <vector>

namespace ltb {

bool is_triangle_format(const gvs::GeometryFormat& format);

template <int L, typename T>
struct Mesh {
    gvs::GeometryFormat geometry_format = gvs::GeometryFormat::Points;

    std::vector<glm::vec<L, T>>     vertices;
    std::vector<glm::vec<L, T>>     normals;
    std::vector<glm::vec<L - 1, T>> tex_coords;
    std::vector<glm::vec3>          colors;

    std::vector<std::uint32_t> indices;

    auto append(const Mesh<L, T>& other) -> void;
};

template <int L, typename T>
auto compute_bounding_box(const Mesh<L, T>& mesh) -> sdf::AABB<L, T> {
    sdf::AABB<L, T> bbox = {};
    for (const glm::vec<L, T>& vertex : mesh.vertices) {
        bbox = sdf::expand(bbox, vertex);
    }
    return bbox;
}

template <int L, typename T>
auto transform(Mesh<L, T> mesh, const glm::mat<L + 1, L + 1, T>& transformation) -> Mesh<L, T> {
    for (auto& vertex : mesh.vertices) {
        vertex = glm::vec<L, T>(transformation * glm::vec<L + 1, T>(vertex, T(1)));
    }
    for (auto& normal : mesh.normals) {
        normal = glm::vec<L, T>(transformation * glm::vec<L + 1, T>(normal, T(0)));
    }
    return mesh;
}

using Mesh2  = Mesh<2, float>;
using Mesh3  = Mesh<3, float>;
using DMesh2 = Mesh<2, double>;
using DMesh3 = Mesh<3, double>;

} // namespace ltb
