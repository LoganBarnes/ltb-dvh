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
#include "main_window.hpp"

// project
#include "ltb/gvs/core/log_params.hpp"
#include "ltb/gvs/display/gui/error_alert.hpp"

// external
#include <Magnum/GL/Context.h>
#include <imgui.h>

static void initialize_dvh_resources() {
    CORRADE_RESOURCE_INITIALIZE(ltb_dvh_display_RESOURCES)
}

using namespace Magnum;

namespace ltb::example {

MainWindow::MainWindow(const Arguments& arguments)
    : gvs::ImGuiMagnumApplication(arguments,
                                  Configuration{}
                                      .setTitle("Tmp Particle Test")
                                      .setSize({1600, 900})
                                      .setWindowFlags(Configuration::WindowFlag::Resizable)),
      gl_version_str_(GL::Context::current().versionString()),
      gl_renderer_str_(GL::Context::current().rendererString()),
      error_alert_(std::make_shared<gvs::ErrorAlert>("DVH Errors")) {

    initialize_dvh_resources();

    camera_package_.zoom_object.translate({0.f, 0.f, 10.f});
    camera_package_.update_object();
}

MainWindow::~MainWindow() = default;

void MainWindow::update() {
    dvh_renderable_.update(0.01);
}

void MainWindow::render(const gvs::CameraPackage& camera_package) const {
    dvh_renderable_.render(camera_package);
}

void MainWindow::configure_gui() {

    auto add_three_line_separator = [] {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Separator();
        ImGui::Separator();
        ImGui::Spacing();
    };

    auto height = static_cast<float>(this->windowSize().y());

    ImGui::SetNextWindowPos({0.f, 0.f});
    ImGui::SetNextWindowSizeConstraints(ImVec2(0.f, height), ImVec2(std::numeric_limits<float>::infinity(), height));
    ImGui::Begin("Settings", nullptr, ImVec2(350.f, height));

    ImGui::Text("GL Version:   ");
    ImGui::SameLine();
    ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.f}, "%s\t", gl_version_str_.c_str());

    ImGui::Text("GL Renderer:  ");
    ImGui::SameLine();
    ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.f}, "%s\t", gl_renderer_str_.c_str());

    add_three_line_separator();

    dvh_renderable_.configure_gui();

    ImGui::End();

    error_alert_->display_next_error();
}

void MainWindow::resize(const Vector2i& /*viewport*/) {}

void MainWindow::handleKeyPressEvent(KeyEvent& /*event*/) {}

void MainWindow::handleKeyReleaseEvent(KeyEvent& /*event*/) {}

auto MainWindow::handleMousePressEvent(MouseEvent & /*event*/) -> void {}

auto MainWindow::handleMouseReleaseEvent(MouseEvent & /*event*/) -> void {}

auto MainWindow::handleMouseMoveEvent(MouseMoveEvent & /*event*/) -> void {}

} // namespace ltb::example
