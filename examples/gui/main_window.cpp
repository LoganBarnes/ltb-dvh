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
#include "ltb/gvs/display/gui/imgui_utils.hpp"
#include "ltb/gvs/display/gui/scene_gui.hpp"
#include "ltb/util/container_utils.hpp"

// external
#include <Magnum/GL/Context.h>
#include <glm/gtx/string_cast.hpp>
#include <imgui.h>

using namespace Magnum;

namespace ltb::example {

MainWindow::MainWindow(const Arguments& arguments)
    : gvs::ImGuiMagnumApplication(arguments,
                                  Configuration{}
                                      .setTitle("Distance Volume Hierarchy")
                                      .setSize({1280, 720})
                                      .setWindowFlags(Configuration::WindowFlag::Resizable)),
      gl_version_str_(GL::Context::current().versionString()),
      gl_renderer_str_(GL::Context::current().rendererString()),
      error_alert_("DVH Errors") {

    camera_package_.zoom_object.translate({0.f, 0.f, 5.f});
    camera_package_.update_object();

    scene_.add_item(gvs::SetReadableId("Axes"), gvs::SetPrimitive(gvs::Axes{}));

    dvh_.add_node({-1, -1});
    dvh_.add_node({0, -1});
    dvh_.add_node({-1, 0});
    dvh_.add_node({0, 0});
}

MainWindow::~MainWindow() = default;

void MainWindow::update() {

    for (auto const& [index, dir_and_dist] : dvh_.nodes()) {

        if (!util::has_key(node_ids_, index)) {
            auto xy = glm::vec2(index);
            node_ids_.emplace(index,
                              scene_.add_item(gvs::SetPositions3d({
                                                  {xy.x, xy.y, 0.f},
                                                  {xy.x + 1.f, xy.y, 0.f},
                                                  {xy.x + 1.f, xy.y + 1.f, 0.f},
                                                  {xy.x, xy.y + 1.f, 0.f},
                                                  {xy.x, xy.y, 0.f},
                                              }),
                                              gvs::SetGeometryFormat(gvs::GeometryFormat::LineStrip),
                                              gvs::SetReadableId(glm::to_string(index)),
                                              gvs::SetParent(node_root_),
                                              gvs::SetColoring(gvs::Coloring::UniformColor),
                                              gvs::SetShading(gvs::Shading::UniformColor)));
        }

        auto color = (std::isinf(dir_and_dist.z) ? gvs::vec3{0.5f, 0.5f, 0.5f} : gvs::default_uniform_color);

        auto const& scene_id = node_ids_.at(index);
        scene_.update_item(scene_id, gvs::SetUniformColor(color));
    }
}

void MainWindow::render(const gvs::CameraPackage& camera_package) const {
    scene_.render(camera_package);
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

    gvs::configure_gui(&scene_);

    ImGui::End();

    error_alert_.display_next_error();
}

void MainWindow::resize(const Vector2i& viewport) {
    scene_.resize(viewport);
}

} // namespace ltb::example
