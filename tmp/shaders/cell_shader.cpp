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
#include "cell_shader.hpp"

// external
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/Matrix4.h>

namespace ltb::dvh {

CellShader::CellShader() {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(Magnum::GL::Version::GL450);

    const Corrade::Utility::Resource rs{"dvh-resource-data"};

    Magnum::GL::Shader vert{Magnum::GL::Version::GL450, Magnum::GL::Shader::Type::Vertex};
    Magnum::GL::Shader frag{Magnum::GL::Version::GL450, Magnum::GL::Shader::Type::Fragment};

    vert.addSource(rs.get("cell_shader.vert"));
    frag.addSource(rs.get("cell_shader.frag"));

    auto vert_ref = Corrade::Containers::Reference<Magnum::GL::Shader>(vert);
    auto frag_ref = Corrade::Containers::Reference<Magnum::GL::Shader>(frag);

    CORRADE_INTERNAL_ASSERT_OUTPUT(Magnum::GL::Shader::compile({vert_ref, frag_ref}));

    attachShaders({vert, frag});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    projection_from_world_uniform_location_ = uniformLocation("projection_from_world");
    projection_from_view_uniform_location_  = uniformLocation("projection_from_view");
    viewport_height_uniform_location_       = uniformLocation("viewport_height");
    base_level_resolution_uniform_location_ = uniformLocation("base_level_resolution");

    coloring_uniform_location_      = uniformLocation("coloring_type");
    uniform_color_uniform_location_ = uniformLocation("uniform_color");

    shading_uniform_location_         = uniformLocation("shading_type");
    light_direction_uniform_location_ = uniformLocation("light_direction");
    light_color_uniform_location_     = uniformLocation("light_color");
    ambient_scale_uniform_location_   = uniformLocation("ambient_scale");
}

auto CellShader::set_projection_from_world_matrix(Magnum::Matrix4 const& projection_from_world) -> CellShader& {
    setUniform(projection_from_world_uniform_location_, projection_from_world);
    return *this;
}

auto CellShader::set_projection_from_view_matrix(Magnum::Matrix4 const& projection_from_view) -> CellShader& {
    setUniform(projection_from_view_uniform_location_, projection_from_view);
    return *this;
}

auto CellShader::set_viewport_height(int const& viewport_height) -> CellShader& {
    setUniform(viewport_height_uniform_location_, viewport_height);
    return *this;
}

auto CellShader::set_base_level_resolution(float const& base_level_resolution) -> CellShader& {
    setUniform(base_level_resolution_uniform_location_, base_level_resolution);
    return *this;
}

auto CellShader::set_coloring_type(CellColoring const& coloring) -> CellShader& {
    setUniform(coloring_uniform_location_, std::underlying_type_t<CellColoring>(coloring));
    return *this;
}

auto CellShader::set_uniform_color(Magnum::Color3 const& color) -> CellShader& {
    setUniform(uniform_color_uniform_location_, color);
    return *this;
}

auto CellShader::set_shading_type(gvs::Shading const& shading) -> CellShader& {
    setUniform(shading_uniform_location_, std::underlying_type_t<gvs::Shading>(shading));
    return *this;
}

} // namespace ltb::dvh
