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
#include "ltb/gvs/display/gui/scene_gui.hpp"

// external
#include <Magnum/GL/Context.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <imgui.h>

using namespace Magnum;
using namespace Platform;

namespace ltb::example {

namespace {

// 'p' is a point on a plane
float intersect_plane(gvs::Ray const& ray, Vector3 p, const Vector3& plane_normal) {
    // plane ray intersection
    float const denom = Math::dot(ray.direction, plane_normal);

    p -= ray.origin;
    float const t = Math::dot(p, plane_normal) / denom;

    if (t >= 0.f && t != std::numeric_limits<float>::infinity()) {
        return t;
    }
    return std::numeric_limits<float>::infinity();
}

template <int L, typename G, typename F>
auto find_closest(float*                    closest_distance,
                  glm::vec<L, float> const& point,
                  std::vector<G> const&     geometries,
                  F const&                  sdf) {

    auto abs_closest_distance = std::abs(*closest_distance);

    for (const auto& geom : geometries) {
        auto dist     = sdf(point, geom);
        auto abs_dist = std::abs(dist);

        if (abs_dist < abs_closest_distance) {
            *closest_distance    = dist;
            abs_closest_distance = abs_dist;
        }
    }
}

} // namespace

SdfView::SdfView(gvs::OrbitCameraPackage& camera_package, gvs::ErrorAlertRecorder error_recorder)
    : camera_package_(camera_package), error_recorder_(std::move(error_recorder)) {

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

void SdfView::handleKeyPressEvent(Application::KeyEvent& event) {
    event.setAccepted(true);

    switch (event.key()) {
    case Application::KeyEvent::Key::LeftCtrl:
    case Application::KeyEvent::Key::RightCtrl:
        ctrl_down_ = true;
        break;

    default:
        event.setAccepted(false);
        break;
    }
}

void SdfView::handleKeyReleaseEvent(Application::KeyEvent& event) {
    event.setAccepted(true);

    switch (event.key()) {
    case Application::KeyEvent::Key::LeftCtrl:
    case Application::KeyEvent::Key::RightCtrl:
        ctrl_down_ = false;
        break;

    default:
        event.setAccepted(false);
        break;
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
    if (!event.buttons()) {
        event.setAccepted(true);

        if (ctrl_down_) {
            auto const orbit_point
                = camera_package_.translation_object.transformation().transformPoint(Magnum::Vector3{0.f});

            auto ray          = camera_package_.get_camera_ray_from_window_pos(Vector2(event.position()));
            auto plane_normal = (ray.origin - orbit_point).normalized();

            auto dist_to_plane = intersect_plane(ray, orbit_point, plane_normal);
            auto world_pos     = ray.origin + ray.direction * dist_to_plane;

            tangent_sphere_center_ = {world_pos.x(), world_pos.y(), world_pos.z()};

            distance_to_closest_geometry_ = std::numeric_limits<float>::infinity();

            find_closest(&distance_to_closest_geometry_, tangent_sphere_center_, lines_, sdf::distance_to_line<3>);
            find_closest(&distance_to_closest_geometry_,
                         glm::vec2(tangent_sphere_center_),
                         oriented_lines_,
                         sdf::distance_to_line<2>);

            // compare_geom(oriented_lines_, sdf::distance_to_oriented_line<2>);

            update_tangent_sphere();
        }
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
