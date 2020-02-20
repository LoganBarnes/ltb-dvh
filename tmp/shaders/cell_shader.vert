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

layout(location = 0) in ivec3 index;
layout(location = 1) in vec3  vector_to_closest_point;
layout(location = 2) in int   level;

//layout(location = 0) in vec4 center_and_radius;

uniform mat4  projection_from_world;
uniform mat4  projection_from_view;
uniform int   viewport_height;
uniform float base_level_resolution;

layout(location = 0) out vec3  world_position;
layout(location = 1) out vec3  world_normal;
layout(location = 2) out float level_shading;
layout(location = 3) out float distance_shading;

out gl_PerVertex
{
    vec4  gl_Position;
    float gl_PointSize;
};

void main()
{
    float resolution                = base_level_resolution * pow(2.f, float(level));
    float distance_to_closest_point = length(vector_to_closest_point);

    world_position   = vec3(index) * resolution;
    world_normal     = normalize(vector_to_closest_point);
    level_shading    = pow(0.8f, float(level));
    distance_shading = clamp(1.f - (distance_to_closest_point / resolution), 0.f, 1.f);

    gl_Position = projection_from_world * vec4(world_position, 1.f);

    // NOTE: must call glEnable(GL_PROGRAM_POINT_SIZE) for this to work
    float point_radius = resolution;
    gl_PointSize = float(viewport_height) * projection_from_view[1][1] * point_radius / gl_Position.w;
}

//void main()
//{
//    world_position   = vec3(center_and_radius);
//    world_normal     = vec3(0.f);
//    level_shading    = 0.f;
//    distance_shading = 0.f;
//
//    gl_Position = projection_from_world * vec4(world_position, 1.f);
//
//    // NOTE: must call glEnable(GL_PROGRAM_POINT_SIZE) for this to work
//    float point_radius = center_and_radius.w;
//    gl_PointSize = float(viewport_height) * projection_from_view[1][1] * point_radius / gl_Position.w;
//}
