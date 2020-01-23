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
#include "ltb/dvh/distance_volume_hierarchy.hpp"
#include "ltb/gvs/display/gui/error_alert_recorder.hpp"
#include "ltb/gvs/display/local_scene.hpp"
#include "ltb/sdf/sdf.hpp"
#include "view.hpp"

namespace ltb::example {

class DvhView2d : public View {
public:
    explicit DvhView2d(gvs::ErrorAlertRecorder error_recorder);
    ~DvhView2d() override;

    void update() override;
    void render(const gvs::CameraPackage& camera_package) const override;
    void configure_gui() override;

    void resize(const Magnum::Vector2i& viewport) override;

    void handleKeyPressEvent(Magnum::Platform::Application::KeyEvent& event) override;
    void handleKeyReleaseEvent(Magnum::Platform::Application::KeyEvent& event) override;

    void handleMousePressEvent(Magnum::Platform::Application::MouseEvent& event) override;
    void handleMouseReleaseEvent(Magnum::Platform::Application::MouseEvent& event) override;
    void handleMouseMoveEvent(Magnum::Platform::Application::MouseMoveEvent& event) override;

private:
    // Errors
    gvs::ErrorAlertRecorder error_recorder_;

    // DVH
    dvh::DistanceVolumeHierarchy<2> dvh_;

    // Additive Volumes
    std::vector<sdf::Geometry<sdf::Box, 2>> additive_boxes_;

    // Scene
    gvs::LocalScene                       scene_;
    gvs::SceneId                          dvh_root_scene_id_ = gvs::nil_id();
    std::unordered_map<int, gvs::SceneId> index_scene_ids_;
};

} // namespace ltb::example
