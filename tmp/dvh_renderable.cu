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
#include "dvh_renderable.hpp"

// project
#include "buffer_map_guard.hpp"

// standard
#include <random>

namespace ltb {
namespace example {

namespace {

//// Cuda kernel to test the CUDA side of things
//__global__ void update_particles(gpu::device_buffer<Particle> particles, float timestep) {
//    uint idx = blockIdx.x * blockDim.x + threadIdx.x;
//
//    if (idx >= particles.length()) {
//        return;
//    }
//
//    // Apply euler update
//    auto& particle = particles[idx];
//    particle.position += particle.velocity * timestep;
//
//    // reverse velocity ("bounce") if we hit a 100x100x100 cube
//    for (int i = 0; i < 3; ++i) {
//        if (fabsf(particle.position[i]) > 50.f) {
//            particle.velocity[i] = -particle.velocity[i];
//        }
//    }
//}

} // namespace

DvhRenderable::DvhRenderable() {
    {
        std::vector<glm::vec4> particles(1000000, glm::vec4(1.f));
        interop_boxes_ = std::make_unique<cuda::GLBuffer<glm::vec4>>(particles);
    }

    mesh_.addVertexBuffer(interop_boxes_->gl_buffer(), 0, gvs::BoxShader::CenterAndRadius());
    mesh_.setCount(interop_boxes_->size());
    mesh_.setPrimitive(Magnum::GL::MeshPrimitive::Points);
}

void DvhRenderable::update(double /*time_step*/) {
    auto guard = cuda::make_gl_buffer_map_guard(*interop_boxes_);
    //    gpu::launch_default(update_particles,
    //                        interop_particles_->size(),
    //                        *interop_particles_,
    //                        static_cast<float>(time_step));
}

void DvhRenderable::render(const gvs::CameraPackage& camera_package) const {
    auto projection_from_world
        = camera_package.camera->projectionMatrix() * camera_package.object.transformationMatrix();

    shader_.set_projection_from_world_matrix(projection_from_world)
        .set_coloring(gvs::Coloring::Normals)
        .set_shading(gvs::Shading::UniformColor);

    mesh_.draw(shader_);
}

void DvhRenderable::configure_gui() {}

} // namespace example
} // namespace ltb
