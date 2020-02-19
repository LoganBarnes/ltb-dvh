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
#include "ltb/cuda/cuda_check.hpp"

// external
#include <Corrade/Containers/ArrayViewStl.h>
#include <Magnum/GL/Buffer.h>
#include <cuda_gl_interop.h>
#include <cuda_runtime.h>

// standard
#include <vector>

namespace ltb {
namespace cuda {

/// @brief Maps an allocated OpenGL buffer for use with CUDA
template <typename T>
class GLBuffer {
public:
    explicit GLBuffer(const std::vector<T>&          initial_data,
                      Magnum::GL::Buffer::TargetHint type  = Magnum::GL::Buffer::TargetHint::Array,
                      Magnum::GL::BufferUsage        usage = Magnum::GL::BufferUsage::DynamicDraw);

    ~GLBuffer();

    GLBuffer(GLBuffer const&)     = delete;
    GLBuffer(GLBuffer&&) noexcept = delete;
    GLBuffer& operator=(GLBuffer const&) = delete;
    GLBuffer& operator=(GLBuffer&&) noexcept = delete;

    auto map_for_cuda() -> void;
    auto unmap_from_cuda() -> void;

    auto cuda_buffer(std::size_t* size) const -> T const*;
    auto gl_buffer() const -> Magnum::GL::Buffer const&;
    auto gl_buffer() -> Magnum::GL::Buffer&;

    auto size() const -> const std::size_t&;
    auto empty() const -> bool;

private:
    Magnum::GL::Buffer     gl_buffer_;
    std::size_t            size_              = 0;
    cudaGraphicsResource_t graphics_resource_ = nullptr;
    bool                   mapped_for_cuda_   = false;
};

template <typename T>
GLBuffer<T>::GLBuffer(const std::vector<T>&          initial_data,
                      Magnum::GL::Buffer::TargetHint gl_type,
                      Magnum::GL::BufferUsage        gl_usage)
    : gl_buffer_(gl_type), size_(initial_data.size()) {
    gl_buffer_.setData(initial_data, gl_usage);

    if (initial_data.empty()) {
        throw std::invalid_argument("'initial_data' is empty. CUDA/GL interop does not work with empty buffers");
    }
    CUDA_CHECK(cudaGraphicsGLRegisterBuffer(&graphics_resource_, gl_buffer_.id(), cudaGraphicsRegisterFlagsNone));
}

template <typename T>
GLBuffer<T>::~GLBuffer() {
    CUDA_CHECK(cudaGraphicsUnregisterResource(graphics_resource_));
}

template <typename T>
auto GLBuffer<T>::map_for_cuda() -> void {
    if (!mapped_for_cuda_) {
        CUDA_CHECK(cudaGraphicsMapResources(1, &graphics_resource_));
        mapped_for_cuda_ = true;
    }
}

template <typename T>
auto GLBuffer<T>::unmap_from_cuda() -> void {
    if (mapped_for_cuda_) {
        CUDA_CHECK(cudaGraphicsUnmapResources(1, &graphics_resource_));
        mapped_for_cuda_ = false;
    }
}

template <typename T>
auto GLBuffer<T>::cuda_buffer(std::size_t* size) const -> T const* {
    T*          devPtr    = nullptr;
    std::size_t byte_size = 0;
    CUDA_CHECK(cudaGraphicsResourceGetMappedPointer(reinterpret_cast<void**>(&devPtr), &byte_size, graphics_resource_));
    *size = byte_size / sizeof(T);
    return devPtr;
}

template <typename T>
auto GLBuffer<T>::gl_buffer() const -> Magnum::GL::Buffer const& {
    return gl_buffer_;
}

template <typename T>
auto GLBuffer<T>::gl_buffer() -> Magnum::GL::Buffer& {
    return gl_buffer_;
}

template <typename T>
auto GLBuffer<T>::size() const -> std::size_t const& {
    return size_;
}

template <typename T>
auto GLBuffer<T>::empty() const -> bool {
    return size() == 0ul;
}

} // namespace cuda
} // namespace ltb
