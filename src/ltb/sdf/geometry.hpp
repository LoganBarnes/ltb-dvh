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

namespace ltb::sdf {

template <template <int, typename> class G, int L, typename T = float>
struct Geometry {
    G<L, T>        geometry;
    glm::vec<L, T> translation;

    explicit Geometry(G<L, T> geom, glm::vec<L, T> trans = {}) : geometry(geom), translation(trans) {}

    auto to_geometry_space(glm::vec<L, T> const& point) const -> glm::vec<L, T>;
};

template <template <int, typename> class G, int L, typename T = float>
auto make_geometry(G<L, T> geometry, glm::vec<L, T> transation = {}) {
    return Geometry<G, L, T>(geometry, transation);
}

template <template <int, typename> class G, int L, typename T>
auto Geometry<G, L, T>::to_geometry_space(glm::vec<L, T> const& point) const -> glm::vec<L, T> {
    return point - translation;
}

template <template <int, typename> class G,
          int L,
          typename T = float,
          typename   = std::enable_if_t<std::is_floating_point_v<T>>>
auto vector_to_geometry(glm::vec<L, T> const& point, Geometry<G, L, T> const& geometry) -> glm::vec<L, T> {
    return vector_to_geometry(geometry.to_geometry_space(point), geometry.geometry);
}

template <template <int, typename> class G,
          int L,
          typename T = float,
          typename   = std::enable_if_t<std::is_floating_point_v<T>>>
auto distance_to_geometry(glm::vec<L, T> const& point, Geometry<G, L, T> const& geometry) {
    return distance_to_geometry(geometry.to_geometry_space(point), geometry.geometry);
}

} // namespace ltb::sdf