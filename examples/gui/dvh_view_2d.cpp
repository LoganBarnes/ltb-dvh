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
#include "dvh_view_2d.hpp"

// project
#include "ltb/gvs/core/log_params.hpp"
#include "ltb/gvs/display/gui/scene_gui.hpp"
#include "ltb/sdf/sdf.hpp"
#include "ltb/util/container_utils.hpp"
#include "ltb/util/result.hpp"
#include "ltb/util/timer.hpp"
#include "scene_helpers.hpp"

// external
#include <Magnum/GL/Context.h>

// standard
#include <sstream>

using namespace Magnum;
using namespace Platform;

namespace ltb::example {
namespace {

void mesh_cell_border(std::vector<glm::vec3>* lines, glm::ivec2 const& cell, float resolution) {

    auto half_resolution = resolution * 0.5f;

    auto center = dvh::cell_center(cell, resolution);

    auto const bottom_left  = center + glm::vec2(-half_resolution, -half_resolution);
    auto const bottom_right = center + glm::vec2(+half_resolution, -half_resolution);
    auto const upper_right  = center + glm::vec2(+half_resolution, +half_resolution);
    auto const upper_left   = center + glm::vec2(-half_resolution, +half_resolution);

    lines->emplace_back(bottom_left.x, bottom_left.y, 1e-4f);
    lines->emplace_back(bottom_right.x, bottom_right.y, 1e-4f);

    lines->emplace_back(bottom_right.x, bottom_right.y, 1e-4f);
    lines->emplace_back(upper_right.x, upper_right.y, 1e-4f);

    lines->emplace_back(upper_right.x, upper_right.y, 1e-4f);
    lines->emplace_back(upper_left.x, upper_left.y, 1e-4f);

    lines->emplace_back(upper_left.x, upper_left.y, 1e-4f);
    lines->emplace_back(bottom_left.x, bottom_left.y, 1e-4f);
}

void mesh_cell(std::vector<glm::vec3>* triangles,
               std::vector<glm::vec3>* colors,
               glm::ivec2 const&       cell,
               float                   resolution,
               float                   distance_value,
               int                     level) {

    auto half_resolution = resolution * 0.5f;

    auto center = dvh::cell_center(cell, resolution);

    auto const bottom_left  = center + glm::vec2(-half_resolution, -half_resolution);
    auto const bottom_right = center + glm::vec2(+half_resolution, -half_resolution);
    auto const upper_right  = center + glm::vec2(+half_resolution, +half_resolution);
    auto const upper_left   = center + glm::vec2(-half_resolution, +half_resolution);

    triangles->emplace_back(bottom_left.x, bottom_left.y, -1e-4f * static_cast<float>(level));
    triangles->emplace_back(bottom_right.x, bottom_right.y, -1e-4f * static_cast<float>(level));
    triangles->emplace_back(upper_right.x, upper_right.y, -1e-4f * static_cast<float>(level));

    triangles->emplace_back(bottom_left.x, bottom_left.y, -1e-4f * static_cast<float>(level));
    triangles->emplace_back(upper_right.x, upper_right.y, -1e-4f * static_cast<float>(level));
    triangles->emplace_back(upper_left.x, upper_left.y, -1e-4f * static_cast<float>(level));

    glm::vec3 color;

    if (distance_value > 0.f) {
        color = {0.6f, 0.3f, 0.f}; // orange if the volume is outside the part

    } else {
        color = {0.0f, 0.6f, 0.6f}; // blue if the volume is inside or on the part
    }

    if (level != dvh::DistanceVolumeHierarchy<2>::base_level) {
        color *= std::pow<float>(0.8f, level);
    }

    colors->insert(colors->end(), 6, color);
}

} // namespace

DvhView2d::DvhView2d(gvs::ErrorAlertRecorder error_recorder)
    : error_recorder_(std::move(error_recorder)), dvh_(base_resolution_) {

    additive_lines_ = {
        sdf::make_oriented_line({0.f, 2.f}, {-2.f, 4.f}),
        sdf::make_oriented_line({-2.f, 4.f}, {-2.51f, 3.f}),
        sdf::make_oriented_line({-2.51f, 3.f}, {-4.f, 2.9f}),
        sdf::make_oriented_line({-4.f, 2.9f}, {-3.f, 0.001f}),
        sdf::make_oriented_line({-3.f, 0.001f}, {0.f, 2.f}),
    };

    additive_boxes_ = {
        sdf::make_geometry(sdf::make_box<2>({2.5f, 1.2f}), {0.5f, -0.75f}),
        sdf::make_geometry(sdf::make_box<2>({0.25f, 1.1f}), {3.7f, 2.f}),
    };

    scene_ = std::make_unique<gvs::LocalScene>();

    reset_volumes();
    reset_scene();
}

DvhView2d::~DvhView2d() = default;

void DvhView2d::update() {}

void DvhView2d::render(const gvs::CameraPackage& camera_package) const {
    scene_->render(camera_package);
}

void DvhView2d::configure_gui() {

    if (ImGui::DragFloat("Base Resolution", &base_resolution_, 0.05f, 0.1f, 2.f)) {
        reset_volumes();
        reset_scene();
    }

    if (!computation_time_message_.empty()) {
        ImGui::Text("%s", computation_time_message_.c_str());
    }

    ImGui::Separator();

    gvs::configure_gui(scene_.get());
}

void DvhView2d::resize(const Vector2i& viewport) {
    scene_->resize(viewport);
}

void DvhView2d::handleKeyPressEvent(Application::KeyEvent& /*event*/) {}

void DvhView2d::handleKeyReleaseEvent(Application::KeyEvent& /*event*/) {}

auto DvhView2d::handleMousePressEvent(Application::MouseEvent & /*event*/) -> void {}

auto DvhView2d::handleMouseReleaseEvent(Application::MouseEvent & /*event*/) -> void {}

auto DvhView2d::handleMouseMoveEvent(Application::MouseMoveEvent & /*event*/) -> void {}

void DvhView2d::reset_volumes() {
    dvh_ = dvh::DistanceVolumeHierarchy<2>{base_resolution_};

    std::stringstream ss;
    {
        util::ScopedTimer timer("Computation time", ss);
        dvh_.add_volume(additive_lines_);

        for (auto const& box : additive_boxes_) {
            dvh_.add_volume(decltype(additive_boxes_){box});
        }
    }
    computation_time_message_ = ss.str();
}

void DvhView2d::reset_scene() {
    scene_->clear();
    index_scene_ids_.clear();

    scene_->add_item(gvs::SetReadableId("Axes"), gvs::SetPrimitive(gvs::Axes{}));
    dvh_root_scene_id_ = scene_->add_item(gvs::SetReadableId("DVH"), gvs::SetPositions3d());

    auto oriented_lines_scene_id = add_lines_to_scene(scene_.get(), additive_lines_);
    scene_->update_item(oriented_lines_scene_id, gvs::SetReadableId("Additive Lines"));

    auto squares_scene_id = add_boxes_to_scene(scene_.get(), additive_boxes_);

    scene_->update_item(squares_scene_id,
                        gvs::SetReadableId("Additive Boxes"),
                        gvs::SetColoring(gvs::Coloring::UniformColor),
                        gvs::SetShading(gvs::Shading::UniformColor),
                        gvs::SetUniformColor({1.f, 1.f, 1.f}));

    for (auto const& [level_index, sparse_distance_field] : dvh_.levels()) {

        if (!util::has_key(index_scene_ids_, level_index)) {
            index_scene_ids_.emplace(level_index,
                                     scene_->add_item(gvs::SetReadableId("Level " + std::to_string(level_index)),
                                                      gvs::SetPositions3d(),
                                                      gvs::SetParent(dvh_root_scene_id_)));

            auto const& level_scene_id = index_scene_ids_.at(level_index);

            scene_->add_item(gvs::SetReadableId("Cell Values"),
                             gvs::SetPositions3d(),
                             gvs::SetTriangles(),
                             gvs::SetShading(gvs::Shading::UniformColor),
                             gvs::SetColoring(gvs::Coloring::VertexColors),
                             gvs::SetParent(level_scene_id));

            scene_->add_item(gvs::SetReadableId("Cell Borders"),
                             gvs::SetPositions3d(),
                             gvs::SetLines(),
                             gvs::SetShading(gvs::Shading::UniformColor),
                             gvs::SetColoring(gvs::Coloring::UniformColor),
                             gvs::SetUniformColor({0.f, 0.f, 0.f}),
                             gvs::SetParent(level_scene_id));
        }

        auto resolution = dvh_.resolution(level_index);

        auto const& scene_id = index_scene_ids_.at(level_index);

        std::vector<gvs::SceneId> children;
        scene_->get_item_info(scene_id, gvs::GetChildren(&children));
        assert(children.size() == 2ul);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> colors;

        for (auto const& [cell, dir_and_dist] : sparse_distance_field) {
            mesh_cell(&positions, &colors, cell, resolution, dir_and_dist[2], level_index);
        }
        scene_->update_item(children[0], gvs::SetPositions3d(positions), gvs::SetVertexColors3d(colors));

        positions.clear();
        for (auto const& [cell, dir_and_dist] : sparse_distance_field) {
            util::ignore(dir_and_dist);
            mesh_cell_border(&positions, cell, resolution);
        }
        scene_->update_item(children[1], gvs::SetPositions3d(positions), gvs::SetLines());
    }
}

} // namespace ltb::example
