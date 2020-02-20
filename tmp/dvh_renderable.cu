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
#include "dvh_renderable.hpp"

// project
#include "buffer_map_guard.hpp"

// external
#include <Magnum/Math/Vector2.h>
#include <glm/ext/scalar_constants.hpp>
#include <thrust/device_ptr.h>
#include <thrust/sort.h>

// standard
#include <algorithm>
#include <random>

namespace ltb {
namespace example {

namespace {

using namespace Magnum::Math::Literals;

struct DistanceFromCameraComparator {
    LTB_CUDA_FUNC bool operator()(const Cell& c1, const Cell& c2) {
        return glm::distance(camera_position, c1.center_point(level_0_resolution))
            > glm::distance(camera_position, c2.center_point(level_0_resolution));
    }

    glm::vec3 camera_position;
    float     level_0_resolution;
};

auto create_cells() {
    constexpr auto index_radius = 20.f;
    constexpr auto num_points   = 1000000u;

    // Create a bunch of uniformly distributed points on a sphere
    std::vector<glm::vec3> points;
    points.reserve(num_points);
    {
        float                                 u, theta, coeff;
        std::mt19937                          gen{std::random_device{}()};
        std::uniform_real_distribution<float> u_dist(-1.f, 1.f);
        std::uniform_real_distribution<float> theta_dist(0.f, 2.f * glm::pi<float>());

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
        return Cell{glm::ivec3(glm::round(p3 * index_radius)), glm::normalize(p3), 3};
    });

    return cells;
}

} // namespace

LTB_CUDA_FUNC auto Cell::center_point(float level_0_resolution) const -> glm::vec3 {
    return glm::vec3(index) * level_0_resolution * glm::pow(2.f, static_cast<float>(level));
}

DvhRenderable::DvhRenderable(glm::ivec2 viewport) {
    {
        auto cells     = create_cells();
        interop_cells_ = std::make_unique<cuda::GLBuffer<Cell>>(cells);
    }

    glEnable(GL_PROGRAM_POINT_SIZE);

    mesh_.addVertexBuffer(interop_cells_->gl_buffer(),
                          0,
                          dvh::CellShader::Index(),
                          dvh::CellShader::VectorToClosestPoint(),
                          dvh::CellShader::Level());
    mesh_.setCount(interop_cells_->size());
    mesh_.setPrimitive(Magnum::GL::MeshPrimitive::Points);

    resize(viewport);
}

void DvhRenderable::update(double /*time_step*/) {
    auto guard = cuda::make_gl_buffer_map_guard(*interop_cells_);

    std::size_t buffer_size;
    auto*       raw_device_ptr = interop_cells_->cuda_buffer(&buffer_size);

    auto device_ptr = thrust::device_pointer_cast(raw_device_ptr);

    thrust::sort(device_ptr,
                 device_ptr + static_cast<decltype(device_ptr)::difference_type>(buffer_size),
                 DistanceFromCameraComparator{camera_position_, base_level_resolution_});
}

void DvhRenderable::render(const gvs::CameraPackage& camera_package) const {

    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);

    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    shader_
        .set_projection_from_world_matrix(camera_package.camera->projectionMatrix()
                                          * camera_package.camera->cameraMatrix())
        .set_projection_from_view_matrix(camera_package.camera->projectionMatrix())
        .set_viewport_height(viewport_.y)
        .set_base_level_resolution(base_level_resolution_)
        .set_coloring_type(dvh::CellColoring::Normals)
        .set_shading_type(gvs::Shading::UniformColor);

    mesh_.draw(shader_);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void DvhRenderable::configure_gui() {}

void DvhRenderable::resize(glm::ivec2 viewport) {
    viewport_ = viewport;
}

} // namespace example
} // namespace ltb
