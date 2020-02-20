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
#include "gl/opengl/opengl_types.h"
#include "gpu/gl-interop/gl_buffer.h"
#include "loop/scene/renderable_interface.h"
#include "num/vec.h"

// standard
#include <memory>

#define PROPER_CELL

#ifdef PROPER_CELL
struct Cell {
    num::ivec3 index;
    num::vec3  vector_to_closest_point;
    float      distance_to_closest_point = 0.f;
    int        level                     = 0;
};
#else
using Cell = num::vec4; // center and radius
#endif

/**
 * The particle update and render logic.
 */
class RenderableDvh : public loop::RenderableInterface {
public:
    explicit RenderableDvh(const num::ivec2& viewport);

    void update(double time_step);
    void on_render(const gl::Camera& camera,
                   const loop::SceneItem& /*item*/,
                   const loop::OpenGLSceneBackend& /*opengl_backend*/,
                   const glm::mat4& /*full_transform*/) const override;
    void resize(const num::ivec2& viewport);

    void set_camera_pos(num::vec3 cam_pos);

private:
    gl::StandardPipeline                 glpl_; ///< collection of OpenGL objects for rendering
    std::unique_ptr<gpu::GLBuffer<Cell>> interop_cells_; ///< GPU buffer shared by OpenGL and the CUDA

    int       viewport_height_       = 1;
    float     base_level_resolution_ = 0.1f;
    num::vec3 camera_position_       = num::vec3(0.f);
};
