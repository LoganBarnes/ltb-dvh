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
#include "dvh_renderable.hpp"
#include "ltb/dvh/impl/distance_volume_hierarchy_gpu.hpp"
#include "ltb/gvs/display/gui/error_alert.hpp"
#include "ltb/gvs/display/gui/imgui_magnum_application.hpp"
#include "ltb/gvs/display/local_scene.hpp"
#include "ltb/sdf/sdf.hpp"

namespace ltb::example {

class MainWindow : public gvs::ImGuiMagnumApplication {
public:
    explicit MainWindow(const Arguments& arguments);
    ~MainWindow() override;

private:
    void update() override;
    void render(const gvs::CameraPackage& camera_package) const override;
    void configure_gui() override;

    void resize(const Magnum::Vector2i& viewport) override;

    void handleKeyPressEvent(KeyEvent& event) override;
    void handleKeyReleaseEvent(KeyEvent& event) override;

    void handleMousePressEvent(MouseEvent& event) override;
    void handleMouseReleaseEvent(MouseEvent& event) override;
    void handleMouseMoveEvent(MouseMoveEvent& event) override;

    // DVH
    float                           base_resolution_ = 1.0f;
    dvh::DistanceVolumeHierarchy<3> dvh_;

    // Additive Volumes
    std::vector<sdf::OrientedTriangle<>> additive_mesh_;

    // Subtractive Volumes
    std::vector<sdf::OffsetLine<3>> subtractive_lines_;

    // Visuals
    DvhRenderable   dvh_renderable_;
    gvs::LocalScene scene_;

    std::stringstream timing_stream_;

    bool paused = true;
};

} // namespace ltb::example
