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
#include "mesh.hpp"

namespace ltb {

auto is_triangle_format(gvs::GeometryFormat const& format) -> bool {
    return format == gvs::GeometryFormat::Triangles || format == gvs::GeometryFormat::TriangleFan
        || format == gvs::GeometryFormat::TriangleStrip;
}

template <int L, typename T>
auto Mesh<L, T>::append(Mesh<L, T> const& other) -> void {
    std::transform(other.indices.begin(),
                   other.indices.end(),
                   std::back_inserter(indices),
                   [add = vertices.size()](std::uint32_t index) { return index + add; });

    vertices.insert(vertices.end(), other.vertices.begin(), other.vertices.end());
    normals.insert(normals.end(), other.normals.begin(), other.normals.end());
    tex_coords.insert(tex_coords.end(), other.tex_coords.begin(), other.tex_coords.end());
    colors.insert(colors.end(), other.colors.begin(), other.colors.end());
}

template struct Mesh<2, float>;
template struct Mesh<3, float>;
template struct Mesh<2, double>;
template struct Mesh<3, double>;

} // namespace ltb
