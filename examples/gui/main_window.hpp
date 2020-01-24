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
#include "dvh_view_2d.hpp"
#include "dvh_view_3d.hpp"
#include "ltb/gvs/display/gui/imgui_magnum_application.hpp"
#include "sdf_view.hpp"

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

    // General Info
    std::string gl_version_str_;
    std::string gl_renderer_str_;

    // Errors
    std::shared_ptr<gvs::ErrorAlert> error_alert_;

    // Views
    SdfView   sdf_view_;
    DvhView3d dvh_view_3d_;
    DvhView2d dvh_view_2d_;

    View* current_view_ = &dvh_view_2d_;
};

} // namespace ltb::example
