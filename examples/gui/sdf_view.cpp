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
#include "scene_helpers.hpp"

// external
#include <Magnum/GL/Context.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <imgui.h>

// standard
#include <utility>

using namespace Magnum;
using namespace Platform;

//#define THREE_DEE

namespace ltb::example {

namespace {

//// 'p' is a point on a plane
//float intersect_plane(gvs::Ray const& ray, Vector3 p, const Vector3& plane_normal)
//{
//    // plane ray intersection
//    float const denom = Math::dot(ray.direction, plane_normal);
//
//    p -= ray.origin;
//    float const t = Math::dot(p, plane_normal) / denom;
//
//    if (t >= 0.f && t != std::numeric_limits<float>::infinity()) {
//        return t;
//    }
//    return std::numeric_limits<float>::infinity();
//}

template <int L, typename G>
auto find_closest(float* closest_distance, glm::vec<L, float> const& point, std::vector<G> const& geometries) {

    auto abs_closest_distance = std::abs(*closest_distance);

    for (const auto& geometry : geometries) {
        auto dist     = geometry.distance_from(point);
        auto abs_dist = std::abs(dist);

        if (abs_dist < abs_closest_distance) {
            *closest_distance    = dist;
            abs_closest_distance = abs_dist;
        }
    }
}

template <typename T>
auto to_vec3(glm::vec<2, T> const& input) -> glm::vec<3, T> {
    return {input, T(0)};
}

template <typename T>
auto to_vec3(glm::vec<3, T> const& input) -> glm::vec<3, T> {
    return input;
}

template <int L, typename G>
auto find_closest(glm::vec<3, float>* closest_vec, glm::vec<L, float> const& point, std::vector<G> const& geometries) {

    auto abs_closest_distance = glm::length(*closest_vec);

    for (const auto& geometry : geometries) {
        auto vec_to_geom = geometry.vector_from(point);
        auto abs_dist    = glm::length(vec_to_geom);

        if (abs_dist < abs_closest_distance) {
            *closest_vec         = to_vec3<float>(vec_to_geom);
            abs_closest_distance = abs_dist;
        }
    }
}

} // namespace

SdfView::SdfView(gvs::CameraPackage& /*camera_package*/, gvs::ErrorAlertRecorder error_recorder)
    : /*camera_package_(camera_package),*/ error_recorder_(std::move(error_recorder)) {

    oriented_lines_ = {
#ifndef THREE_DEE
        sdf::make_oriented_line({2.f, -2.f}, {2.f, 2.f}),
        sdf::make_oriented_line({2.f, 2.f}, {-1.f, 3.f}),
        sdf::make_oriented_line({-1.f, 3.f}, {-3.f, -1.f}),
#endif
    };

    squares_ = {
#ifndef THREE_DEE
        sdf::make_transformed_geometry(sdf::make_box<2>({2.4f, 1.3f}), {0.5f, -2.f}),
        sdf::make_transformed_geometry(sdf::make_box<2>({1.2f, 4.f}), {-3.f, 0.5f}),
#endif
    };

    boxes_ = {
#ifdef THREE_DEE
        sdf::make_transformed_geometry(sdf::make_box<3>({2.4f, 1.3f, 0.9f}), {0.5f, -2.f, 0.f}),
        sdf::make_transformed_geometry(sdf::make_box<3>({1.2f, 4.f, 3.1f}), {-3.f, 0.5f, 1.f}),
#endif
    };

    scene_.add_item(gvs::SetReadableId("Axes"), gvs::SetPrimitive(gvs::Axes{}));
    geometry_root_scene_id_ = scene_.add_item(gvs::SetReadableId("Geometry"), gvs::SetPositions3d());

    std::vector<glm::vec3> lines;
    for (const auto& line : lines_) {
        lines.emplace_back(line.start);
        lines.emplace_back(line.end);
    }

    scene_.add_item(gvs::SetReadableId("Lines"),
                    gvs::SetPositions3d(lines),
                    gvs::SetGeometryFormat(gvs::GeometryFormat::Lines),
                    gvs::SetColoring(gvs::Coloring::White),
                    gvs::SetShading(gvs::Shading::UniformColor),
                    gvs::SetParent(geometry_root_scene_id_));

    auto oriented_lines_scene_id = add_lines_to_scene(&scene_, oriented_lines_, geometry_root_scene_id_);
    scene_.update_item(oriented_lines_scene_id, gvs::SetReadableId("Oriented Lines"));

    auto squares_scene_id = add_boxes_to_scene(&scene_, squares_, geometry_root_scene_id_);
    scene_.update_item(squares_scene_id,
                       gvs::SetReadableId("Squares"),
                       gvs::SetColoring(gvs::Coloring::UniformColor),
                       gvs::SetShading(gvs::Shading::UniformColor));

    auto boxes_scene_id = add_boxes_to_scene(&scene_, boxes_, geometry_root_scene_id_);
    scene_.update_item(boxes_scene_id,
                       gvs::SetReadableId("Boxes"),
                       gvs::SetColoring(gvs::Coloring::UniformColor),
                       gvs::SetShading(gvs::Shading::Lambertian));

    // SDF Feedback
    tangent_sphere_scene_id_ = scene_.add_item(gvs::SetReadableId("Tangent Sphere"),
                                               gvs::SetPrimitive(gvs::Sphere{}),
                                               gvs::SetShading(gvs::Shading::Lambertian));

    sdf_line_scene_id_ = scene_.add_item(gvs::SetReadableId("Tangent Sphere"),
                                         gvs::SetPositions3d(),
                                         gvs::SetGeometryFormat(gvs::GeometryFormat::Lines),
                                         gvs::SetColoring(gvs::Coloring::TextureCoordinates),
                                         gvs::SetShading(gvs::Shading::UniformColor));
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

    case Application::KeyEvent::Key::LeftShift:
    case Application::KeyEvent::Key::RightShift:
        shift_down_ = true;
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

    case Application::KeyEvent::Key::LeftShift:
    case Application::KeyEvent::Key::RightShift:
        shift_down_ = false;
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

        if (ctrl_down_ || shift_down_) {
            //            auto const orbit_point
            //                = camera_package_.translation_object.transformation().transformPoint(Magnum::Vector3{0.f});
            //
            //            auto ray          = camera_package_.get_camera_ray_from_window_pos(Vector2(event.position()));
            //            auto plane_normal = (ray.origin - orbit_point).normalized();
            //
            //            auto dist_to_plane = intersect_plane(ray, orbit_point, plane_normal);
            //            auto world_pos     = ray.origin + ray.direction * dist_to_plane;
            //
            //            tangent_sphere_center_ = {world_pos.x(), world_pos.y(), world_pos.z()};

            distance_to_closest_geometry_ = std::numeric_limits<float>::infinity();
        }

        auto tangent_sphere_center_2d = glm::vec2(tangent_sphere_center_);

        if (ctrl_down_) {
            find_closest(&distance_to_closest_geometry_, tangent_sphere_center_, lines_);
            find_closest(&distance_to_closest_geometry_, tangent_sphere_center_2d, oriented_lines_);
            find_closest(&distance_to_closest_geometry_, tangent_sphere_center_2d, squares_);
            find_closest(&distance_to_closest_geometry_, tangent_sphere_center_, boxes_);

            update_tangent_sphere();
        }

        if (shift_down_) {
            auto closest = glm::vec3(std::numeric_limits<float>::infinity());

            find_closest(&closest, tangent_sphere_center_, lines_);
            find_closest(&closest, tangent_sphere_center_2d, oriented_lines_);
            find_closest(&closest, tangent_sphere_center_2d, squares_);
            find_closest(&closest, tangent_sphere_center_, boxes_);

            line_to_closest_geometry_.start = tangent_sphere_center_;
            line_to_closest_geometry_.end   = tangent_sphere_center_ + closest;

            update_sdf_line();
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

    scene_.update_item(tangent_sphere_scene_id_,
                       gvs::SetVisible(true),
                       gvs::SetTransformation(tmp_conversion),
                       gvs::SetUniformColor(color));

    scene_.update_item(sdf_line_scene_id_, gvs::SetVisible(false));
}

void SdfView::update_sdf_line() {

    auto color = (distance_to_closest_geometry_ < 0.f ? gvs::vec3{0.f, 1.f, 1.f} : gvs::vec3{1.f, 0.5f, 0.1f});

    scene_.update_item(sdf_line_scene_id_,
                       gvs::SetVisible(true),
                       gvs::SetPositions3d({line_to_closest_geometry_.start, line_to_closest_geometry_.end}),
                       gvs::SetTextureCoordinates3d({{0.f, 0.f}, {0.f, 1.f}}),
                       gvs::SetUniformColor(color));

    scene_.update_item(tangent_sphere_scene_id_, gvs::SetVisible(false));
}

} // namespace ltb::example
