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
#include "renderable_dvh.hpp"

// project
#include "carpet/generic_guard.h"
#include "carpet/paths.h"
#include "gl/camera/camera.h"
#include "gl/opengl/program.h"
#include "gl/opengl/vertex_array.h"
#include "gl/shaders/gl_shaders.h"
#include "gpu/gl-interop/gl_buffer_map_guard.h"
#include "mesh/primitive_meshes.h"

// external
#include <thrust/device_ptr.h>
#include <thrust/sort.h>

// standard
#include <random>

#ifdef PROPER_CELL
#else
#endif

namespace {

struct DistanceFromCameraComparator {
    CUDA_FUNC bool operator()(const Cell& c1, const Cell& c2) {
#ifdef PROPER_CELL
        return num::distance(camera_position, num::vec3::make_from(c1.index))
            > num::distance(camera_position, num::vec3::make_from(c2.index));
#else
        return num::distance(camera_position, num::vec3::make_from(c1))
            > num::distance(camera_position, num::vec3::make_from(c2));
#endif
    }

    num::vec3 camera_position;
};

} // namespace

/**
 * The particle update and render logic.
 */
RenderableDvh::RenderableDvh(const num::ivec2& viewport) {
    {
        constexpr auto index_radius = 20.f;
        constexpr auto num_points   = 1000000u;

        // Create a bunch of uniformly distributed points on a sphere
        std::vector<num::vec3> points;
        points.reserve(num_points);
        {
            float                                 u, theta, coeff;
            std::mt19937                          gen{std::random_device{}()};
            std::uniform_real_distribution<float> u_dist(-1.f, 1.f);
            std::uniform_real_distribution<float> theta_dist(0.f, 2.f * num::constants::pi<float>());

            for (auto i = 0u; i < num_points; ++i) {
                // Uniform surface distribution
                u     = u_dist(gen);
                theta = theta_dist(gen);
                coeff = std::sqrt(1.f - u * u);

                points.emplace_back(coeff * std::cos(theta), coeff * std::sin(theta), u);
            }
        }

        std::vector<Cell> cells(points.size());

        std::transform(points.begin(), points.end(), cells.begin(), [](const auto& p3) {
#ifdef PROPER_CELL
            return Cell{num::lround<int>(p3 * index_radius), num::normalize(p3), 1.f, 3};
#else
            return num::swizzle(p3 * index_radius, 0.5f);
#endif
        });

        interop_cells_ = std::make_unique<gpu::GLBuffer<Cell>>(cells);
    }

    glEnable(GL_PROGRAM_POINT_SIZE);

    auto shader_path     = carpet::paths::project_root() / "loop" / "src" / "examples" / "dvh_viewer" / "shaders";
    auto shader_path_str = shader_path.string() + "/";

    auto point_shader_vert = gl::ShaderInfo::make_dynamic_shader(
#include "shaders/point_shader.vert"
        , "point_shader.vert", shader_path_str);

    auto point_shader_frag = gl::ShaderInfo::make_dynamic_shader(
#include "shaders/point_shader.frag"
        , "point_shader.frag", shader_path_str);

#ifdef PROPER_CELL
    std::vector<gl::VAOElement> elements{{"index", 3, GL_INT, reinterpret_cast<void*>(offsetof(Cell, index))},
                                         {"vector_to_closest_point",
                                          3,
                                          GL_FLOAT,
                                          reinterpret_cast<void*>(offsetof(Cell, vector_to_closest_point))},
                                         {"distance_to_closest_point",
                                          1,
                                          GL_FLOAT,
                                          reinterpret_cast<void*>(offsetof(Cell, distance_to_closest_point))},
                                         {"level", 1, GL_INT, reinterpret_cast<void*>(offsetof(Cell, level))}};
#else
    std::vector<gl::VAOElement> elements{{"center_and_radius", 4, GL_FLOAT, nullptr}};
#endif

    glpl_.program = std::make_shared<gl::Program>(point_shader_vert, point_shader_frag);
    glpl_.vao = std::make_shared<gl::VertexArray>(*glpl_.program, interop_cells_->gl_buffer(), sizeof(Cell), elements);

    resize(viewport);
}

void RenderableDvh::update(double /*time_step*/) {
    auto guard = gpu::make_gl_buffer_map_guard(*interop_cells_);

    std::size_t buffer_size;
    auto*       raw_device_ptr = interop_cells_->cuda_buffer(&buffer_size);

    auto device_ptr = thrust::device_pointer_cast(raw_device_ptr);

    thrust::sort(device_ptr, device_ptr + buffer_size, DistanceFromCameraComparator{camera_position_});
}

void RenderableDvh::on_render(const gl::Camera& camera,
                              const loop::SceneItem& /*item*/,
                              const loop::OpenGLSceneBackend& /*opengl_backend*/,
                              const glm::mat4& /*full_transform*/) const {

    auto scoped_blend         = carpet::make_guard(glEnable, glDisable, GL_BLEND);
    auto scoped_no_depth_mask = carpet::make_guard([] { glDepthMask(GL_FALSE); }, [] { glDepthMask(GL_TRUE); });

    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    // OpenGL rendering to test the OpenGL side of things
    glpl_.program->use([&] {
        glpl_.program->set_uniform(camera.get_projection_view_matrix(), "projection_from_world");
        glpl_.program->set_uniform(camera.get_projection_matrix(), "projection_from_view");
        glpl_.program->set_uniform(viewport_height_, "viewport_height");

#ifdef PROPER_CELL
        glpl_.program->set_uniform(base_level_resolution_, "base_level_resolution");
        glpl_.program->set_uniform(1, "shading_type");
#endif

        glpl_.vao->draw(GL_POINTS, 0, static_cast<int>(interop_cells_->size()));
    });
}

void RenderableDvh::resize(const num::ivec2& viewport) {
    viewport_height_ = viewport.y;
}

void RenderableDvh::set_camera_pos(num::vec3 cam_pos) {
    camera_position_ = cam_pos;
}
