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
#include "scene_helpers.hpp"

// project
#include "ltb/gvs/core/log_params.hpp"
#include "ltb/sdf/sdf.hpp"

// external
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ltb::example {

constexpr auto z_offset = 1e-3f;

auto add_boxes_to_scene(gvs::Scene*                                               scene,
                        std::vector<sdf::TransformedGeometry<sdf::Box, 2>> const& boxes,
                        gvs::SceneId const&                                       parent) -> gvs::SceneId {

    std::vector<glm::vec3> lines;

    for (const auto& square : boxes) {
        auto d = square.geometry.half_dimensions;

        auto p0 = glm::vec2(-d.x, -d.y) + square.translation;
        auto p1 = glm::vec2(+d.x, -d.y) + square.translation;
        auto p2 = glm::vec2(+d.x, +d.y) + square.translation;
        auto p3 = glm::vec2(-d.x, +d.y) + square.translation;

        lines.emplace_back(p0.x, p0.y, z_offset);
        lines.emplace_back(p1.x, p1.y, z_offset);

        lines.emplace_back(p1.x, p1.y, z_offset);
        lines.emplace_back(p2.x, p2.y, z_offset);

        lines.emplace_back(p2.x, p2.y, z_offset);
        lines.emplace_back(p3.x, p3.y, z_offset);

        lines.emplace_back(p3.x, p3.y, z_offset);
        lines.emplace_back(p0.x, p0.y, z_offset);
    }

    return scene->add_item(gvs::SetPositions3d(lines), gvs::SetLines(), gvs::SetParent(parent));
}

auto add_boxes_to_scene(gvs::Scene*                                               scene,
                        std::vector<sdf::TransformedGeometry<sdf::Box, 3>> const& boxes,
                        gvs::SceneId const&                                       parent) -> gvs::SceneId {

    auto root = scene->add_item(gvs::SetPositions3d(), gvs::SetParent(parent));

    for (auto const& box : boxes) {
        auto transform = glm::translate(glm::identity<glm::mat4>(), box.translation)
            * glm::scale(glm::identity<glm::mat4>(), box.geometry.half_dimensions);

        const auto* transform_data = glm::value_ptr(transform);
        gvs::mat4   tmp_conversion;

        for (auto i = 0u; i < 16u; ++i) {
            tmp_conversion[i] = transform_data[i];
        }

        scene->add_item(gvs::SetPrimitive(gvs::Cube{}), gvs::SetTransformation(tmp_conversion), gvs::SetParent(root));
    }

    return root;
}

auto add_lines_to_scene(gvs::Scene*                             scene,
                        std::vector<sdf::OrientedLine<>> const& oriented_lines,
                        gvs::SceneId const&                     parent) -> gvs::SceneId {

    std::vector<glm::vec3> lines;
    std::vector<glm::vec2> tex_coords;
    for (const auto& line : oriented_lines) {
        lines.emplace_back(line.start.x, line.start.y, z_offset);
        lines.emplace_back(line.end.x, line.end.y, z_offset);
        tex_coords.emplace_back(0.f, 0.f);
        tex_coords.emplace_back(1.f, 1.f);
    }
    assert(lines.size() == tex_coords.size());

    return scene->add_item(gvs::SetPositions3d(lines),
                           gvs::SetTextureCoordinates3d(tex_coords),
                           gvs::SetLines(),
                           gvs::SetColoring(gvs::Coloring::TextureCoordinates),
                           gvs::SetShading(gvs::Shading::UniformColor),
                           gvs::SetParent(parent));
}

auto add_offset_lines_to_scene(gvs::Scene*                            scene,
                               std::vector<sdf::OffsetLine<2>> const& offset_lines,
                               gvs::SceneId const&                    parent) -> gvs::SceneId {

    std::vector<glm::vec3> lines;
    std::vector<glm::vec2> tex_coords;
    for (const auto& line : offset_lines) {
        lines.emplace_back(line.start.x, line.start.y, z_offset);
        lines.emplace_back(line.end.x, line.end.y, z_offset);
        tex_coords.emplace_back(0.f, 0.f);
        tex_coords.emplace_back(1.f, 1.f);
    }
    assert(lines.size() == tex_coords.size());

    return scene->add_item(gvs::SetPositions3d(lines),
                           gvs::SetTextureCoordinates3d(tex_coords),
                           gvs::SetLines(),
                           gvs::SetColoring(gvs::Coloring::TextureCoordinates),
                           gvs::SetShading(gvs::Shading::UniformColor),
                           gvs::SetParent(parent));
}

} // namespace ltb::example
