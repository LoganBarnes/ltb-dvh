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
#include "glm/geometric.hpp"

namespace ltb {
namespace sdf {

template <template <int, typename> class G, int L, typename T = float>
struct TransformedGeometry : public Geometry<L, T> {
    G<L, T>        geometry;
    glm::vec<L, T> translation;

    explicit TransformedGeometry(G<L, T> geom, glm::vec<L, T> trans = {}) : geometry(geom), translation(trans) {}

    LTB_CUDA_FUNC auto vector_from(glm::vec<L, T> const& point) const -> glm::vec<L, T> override;
    LTB_CUDA_FUNC auto distance_from(glm::vec<L, T> const& point) const -> T override;
    LTB_CUDA_FUNC auto bounding_box() const -> AABB<L, T> override;

    LTB_CUDA_FUNC auto to_geometry_space(glm::vec<L, T> const& point) const -> glm::vec<L, T>;
    LTB_CUDA_FUNC auto from_geometry_space(glm::vec<L, T> const& point) const -> glm::vec<L, T>;
};

template <template <int, typename> class G,
          int L,
          typename T = float,
          typename   = typename std::enable_if<std::is_base_of<Geometry<L, T>, G<L, T>>::value>::type>
auto make_transformed_geometry(G<L, T> geometry, glm::vec<L, T> translation = {}) {
    return TransformedGeometry<G, L, T>(geometry, translation);
}

template <template <int, typename> class G, int L, typename T>
LTB_CUDA_FUNC auto TransformedGeometry<G, L, T>::vector_from(glm::vec<L, T> const& point) const -> glm::vec<L, T> {
    return geometry.vector_from(to_geometry_space(point));
}

template <template <int, typename> class G, int L, typename T>
LTB_CUDA_FUNC auto TransformedGeometry<G, L, T>::distance_from(glm::vec<L, T> const& point) const -> T {
    return geometry.distance_from(to_geometry_space(point));
}

template <template <int, typename> class G, int L, typename T>
LTB_CUDA_FUNC auto TransformedGeometry<G, L, T>::bounding_box() const -> AABB<L, T> {
    auto aabb = geometry.bounding_box();
    return {from_geometry_space(aabb.min_point), from_geometry_space(aabb.max_point)};
}

template <template <int, typename> class G, int L, typename T>
LTB_CUDA_FUNC auto TransformedGeometry<G, L, T>::to_geometry_space(glm::vec<L, T> const& point) const
    -> glm::vec<L, T> {
    return point - translation;
}

template <template <int, typename> class G, int L, typename T>
LTB_CUDA_FUNC auto TransformedGeometry<G, L, T>::from_geometry_space(glm::vec<L, T> const& point) const
    -> glm::vec<L, T> {
    return point + translation;
}

} // namespace sdf
} // namespace ltb