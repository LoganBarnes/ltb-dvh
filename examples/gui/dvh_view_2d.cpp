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
#include "scene_helpers.hpp"

// external
#include <Magnum/GL/Context.h>

using namespace Magnum;
using namespace Platform;

namespace ltb::example {
namespace {

void mesh_cell_border(std::vector<glm::vec3>* lines, glm::ivec2 const& cell, float resolution) {

    auto half_resolution = resolution * 0.5f;

    auto const bottom_left  = glm::vec2(cell) + glm::vec2(-half_resolution, -half_resolution);
    auto const bottom_right = glm::vec2(cell) + glm::vec2(+half_resolution, -half_resolution);
    auto const upper_right  = glm::vec2(cell) + glm::vec2(+half_resolution, +half_resolution);
    auto const upper_left   = glm::vec2(cell) + glm::vec2(-half_resolution, +half_resolution);

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
               float                   distance_value) {

    auto half_resolution = resolution * 0.5f;

    auto const bottom_left  = glm::vec2(cell) + glm::vec2(-half_resolution, -half_resolution);
    auto const bottom_right = glm::vec2(cell) + glm::vec2(+half_resolution, -half_resolution);
    auto const upper_right  = glm::vec2(cell) + glm::vec2(+half_resolution, +half_resolution);
    auto const upper_left   = glm::vec2(cell) + glm::vec2(-half_resolution, +half_resolution);

    triangles->emplace_back(bottom_left.x, bottom_left.y, 0.f);
    triangles->emplace_back(bottom_right.x, bottom_right.y, 0.f);
    triangles->emplace_back(upper_right.x, upper_right.y, 0.f);

    triangles->emplace_back(bottom_left.x, bottom_left.y, 0.f);
    triangles->emplace_back(upper_right.x, upper_right.y, 0.f);
    triangles->emplace_back(upper_left.x, upper_left.y, 0.f);

    auto color = (std::isinf(distance_value) ? glm::vec3(0.3f) : glm::vec3{0.6f, 0.3f, 0.f});
    colors->insert(colors->end(), 6, color);
}

} // namespace

DvhView2d::DvhView2d(gvs::ErrorAlertRecorder error_recorder) : error_recorder_(std::move(error_recorder)), dvh_(1.f) {

    additive_boxes_ = {
        sdf::make_geometry(sdf::make_box<2>({2.5f, 1.2f}), {0.5f, -0.75f}),
        sdf::make_geometry(sdf::make_box<2>({0.25f, 1.1f}), {3.7f, 2.f}),
    };

    dvh_.add_volumes(additive_boxes_);

    scene_.add_item(gvs::SetReadableId("Axes"), gvs::SetPrimitive(gvs::Axes{}));
    dvh_root_scene_id_ = scene_.add_item(gvs::SetReadableId("DVH"), gvs::SetPositions3d());

    auto squares_scene_id = add_boxes_to_scene(&scene_, additive_boxes_);

    scene_.update_item(squares_scene_id,
                       gvs::SetReadableId("Additive Boxes"),
                       gvs::SetColoring(gvs::Coloring::UniformColor),
                       gvs::SetShading(gvs::Shading::UniformColor),
                       gvs::SetUniformColor({1.f, 1.f, 1.f}));
}

DvhView2d::~DvhView2d() = default;

void DvhView2d::update() {

    for (auto const& [level_index, sparse_distance_field] : dvh_.levels()) {

        if (!util::has_key(index_scene_ids_, level_index)) {
            index_scene_ids_.emplace(level_index,
                                     scene_.add_item(gvs::SetReadableId("Level " + std::to_string(level_index)),
                                                     gvs::SetPositions3d(),
                                                     gvs::SetParent(dvh_root_scene_id_)));

            auto const& level_scene_id = index_scene_ids_.at(level_index);

            scene_.add_item(gvs::SetReadableId("Cell Values"),
                            gvs::SetPositions3d(),
                            gvs::SetTriangles(),
                            gvs::SetShading(gvs::Shading::UniformColor),
                            gvs::SetColoring(gvs::Coloring::VertexColors),
                            gvs::SetParent(level_scene_id));

            scene_.add_item(gvs::SetReadableId("Cell Borders"),
                            gvs::SetPositions3d(),
                            gvs::SetLines(),
                            gvs::SetShading(gvs::Shading::UniformColor),
                            gvs::SetColoring(gvs::Coloring::UniformColor),
                            gvs::SetUniformColor({0.f, 0.f, 0.f}),
                            gvs::SetParent(level_scene_id));
        }

        auto resolution = dvh_.level_resolution(level_index);

        auto const& scene_id = index_scene_ids_.at(level_index);

        std::vector<gvs::SceneId> children;
        scene_.get_item_info(scene_id, gvs::GetChildren(&children));
        assert(children.size() == 2ul);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> colors;

        for (auto const& [cell, dir_and_dist] : sparse_distance_field) {
            mesh_cell(&positions, &colors, cell, resolution, dir_and_dist.z);
        }
        scene_.update_item(children[0], gvs::SetPositions3d(positions), gvs::SetVertexColors3d(colors));

        positions.clear();
        for (auto const& [cell, dir_and_dist] : sparse_distance_field) {
            util::ignore(dir_and_dist);
            mesh_cell_border(&positions, cell, resolution);
        }
        scene_.update_item(children[1], gvs::SetPositions3d(positions), gvs::SetLines());
    }
}

void DvhView2d::render(const gvs::CameraPackage& camera_package) const {
    scene_.render(camera_package);
}

void DvhView2d::configure_gui() {
    gvs::configure_gui(&scene_);
}

void DvhView2d::resize(const Vector2i& viewport) {
    scene_.resize(viewport);
}

void DvhView2d::handleKeyPressEvent(Application::KeyEvent& /*event*/) {}

void DvhView2d::handleKeyReleaseEvent(Application::KeyEvent& /*event*/) {}

auto DvhView2d::handleMousePressEvent(Application::MouseEvent & /*event*/) -> void {}

auto DvhView2d::handleMouseReleaseEvent(Application::MouseEvent & /*event*/) -> void {}

auto DvhView2d::handleMouseMoveEvent(Application::MouseMoveEvent & /*event*/) -> void {}

} // namespace ltb::example
