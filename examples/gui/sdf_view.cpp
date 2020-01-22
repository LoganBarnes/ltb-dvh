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
#include "sdf_view.hpp"

// project
#include "ltb/gvs/core/log_params.hpp"
#include "ltb/gvs/display/gui/imgui_utils.hpp"
#include "ltb/gvs/display/gui/scene_gui.hpp"
#include "ltb/util/container_utils.hpp"

// external
#include <Magnum/GL/Context.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <imgui.h>

using namespace Magnum;
using namespace Platform;

namespace ltb::example {

SdfView::SdfView(gvs::ErrorAlertRecorder error_recorder) : error_recorder_(std::move(error_recorder)) {

    oriented_lines_ = {
        {{2.f, -2.f}, {2.f, 2.f}},
        {{2.f, 2.f}, {-1.f, 3.f}},
        {{-1.f, 3.f}, {-3.f, -1.f}},
    };

    scene_.add_item(gvs::SetReadableId("Axes"), gvs::SetPrimitive(gvs::Axes{}));
    geometry_root_scene_id_ = scene_.add_item(gvs::SetReadableId("Geometry"), gvs::SetPositions3d());

    std::vector<glm::vec3> lines;
    for (const auto& line : lines_) {
        lines.emplace_back(line.start);
        lines.emplace_back(line.end);
    }

    lines_scene_id_ = scene_.add_item(gvs::SetReadableId("Lines"),
                                      gvs::SetPositions3d(lines),
                                      gvs::SetGeometryFormat(gvs::GeometryFormat::Lines),
                                      gvs::SetColoring(gvs::Coloring::White),
                                      gvs::SetShading(gvs::Shading::UniformColor),
                                      gvs::SetParent(geometry_root_scene_id_));

    lines.clear();
    std::vector<glm::vec2> tex_coords;
    for (const auto& line : oriented_lines_) {
        lines.emplace_back(line.start.x, line.start.y, 0.f);
        lines.emplace_back(line.end.x, line.end.y, 0.f);
        tex_coords.emplace_back(0.f, 0.f);
        tex_coords.emplace_back(1.f, 1.f);
    }

    assert(lines.size() == tex_coords.size());
    oriented_lines_scene_id_ = scene_.add_item(gvs::SetReadableId("Oriented Lines"),
                                               gvs::SetPositions3d(lines),
                                               gvs::SetTextureCoordinates3d(tex_coords),
                                               gvs::SetGeometryFormat(gvs::GeometryFormat::Lines),
                                               gvs::SetColoring(gvs::Coloring::TextureCoordinates),
                                               gvs::SetShading(gvs::Shading::UniformColor),
                                               gvs::SetParent(geometry_root_scene_id_));

    tangent_sphere_scene_id_ = scene_.add_item(gvs::SetReadableId("Tangent Sphere"),
                                               gvs::SetPrimitive(gvs::Sphere{}),
                                               gvs::SetShading(gvs::Shading::Lambertian));
    update_tangent_sphere();
}

SdfView::~SdfView() = default;

void SdfView::render(const gvs::CameraPackage& camera_package) const {
    scene_.render(camera_package);
}

void SdfView::configure_gui() {
    gvs::configure_gui(&scene_);
}

void SdfView::resize(const Vector2i& viewport) {
    scene_.resize(viewport);
}

void SdfView::handleKeyReleaseEvent(Application::KeyEvent& event) {
    if (event.key() == Application::KeyEvent::Key::Esc) {
        event.setAccepted(true);
    }
}

auto SdfView::handleMousePressEvent(Application::MouseEvent& event) -> void {
    if (event.button() == Application::MouseEvent::Button::Left) {
        event.setAccepted(true);
    }
}

auto SdfView::handleMouseReleaseEvent(Application::MouseEvent& event) -> void {
    if (event.button() == Application::MouseEvent::Button::Left) {
        event.setAccepted(true);
    }
}

auto SdfView::handleMouseMoveEvent(Application::MouseMoveEvent& event) -> void {
    if (event.buttons() & Application::MouseMoveEvent::Button::Left) {
        event.setAccepted(true);
    }
}
void SdfView::update_tangent_sphere() {
    constexpr auto identity = glm::identity<glm::mat4>();

    auto transform = glm::translate(identity, tangent_sphere_center_)
        * glm::scale(identity, glm::vec3(std::abs(distance_to_closest_geometry_)));

    const auto* transform_data = glm::value_ptr(transform);
    gvs::mat4   tmp_conversion;

    for (auto i = 0u; i < 16u; ++i) {
        tmp_conversion[i] = transform_data[i];
    }

    auto color = (distance_to_closest_geometry_ < 0.f ? gvs::vec3{0.f, 1.f, 1.f} : gvs::vec3{1.f, 0.5f, 0.1f});

    scene_.update_item(tangent_sphere_scene_id_, gvs::SetTransformation(tmp_conversion), gvs::SetUniformColor(color));
}

} // namespace ltb::example
