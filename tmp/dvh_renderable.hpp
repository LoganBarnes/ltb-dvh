// ///////////////////////////////////////////////////////////////////////////////////////
//                                                                           |________|
//  Copyright (c) 2020 CloudNC Ltd - All Rights Reserved                        |  |
//                                                                              |__|
//        ____                                                                .  ||
//       / __ \                                                               .`~||$$$$
//      | /  \ \         /$$$$$$  /$$                           /$$ /$$   /$$  /$$$$$$$
//      \ \ \ \ \       /$$__  $$| $$                          | $$| $$$ | $$ /$$__  $$
//    / / /  \ \ \     | $$  \__/| $$  /$$$$$$  /$$   /$$  /$$$$$$$| $$$$| $$| $$  \__/
//   / / /    \ \__    | $$      | $$ /$$__  $$| $$  | $$ /$$__  $$| $$ $$ $$| $$
//  / / /      \__ \   | $$      | $$| $$  \ $$| $$  | $$| $$  | $$| $$  $$$$| $$
// | | / ________ \ \  | $$    $$| $$| $$  | $$| $$  | $$| $$  | $$| $$\  $$$| $$    $$
//  \ \_/ ________/ /  |  $$$$$$/| $$|  $$$$$$/|  $$$$$$/|  $$$$$$$| $$ \  $$|  $$$$$$/
//   \___/ ________/    \______/ |__/ \______/  \______/  \_______/|__/  \__/ \______/
//
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "gl_buffer.hpp"
#include "ltb/gvs/display/camera_package.hpp"
#include "ltb/gvs/display/shaders/points_shader.hpp"

// external
#include <glm/glm.hpp>

// standard
#include <memory>

namespace ltb {
namespace example {

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
};

/**
 * The particle update and render logic.
 */
class DvhRenderable {
public:
    DvhRenderable();

    void update(double time_step);
    void render(const gvs::CameraPackage& camera_package) const;
    void configure_gui();

    float global_particle_radius = 0.02f;
    int   viewport_height        = 1;

private:
    gvs::PointsShader shader_;

    // gl::StandardPipeline glpl_; ///< collection of OpenGL objects for rendering
    std::unique_ptr<cuda::GLBuffer<Particle>> interop_particles_; ///< GPU buffer shared by OpenGL and the CUDA
};

} // namespace example
} // namespace ltb
