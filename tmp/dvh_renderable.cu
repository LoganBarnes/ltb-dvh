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
#include "ltb/gvs/display/gui/imgui_colors.hpp"

// external
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Vector2.h>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtx/hash.hpp>
#include <thrust/device_ptr.h>
#include <thrust/sort.h>

// standard
#include <algorithm>
#include <random>
#include <unordered_set>

//#define NOT_UNIQUE

using namespace Magnum;
using namespace Math::Literals;

namespace ltb {
namespace example {

namespace {

struct DistanceFromCameraComparator {
    LTB_CUDA_FUNC bool operator()(const Cell& c1, const Cell& c2) {
        return glm::distance(camera_position, c1.center_point(level_0_resolution))
            > glm::distance(camera_position, c2.center_point(level_0_resolution));
    }

    glm::vec3 camera_position;
    float     level_0_resolution;
};

//auto to_1d_index(glm::ivec2 const& index, glm::ivec2 const& dimensions) -> std::size_t {
//    return static_cast<std::size_t>(index.y) * static_cast<std::size_t>(dimensions.x) + index.x;
//}
//
//auto to_1d_index(glm::ivec3 const& index, glm::ivec3 const& min_index, glm::ivec3 const& max_index) -> std::size_t {
//    return index.z
//}

auto create_cells() {
    constexpr auto index_radius = 10.f;
    constexpr auto num_points   = 1000000u;

    // Create a bunch of uniformly distributed points on a sphere
#ifdef NOT_UNIQUE
    std::vector<glm::ivec3> points;
    points.reserve(num_points);
#else
    std::unordered_set<glm::ivec3> points;
#endif
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

            auto p = glm::vec3(coeff * std::cos(theta), coeff * std::sin(theta), u);
#ifdef NOT_UNIQUE
            points.emplace_back(glm::ivec3(glm::round(p * index_radius)));
#else
            points.emplace(glm::ivec3(glm::round(p * index_radius)));
#endif
        }
    }

    std::vector<Cell> cells(points.size());

    std::transform(points.begin(), points.end(), cells.begin(), [](const auto& index) {
        return Cell{index, glm::normalize(glm::vec3(index)), 3};
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
    mesh_.setPrimitive(GL::MeshPrimitive::Points);

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
    if (!visible_) {
        return;
    }

    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::setDepthMask(false);

    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add, GL::Renderer::BlendEquation::Add);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
                                   GL::Renderer::BlendFunction::OneMinusSourceAlpha,
                                   GL::Renderer::BlendFunction::One,
                                   GL::Renderer::BlendFunction::Zero);

    shader_
        .set_projection_from_world_matrix(camera_package.camera->projectionMatrix()
                                          * camera_package.camera->cameraMatrix())
        .set_projection_from_view_matrix(camera_package.camera->projectionMatrix())
        .set_viewport_height(viewport_.y)
        .set_base_level_resolution(base_level_resolution_)
        .set_coloring_type(dvh::CellColoring::UniformColor)
        .set_shading_type(gvs::Shading::Lambertian);

    mesh_.draw(shader_);

    GL::Renderer::setDepthMask(true);
    GL::Renderer::disable(GL::Renderer::Feature::Blending);
}

void DvhRenderable::configure_gui() {
    ImGui::Checkbox("###dvh_visible", &visible_);

    ImGui::SameLine();
    if (ImGui::TreeNode("DVH")) {
        ImGui::TextColored(gvs::gray(), "No settings");
        ImGui::TreePop();
    }
}

void DvhRenderable::resize(glm::ivec2 viewport) {
    viewport_ = viewport;
}
void DvhRenderable::set_camera_position(glm::vec3 cam_pos) {
    camera_position_ = cam_pos;
}

} // namespace example
} // namespace ltb
