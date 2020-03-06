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
#include "ltb/gvs/display/gui/imgui_utils.hpp"
#include "ltb/gvs/display/gui/scene_gui.hpp"
#include "obj_io.hpp"

// external
#include <Magnum/GL/Context.h>
#include <imgui.h>
#include <ltb/util/timer.hpp>

static void initialize_dvh_resources() {
    CORRADE_RESOURCE_INITIALIZE(ltb_dvh_display_RESOURCES)
}

using namespace Magnum;
using namespace Magnum::Math::Literals;

namespace ltb::example {

MainWindow::MainWindow(const Arguments& arguments)
    : gvs::ImGuiMagnumApplication(arguments,
                                  Configuration{}
                                      .setTitle("Tmp Particle Test")
                                      .setSize({1600, 900})
                                      .setWindowFlags(Configuration::WindowFlag::Resizable)),
      dvh_(0.1f),
      dvh_renderable_({this->windowSize().x(), this->windowSize().y()}) {

    initialize_dvh_resources();

    auto rotation = Matrix4::rotationY(30.0_degf) * Matrix4::rotationX(-15.0_degf);
    auto eye      = rotation.transformPoint({0.f, 0.f, 100.f});
    auto center   = Vector3{};
    auto up       = Vector3{0.f, 1.f, 0.f};

    arcball_camera_->setViewParameters(eye, center, up);
    arcball_camera_->update(true);

    camera_package_.camera->setProjectionMatrix(
        Magnum::Matrix4::perspectiveProjection(45.0_degf,
                                               Magnum::Vector2{this->windowSize()}.aspectRatio(),
                                               0.1f,
                                               5e4f));

    scene_.add_item(gvs::SetReadableId("Axes"), gvs::SetPrimitive(gvs::Axes{}));

    auto obj_file = std::string{"/home/logan/Documents/projects/engine/cache/dvh/hard-part-low-res.obj"};

    if (arguments.argc > 1) {
        obj_file = arguments.argv[1];
    }

    {
        ltb::util::ScopedTimer timer("Obj Loading", std::cout);
        mesh_ = io::load_obj(obj_file);

        scene_.add_item(gvs::SetReadableId("Mesh"),
                        gvs::SetPositions3d(mesh_.vertices),
                        gvs::SetIndices(mesh_.indices),
                        gvs::SetGeometryFormat(mesh_.geometry_format),
                        gvs::SetShading(gvs::Shading::Lambertian),
                        gvs::SetColoring(gvs::Coloring::UniformColor));
    }
}

MainWindow::~MainWindow() = default;

void MainWindow::update() {
    auto camera_pos = camera_package_.camera->cameraMatrix().inverted().transformPoint({0.f, 0.f, 0.f});
    dvh_renderable_.set_camera_position({camera_pos.x(), camera_pos.y(), camera_pos.z()});
    dvh_renderable_.update(0.01);

    if (!paused) {
        reset_draw_counter();
    }
}

void MainWindow::render(const gvs::CameraPackage& camera_package) const {
    scene_.render(camera_package);
    dvh_renderable_.render(camera_package);
}

void MainWindow::configure_gui() {
    auto height = static_cast<float>(this->windowSize().y());

    ImGui::SetNextWindowPos({0.f, 0.f});
    ImGui::SetNextWindowSizeConstraints(ImVec2(0.f, height), ImVec2(std::numeric_limits<float>::infinity(), height));
    ImGui::Begin("Settings", nullptr, ImVec2(350.f, height));

    if (!paused) {
        display_fps_info();
    }

    display_device_info();

    settings_.configure_gui();

    gvs::add_three_line_separator();

    if (ImGui::Button("Load Mesh")) {
        ltb::util::ScopedTimer timer("Obj Loading", std::cout);
    }

    gvs::add_three_line_separator();

    dvh_renderable_.configure_gui();
    gvs::configure_gui(&scene_);

    ImGui::End();

    error_alert_->display_next_error();
}

void MainWindow::resize(const Vector2i& viewport) {
    scene_.resize(viewport);
    dvh_renderable_.resize({viewport.x(), viewport.y()});
}

void MainWindow::handleKeyPressEvent(KeyEvent& /*event*/) {}

void MainWindow::handleKeyReleaseEvent(KeyEvent& event) {
    event.setAccepted(true);

    switch (event.key()) {

    case KeyEvent::Key::P:
        paused = !paused;
        if (!paused) {
            reset_draw_counter();
        }
        break;

    default:
        event.setAccepted(false);
        break;
    }
}

auto MainWindow::handleMousePressEvent(MouseEvent & /*event*/) -> void {}

auto MainWindow::handleMouseReleaseEvent(MouseEvent & /*event*/) -> void {}

auto MainWindow::handleMouseMoveEvent(MouseMoveEvent & /*event*/) -> void {}

} // namespace ltb::example
