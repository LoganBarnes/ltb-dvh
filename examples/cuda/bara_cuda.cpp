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
#include "bara_cuda.hpp"

// project
#include "ltb/gvs/display/gui/imgui_utils.hpp"

// external
#include <Magnum/GL/Context.h>
#include <imgui.h>

using namespace Magnum;

namespace ltb::example {

BaraCUDA::BaraCUDA(const Arguments& arguments)
    : gvs::ImGuiMagnumApplication(arguments,
                                  Configuration{}
                                      .setTitle("Distance Volume Hierarchy")
                                      .setSize({1280, 720})
                                      .setWindowFlags(Configuration::WindowFlag::Resizable)),
      gl_version_str_(GL::Context::current().versionString()),
      gl_renderer_str_(GL::Context::current().rendererString()) {}

BaraCUDA::~BaraCUDA() = default;

void BaraCUDA::update() {}

void BaraCUDA::render(const gvs::CameraPackage& /*camera_package*/) const {}

void BaraCUDA::configure_gui() {

    auto add_three_line_separator = [] {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Separator();
        ImGui::Separator();
        ImGui::Spacing();
    };

    auto width  = static_cast<float>(this->windowSize().x());
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

    ImGui::End();

    if (not error_message_.empty()) {
        ImGui::SetNextWindowPos({width * 0.5f, 0.f}, 0, {0.5f, 0.f});

        bool open = true;
        ImGui::Begin("Errors", &open, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
        ImGui::TextColored({1.f, 0.f, 0.f, 1.f}, "%s", error_message_.c_str());
        ImGui::End();

        if (not open) {
            error_message_ = "";
        }
    }
}

void BaraCUDA::resize(const Vector2i& /*viewport*/) {}

} // namespace ltb::example

auto main(int argc, char* argv[]) -> int {
    ltb::example::BaraCUDA app({argc, argv});
    return app.exec();
}
