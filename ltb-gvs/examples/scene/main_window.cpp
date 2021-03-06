// ///////////////////////////////////////////////////////////////////////////////////////
// LTB Geometry Visualization Server
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
#include "../common/test_scene.hpp"
#include "ltb/gvs/core/log_params.hpp"
#include "ltb/gvs/display/gui/imgui_utils.hpp"
#include "ltb/gvs/display/gui/scene_gui.hpp"

// external
#include <Magnum/GL/Context.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Trade/MeshData3D.h>
#include <imgui.h>

// standard
#include <random>

using namespace Magnum;
using namespace Math::Literals;

namespace ltb::example {

MainWindow::MainWindow(const Arguments& arguments)
    : gvs::ImGuiMagnumApplication(arguments,
                                  Configuration{}
                                      .setTitle("Machine Emulator")
                                      .setSize({1280, 720})
                                      .setWindowFlags(Configuration::WindowFlag::Resizable)) {

    auto rotation = Matrix4::rotationY(30.0_degf) * Matrix4::rotationX(-15.0_degf);
    auto eye      = rotation.transformPoint({0.f, 0.f, 20.f});
    auto center   = Vector3{};
    auto up       = Vector3{0.f, 1.f, 0.f};

    arcball_camera_->setViewParameters(eye, center, up);
    arcball_camera_->update(true);

    scene_.add_item(gvs::SetReadableId("Axes"), gvs::SetPrimitive(gvs::Axes{}));

    {
        Trade::MeshData3D const cube = Primitives::cubeSolid();

        auto scale_transfomation = gvs::mat4();
        scale_transfomation[0]   = 0.5f;
        scale_transfomation[5]   = 0.5f;
        scale_transfomation[10]  = 0.5f;
        scale_transfomation[15]  = 1.f;

        scene_.add_item(gvs::SetReadableId("Cube"),
                        gvs::SetPositions3d(cube.positions(0)),
                        gvs::SetNormals3d(cube.normals(0)),
                        gvs::SetTriangles(cube.indices()),
                        gvs::SetTransformation(scale_transfomation),
                        gvs::SetShading(gvs::Shading::Lambertian));
    }

    std::vector<gvs::vec3> sphere;

    {
        std::mt19937                          gen{std::random_device{}()};
        std::uniform_real_distribution<float> u_dist(-1.f, 1.f);
        std::uniform_real_distribution<float> theta_dist(0.f, 2.f * pi);

        float u;
        float theta;
        float coeff;

        for (auto i = 0u; i < 5000u; ++i) {
            u     = u_dist(gen);
            theta = theta_dist(gen);
            coeff = std::sqrt(1.f - u * u);

            sphere.push_back({coeff * std::cos(theta), coeff * std::sin(theta), u});
        }
    }

    {
        auto scale_transfomation = gvs::mat4();
        scale_transfomation[0]   = 2.f;
        scale_transfomation[5]   = 2.f;
        scale_transfomation[10]  = 2.f;
        scale_transfomation[15]  = 1.f;

        scene_.add_item(gvs::SetReadableId("Sphere"),
                        gvs::SetPositions3d(sphere),
                        gvs::SetNormals3d(sphere),
                        gvs::SetTransformation(scale_transfomation),
                        gvs::SetShading(gvs::Shading::Lambertian),
                        gvs::SetColoring(gvs::Coloring::Normals));
    }

    auto test_scene_root = scene_.add_item(gvs::SetReadableId("Test Scene"), gvs::SetVisible(false));
    build_test_scene(&scene_, test_scene_root);
}

MainWindow::~MainWindow() = default;

void MainWindow::update() {
    // Nothing to do
}

void MainWindow::render(const gvs::CameraPackage& camera_package) const {
    scene_.render(camera_package);
}

void MainWindow::configure_gui() {

    auto height = static_cast<float>(this->windowSize().y());
    ImGui::SetNextWindowPos({0.f, 0.f});
    ImGui::SetNextWindowSizeConstraints({0.f, height}, {std::numeric_limits<float>::infinity(), height});
    ImGui::Begin("Settings", nullptr, {350.f, height});

    display_device_info();

    settings_.configure_gui();

    gvs::add_three_line_separator();

    gvs::configure_gui(&scene_);

    ImGui::End();

    error_alert_.display_next_error();
}

void MainWindow::resize(const Vector2i& viewport) {
    scene_.resize(viewport);
}

} // namespace ltb::example

auto main(int argc, char* argv[]) -> int {
    ltb::example::MainWindow app({argc, argv});
    return app.exec();
}
