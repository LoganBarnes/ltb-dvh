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
#include "ltb/cuda/cuda_func.hpp"

// external
#include "glm/geometric.hpp"

namespace ltb {
namespace sdf {

template <int L, typename T = float>
struct Geometry {
    virtual ~Geometry() = 0;

    LTB_CUDA_FUNC virtual auto vector_from(glm::vec<L, T> const& point) const -> glm::vec<L, T> = 0;
    LTB_CUDA_FUNC virtual auto distance_from(glm::vec<L, T> const& point) const -> T            = 0;
    LTB_CUDA_FUNC virtual auto bounding_box() const -> AABB<L, T>                               = 0;
};

template <int L, typename T>
Geometry<L, T>::~Geometry() = default;

} // namespace sdf
} // namespace ltb