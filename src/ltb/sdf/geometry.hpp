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

// standard
#include <type_traits>

// https://stackoverflow.com/a/16824239

#define HAS_FUNCTION(name)                                                                                             \
    template <typename, typename T>                                                                                    \
    struct has_##name {                                                                                                \
        static_assert(std::integral_constant<T, false>::value,                                                         \
                      "Second template parameter needs to be of function type.");                                      \
    };                                                                                                                 \
                                                                                                                       \
    template <typename C, typename Ret, typename... Args>                                                              \
    struct has_##name<C, Ret(Args...)> {                                                                               \
    private:                                                                                                           \
        template <typename T>                                                                                          \
        static constexpr auto check(T*) ->                                                                             \
            typename std::is_same<decltype(std::declval<T>().name(std::declval<Args>()...)), Ret>::type;               \
                                                                                                                       \
        template <typename>                                                                                            \
        static constexpr std::false_type check(...);                                                                   \
                                                                                                                       \
        typedef decltype(check<C>(0)) type;                                                                            \
                                                                                                                       \
    public:                                                                                                            \
        static constexpr bool value = type::value;                                                                     \
    };

HAS_FUNCTION(vector_from)
HAS_FUNCTION(distance_from)
HAS_FUNCTION(bounding_box)

template <template <int, typename> class G, int L, typename T>
constexpr bool has_vector_from_v = has_vector_from<G<L, T>, glm::vec<L, T>(glm::vec<L, T> const&)>::value;

template <template <int, typename> class G, int L, typename T>
constexpr bool has_distance_from_v = has_distance_from<G<L, T>, glm::vec<L, T>(glm::vec<L, T> const&)>::value;

template <template <int, typename> class G, int L, typename T>
constexpr bool has_bounding_box_v = has_bounding_box<G<L, T>, ::ltb::sdf::AABB<L, T>()>::value;

template <template <int, typename> class G, int L, typename T>
constexpr bool is_geometry_v
    = has_bounding_box_v<G, L, T>() && has_vector_from_v<G, L, T>() && has_distance_from_v<G, L, T>();
