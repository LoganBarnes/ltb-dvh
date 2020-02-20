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

// project
#include "gl/opengl/opengl.h"

#include "renderable_dvh.hpp"

#include "loop/opengl_simulation.h"
#include "loop/scene/scene_params.h"
#include "loop/scene/scene_util.h"
#include "loop/scene/static_scene.h"

/**
 * The main simulation class and scene
 */
class ExampleSim {
public:
    ExampleSim(int w, int h, loop::SimData& sim_data)
        : sim_data_{sim_data}, dvh_renderable_(std::make_shared<RenderableDvh>(num::ivec2(w, h))) {

        loop::SceneUtil::set_default_lighting(scene_);
        loop::SceneUtil::add_axes_at_origin(scene_);

        scene_.add_item(loop::SetRenderable(dvh_renderable_), loop::SetReadableID("Particles"));

        // offset the camera so when we drag the mouse we orbit around the origin
        sim_data_.camera3d_mover.set_origin_offset(150);
        sim_data_.get_camera3d().set_use_perspective(true);
    }

    /**
     * Calls a cuda kernel to update the particle positions
     */
    void on_update(double, double time_step) {
        dvh_renderable_->set_camera_pos(sim_data_.get_current_camera().get_eye_pos());
        dvh_renderable_->update(time_step);
    }

    /**
     * Renders the particles
     */
    void on_render(int, int viewport_height, float) const {
        auto& camera = sim_data_.get_current_camera();
        scene_.render(camera);
    }

    /**
     * Displays UI rendering options
     */
    void on_gui_render(int viewport_width, int viewport_height) {
        ImGui::Text("Approx FPS: %.2f", ImGui::GetIO().Framerate);

        scene_.configure_gui();

        if (ImGui::CollapsingHeader("Controls", ImGuiTreeNodeFlags_DefaultOpen)) {
            std::stringstream msg;
            msg << "Exit - Ctrl + Shift + Q\n";
            msg << "Camera:\n";
            msg << "\tRotate - Left mouse drag\n";
            msg << "\tPan - Middle mouse drag\n";
            msg << "\tZoom - Right mouse drag/Mouse scroll\n";
            ImGui::Text("%s", msg.str().c_str());
        }
    }

    void handle_framebuffer_size(GLFWwindow* /*window*/, int width, int height) {
        auto viewport = num::ivec2(width, height);
        scene_.resize(viewport);
        dvh_renderable_->resize(viewport);
    }

private:
    loop::SimData&                 sim_data_; ///< publicly available simulation data
    loop::StaticScene              scene_; ///< Scene to handle rendering
    std::shared_ptr<RenderableDvh> dvh_renderable_; ///< The OpenGL/CUDA particles
};

int main() {
    loop::OpenGLSimulation<ExampleSim> sim({"Example", 1600, 900, true, 4});
    sim.run_as_fast_as_possible_loop();
    return 0;
}
