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
#include "ltb/gvs/display/gui/error_alert.hpp"
#include "ltb/gvs/display/gui/imgui_magnum_application.hpp"
#include "ltb/gvs/display/local_scene.hpp"

namespace ltb::example {

class View {
public:
    virtual ~View() = default;

    // updates
    virtual auto update() -> void{};
    virtual auto render(gvs::CameraPackage const& /*camera_package*/) const -> void{};
    virtual auto configure_gui() -> void{};

    virtual auto resize(const Magnum::Vector2i & /*viewport*/) -> void{};

    // keyboard input
    virtual auto handleKeyPressEvent(Magnum::Platform::Application::KeyEvent & /*event*/) -> void {}
    virtual auto handleKeyReleaseEvent(Magnum::Platform::Application::KeyEvent & /*event*/) -> void {}
    virtual auto handleTextInputEvent(Magnum::Platform::Application::TextInputEvent & /*event*/) -> void {}

    // mouse input
    virtual auto handleMousePressEvent(Magnum::Platform::Application::MouseEvent & /*event*/) -> void {}
    virtual auto handleMouseReleaseEvent(Magnum::Platform::Application::MouseEvent & /*event*/) -> void {}
    virtual auto handleMouseMoveEvent(Magnum::Platform::Application::MouseMoveEvent & /*event*/) -> void {}
    virtual auto handleMouseScrollEvent(Magnum::Platform::Application::MouseScrollEvent & /*event*/) -> void {}
};

} // namespace ltb::example
