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
#include "Magnum/GL/Mesh.h"
#include "gl_buffer.hpp"
#include "ltb/cuda/cuda_func.hpp"
#include "ltb/gvs/display/camera_package.hpp"
#include "shaders/cell_shader.hpp"

// external
#include <glm/glm.hpp>

// standard
#include <memory>

namespace ltb {
namespace example {

struct Cell {
    glm::ivec3 index;
    glm::vec3  vector_to_closest_point;
    int        level = 0;

    LTB_CUDA_FUNC auto center_point(float level_0_resolution) const -> glm::vec3;
};

/**
 * The particle update and render logic.
 */
class DvhRenderable {
public:
    explicit DvhRenderable(glm::ivec2 viewport);

    void update(double time_step);
    void render(const gvs::CameraPackage& camera_package) const;
    void configure_gui();

    void resize(glm::ivec2 viewport);
    void set_camera_position(glm::vec3 cam_pos);

private:
    mutable dvh::CellShader  shader_;
    mutable Magnum::GL::Mesh mesh_;

    std::unique_ptr<cuda::GLBuffer<Cell>> interop_cells_; ///< GPU buffer shared by OpenGL and the CUDA

    // TODO: hook this stuff up
    glm::ivec2 viewport_;
    float      base_level_resolution_ = 0.1f;
    glm::vec3  camera_position_       = glm::vec3(0.f);
};

} // namespace example
} // namespace ltb
