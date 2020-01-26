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

namespace ltb {
namespace sdf {

template <int L, typename T = float>
struct AABB {
    glm::vec<L, T> min_point = glm::vec<L, T>(std::numeric_limits<T>::infinity());
    glm::vec<L, T> max_point = glm::vec<L, T>(-std::numeric_limits<T>::infinity());
};

template <int L, typename T = float>
auto expand(AABB<L, T> const& aabb, glm::vec<L, T> const& point) -> AABB<L, T> {
    return {glm::min(aabb.min_point, point), glm::max(aabb.max_point, point)};
}

} // namespace sdf
} // namespace ltb
