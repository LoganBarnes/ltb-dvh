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
#include "ltb/dvh/cuda_check.hpp"

// external
#include <Corrade/Containers/ArrayViewStl.h>
#include <Magnum/GL/Buffer.h>
#include <cuda_gl_interop.h>
#include <cuda_runtime.h>

// standard
#include <vector>

namespace cuda {
namespace interop {

/// @brief Maps an allocated OpenGL buffer for use with CUDA
template <typename T>
class GLBuffer {
public:
    explicit GLBuffer(const std::vector<T>& initial_data,
                      Magnum::GL::Buffer::TargetHint type = Magnum::GL::Buffer::TargetHint::Array,
                      Magnum::GL::BufferUsage usage = Magnum::GL::BufferUsage::DynamicDraw);
    ~GLBuffer();

    void map_for_cuda();
    void unmap_from_cuda();

    T* cuda_buffer(std::size_t* size) const;
    const Magnum::GL::Buffer& gl_buffer() const;

    const std::size_t& size() const;
    bool empty() const;

private:
    Magnum::GL::Buffer gl_buffer_;
    std::size_t size_ = 0;
    cudaGraphicsResource_t graphics_resource_ = nullptr;
    bool mapped_for_cuda_ = false;
};

template <typename T>
GLBuffer<T>::GLBuffer(const std::vector<T>& initial_data,
                      Magnum::GL::Buffer::TargetHint gl_type,
                      Magnum::GL::BufferUsage gl_usage)
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
void GLBuffer<T>::map_for_cuda() {
    if (!mapped_for_cuda_) {
        CUDA_CHECK(cudaGraphicsMapResources(1, &graphics_resource_));
        mapped_for_cuda_ = true;
    }
}

template <typename T>
void GLBuffer<T>::unmap_from_cuda() {
    if (mapped_for_cuda_) {
        CUDA_CHECK(cudaGraphicsUnmapResources(1, &graphics_resource_));
        mapped_for_cuda_ = false;
    }
}

template <typename T>
T* GLBuffer<T>::cuda_buffer(std::size_t* size) const {
    T* devPtr = nullptr;
    std::size_t byte_size = 0;
    CUDA_CHECK(cudaGraphicsResourceGetMappedPointer(reinterpret_cast<void**>(&devPtr), &byte_size, graphics_resource_));
    *size = byte_size / sizeof(T);
    return devPtr;
}

template <typename T>
const Magnum::GL::Buffer& GLBuffer<T>::gl_buffer() const {
    return gl_buffer_;
}

template <typename T>
const std::size_t& GLBuffer<T>::size() const {
    return size_;
}

} // namespace interop
} // namespace cuda
