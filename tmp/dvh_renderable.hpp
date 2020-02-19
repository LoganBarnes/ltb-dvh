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
#include "Magnum/GL/Mesh.h"
#include "gl_buffer.hpp"
#include "ltb/gvs/display/camera_package.hpp"
#include "ltb/gvs/display/shaders/box_shader.hpp"

// external
#include <glm/glm.hpp>

// standard
#include <memory>

namespace ltb {
namespace example {

/**
 * The particle update and render logic.
 */
class DvhRenderable {
public:
    DvhRenderable();

    void update(double time_step);
    void render(const gvs::CameraPackage& camera_package) const;
    void configure_gui();

private:
    mutable gvs::BoxShader   shader_;
    mutable Magnum::GL::Mesh mesh_;

    std::unique_ptr<cuda::GLBuffer<glm::vec4>> interop_boxes_; ///< GPU buffer shared by OpenGL and the CUDA
};

} // namespace example
} // namespace ltb
