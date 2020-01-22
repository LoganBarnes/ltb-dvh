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
#include "ltb/gvs/display/gui/imgui_utils.hpp"
#include "ltb/gvs/display/gui/scene_gui.hpp"
#include "ltb/util/container_utils.hpp"

// external
#include <Magnum/GL/Context.h>
#include <glm/gtx/string_cast.hpp>
#include <imgui.h>

using namespace Magnum;
using namespace Platform;

namespace ltb::example {

SdfView::SdfView(gvs::ErrorAlertRecorder error_recorder) : error_recorder_(std::move(error_recorder)) {

    scene_.add_item(gvs::SetReadableId("Axes"), gvs::SetPrimitive(gvs::Axes{}));
    geometry_root_scene_id_ = scene_.add_item(gvs::SetReadableId("Geometry"), gvs::SetPositions3d());
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

void SdfView::handleKeyReleaseEvent(Application::KeyEvent& event) {
    if (event.key() == Application::KeyEvent::Key::Esc) {
        event.setAccepted(true);
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
    if (event.buttons() & Application::MouseMoveEvent::Button::Left) {
        event.setAccepted(true);
    }
}

} // namespace ltb::example
