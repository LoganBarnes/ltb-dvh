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
#include "ltb/gvs/display/gui/error_alert_recorder.hpp"
#include "ltb/gvs/display/local_scene.hpp"
#include "ltb/sdf/sdf.hpp"
#include "view.hpp"

namespace ltb::example {

class SdfView : public View {
public:
    explicit SdfView(gvs::OrbitCameraPackage& camera_package, gvs::ErrorAlertRecorder error_recorder);
    ~SdfView() override;

    void render(const gvs::CameraPackage& camera_package) const override;
    void configure_gui() override;

    void resize(const Magnum::Vector2i& viewport) override;

    void handleKeyPressEvent(Magnum::Platform::Application::KeyEvent& event) override;
    void handleKeyReleaseEvent(Magnum::Platform::Application::KeyEvent& event) override;

    void handleMousePressEvent(Magnum::Platform::Application::MouseEvent& event) override;
    void handleMouseReleaseEvent(Magnum::Platform::Application::MouseEvent& event) override;
    void handleMouseMoveEvent(Magnum::Platform::Application::MouseMoveEvent& event) override;

private:
    // Camera
    gvs::OrbitCameraPackage& camera_package_;

    // Errors
    gvs::ErrorAlertRecorder error_recorder_;

    // Geometry
    std::vector<sdf::Line<3>>               lines_;
    std::vector<sdf::OrientedLine<>>        oriented_lines_;
    std::vector<sdf::Geometry<sdf::Box, 2>> squares_;

    // Scene
    gvs::LocalScene scene_;
    gvs::SceneId    geometry_root_scene_id_ = gvs::nil_id();

    // Interaction
    glm::vec3 tangent_sphere_center_ = glm::vec3(0.f);

    float        distance_to_closest_geometry_ = std::numeric_limits<float>::infinity();
    gvs::SceneId tangent_sphere_scene_id_      = gvs::nil_id();

    sdf::Line<3> line_to_closest_geometry_{};
    gvs::SceneId sdf_line_scene_id_ = gvs::nil_id();

    // Inputs
    bool ctrl_down_  = false;
    bool shift_down_ = false;

    void update_tangent_sphere();
    void update_sdf_line();
};

} // namespace ltb::example
