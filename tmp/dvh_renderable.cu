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
    // create 1000000 random particles within a 70x70x70 cube
    {
        std::mt19937                          gen(std::random_device{}());
        std::uniform_real_distribution<float> dist(-35.f, 35.f);
        std::vector<Particle>                 particles(1000000);

        for (auto& particle : particles) {
            particle = {{dist(gen), dist(gen), dist(gen)}, glm::normalize(glm::vec3(dist(gen), dist(gen), dist(gen)))};
        }

        interop_particles_ = std::make_unique<cuda::GLBuffer<Particle>>(particles);
    }

    glEnable(GL_PROGRAM_POINT_SIZE);
    //    glpl_.program = std::make_shared<gl::Program>(gl::points_vert(), gl::points_frag());
    //    glpl_.vao     = std::make_shared<gl::VertexArray>(*glpl_.program,
    //                                                  interop_particles_->gl_buffer(),
    //                                                  sizeof(Particle),
    //                                                  gl::pos_float_vao_elements());
}

void DvhRenderable::update(double time_step) {
    auto guard = cuda::make_gl_buffer_map_guard(*interop_particles_);
    //    gpu::launch_default(update_particles,
    //                        interop_particles_->size(),
    //                        *interop_particles_,
    //                        static_cast<float>(time_step));
}

void DvhRenderable::render(const gvs::CameraPackage& /*camera_package*/) const {
    // OpenGL rendering to test the OpenGL side of things
    //    glpl_.program->use([&] {
    //        glpl_.program->set_uniform(camera.get_projection_view_matrix(), "world_from_local");
    //        glpl_.program->set_uniform(viewport_height, "view_height");
    //        glpl_.program->set_uniform(camera.get_projection_matrix(), "projection_from_view");
    //        glpl_.program->set_uniform(global_particle_radius, "point_radius");
    //        glpl_.program->set_uniform(camera.get_view_matrix(), "view_from_world");
    //
    //        glpl_.vao->draw(GL_POINTS, 0, static_cast<int>(interop_particles_->size()));
    //    });
}

void DvhRenderable::configure_gui() {}

} // namespace example
} // namespace ltb
