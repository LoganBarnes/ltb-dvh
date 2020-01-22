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
#include "ltb/gvs/display/gui/imgui_utils.hpp"
#include "ltb/gvs/display/gui/scene_gui.hpp"
#include "ltb/util/container_utils.hpp"

// external
#include <Magnum/GL/Context.h>
#include <glm/gtx/string_cast.hpp>
#include <imgui.h>

using namespace Magnum;
using namespace Platform;

namespace ltb::example {
namespace {

void build_grids(dvh::DistanceVolumeHierarchy<2> const& dvh, gvs::Scene* scene, gvs::SceneId const& parent) {

    std::vector<glm::vec3> lines;

    for (int i = dvh.min_index().x; i <= dvh.max_index().x + 1; ++i) {
        lines.emplace_back(i, dvh.min_index().y, 0.f);
        lines.emplace_back(i, dvh.max_index().y + 1, 0.f);
    }
    for (int i = dvh.min_index().y; i <= dvh.max_index().y + 1; ++i) {
        lines.emplace_back(dvh.min_index().x, i, 0.f);
        lines.emplace_back(dvh.max_index().x + 1, i, 0.f);
    }

    scene->add_item(gvs::SetPositions3d(lines),
                    gvs::SetGeometryFormat(gvs::GeometryFormat::Lines),
                    gvs::SetReadableId("Base"),
                    gvs::SetColoring(gvs::Coloring::UniformColor),
                    gvs::SetShading(gvs::Shading::UniformColor),
                    gvs::SetUniformColor({0.6f, 0.6f, 0.6f}),
                    gvs::SetParent(parent));
}

} // namespace

DvhView2d::DvhView2d(gvs::ErrorAlertRecorder error_recorder)
    : error_recorder_(std::move(error_recorder)), dvh_({-1, -2}, {1, 0}) {

    scene_.add_item(gvs::SetReadableId("Axes"), gvs::SetPrimitive(gvs::Axes{}));
    indices_root_ = scene_.add_item(gvs::SetReadableId("Volume"), gvs::SetPositions3d());
    grid_root_    = scene_.add_item(gvs::SetReadableId("Grid"), gvs::SetPositions3d());

    build_grids(dvh_, &scene_, grid_root_);
}

DvhView2d::~DvhView2d() = default;

void DvhView2d::update() {

    for (auto const& [index, dir_and_dist] : dvh_.sparse_distance_field()) {

        if (!util::has_key(index_scene_ids_, index)) {
            auto xy = glm::vec2(index);
            index_scene_ids_.emplace(index,
                                     scene_.add_item(gvs::SetPositions3d({{xy.x, xy.y, 0.f},
                                                                          {xy.x + 1.f, xy.y, 0.f},
                                                                          {xy.x + 1.f, xy.y + 1.f, 0.f},
                                                                          {xy.x, xy.y + 1.f, 0.f}}),
                                                     gvs::SetGeometryFormat(gvs::GeometryFormat::TriangleFan),
                                                     gvs::SetReadableId(glm::to_string(index)),
                                                     gvs::SetParent(indices_root_),
                                                     gvs::SetColoring(gvs::Coloring::UniformColor),
                                                     gvs::SetShading(gvs::Shading::UniformColor),
                                                     gvs::SetUniformColor(gvs::default_uniform_color)));
        }

        auto color = (std::isinf(dir_and_dist.z) ? gvs::vec3{0.5f, 0.5f, 0.5f} : gvs::default_uniform_color);

        auto const& scene_id = index_scene_ids_.at(index);
        scene_.update_item(scene_id, gvs::SetUniformColor(color));
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
