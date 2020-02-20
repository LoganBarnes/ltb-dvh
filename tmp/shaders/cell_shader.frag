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

/*
 * Constants
 */
const float PI  = 3.141592653589793f;
const float INF = 1.f / 0.f;

const int COLORING_POSITIONS     = 0;
const int COLORING_NORMALS       = 1;
const int COLORING_LEVEL         = 2;
const int COLORING_DISTANCE      = 4;
const int COLORING_UNIFORM_COLOR = 5;
const int COLORING_WHITE         = 6;

const int SHADING_UNIFORM_COLOR = 0;
const int SHADING_LAMBERTIAN    = 1;
const int SHADING_COOK_TORRANCE = 2;

/*
 * Inputs
 */
layout(location = 0) in vec3  world_position;
layout(location = 1) in vec3  world_normal;
layout(location = 2) in float level_shading;
layout(location = 3) in float distance_shading;

/*
 * Uniforms
 */
uniform int   coloring_type = COLORING_UNIFORM_COLOR;
uniform vec3  uniform_color = { 1.f, 0.9f, 0.7f };

uniform int   shading_type    = SHADING_UNIFORM_COLOR;
uniform vec3  light_direction = { -1.f, -2.f, -3.f };
uniform vec3  light_color     = { +1.f, +1.f, +1.f };
uniform float ambient_scale   = 0.15f;

layout(location = 0) out vec4 out_color;

float guassian(in vec2 xy, in float omega) {
    float two_omega_pow_2 = 2.f * omega * omega;
    float power = -dot(xy, xy) / two_omega_pow_2;

    return (1.f / (two_omega_pow_2 * PI)) * exp(power);
}

void main()
{
    vec2 point_space = gl_PointCoord * vec2(2.f, -2.f) + vec2(-1.f, 1.f);// [-1.f, +1.f]

    // if (dot(point_space, point_space) > 1.f) {
    //     discard; // kill pixels outside circle
    // }

    vec2 gauss_space = point_space * 4.f;

    float max_guass = guassian(vec2(0.f, 0.f), 1.f);;
    float opacity   = guassian(gauss_space, 1.f) / max_guass;

    vec3 shape_color = { 1.f, 1.f, 1.f };

    switch (coloring_type) {
        case COLORING_POSITIONS:
        shape_color = world_position;
        break;

        case COLORING_NORMALS:
        shape_color = world_normal * 0.5f + 0.5f;// between 0 and 1
        break;

        case COLORING_LEVEL:
        shape_color = uniform_color * level_shading;
        break;

        case COLORING_DISTANCE:
        shape_color = uniform_color * distance_shading;
        break;

        case COLORING_UNIFORM_COLOR:
        shape_color = uniform_color;
        break;

        case COLORING_WHITE:
        break;

    }// end switch(coloring_type)

    vec3 final_color = { 1.f, 1.f, 1.f };

    vec3 surface_normal = normalize(world_normal);
    vec3 direction_to_light = normalize(-light_direction);

    switch (shading_type) {
        case SHADING_UNIFORM_COLOR:
        final_color = shape_color;
        break;

        case SHADING_COOK_TORRANCE:// TODO
        case SHADING_LAMBERTIAN:
        vec3 diffuse_lighting = max(0.f, dot(surface_normal, direction_to_light)) * light_color;
        final_color = diffuse_lighting * shape_color + ambient_scale * shape_color;
        break;

    }// end switch(shading_type)

    out_color = vec4(final_color, opacity);
}
