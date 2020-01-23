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

namespace ltb::example {

auto add_boxes_to_scene(gvs::Scene*                                    scene,
                        std::vector<sdf::Geometry<sdf::Box, 2>> const& boxes,
                        gvs::SceneId const&                            parent) -> gvs::SceneId {

    std::vector<glm::vec3> lines;

    for (const auto& square : boxes) {
        auto d = square.geometry.dimensions / 2.f;

        auto p0 = glm::vec2(-d.x, -d.y) + square.translation;
        auto p1 = glm::vec2(d.x, -d.y) + square.translation;
        auto p2 = glm::vec2(d.x, d.y) + square.translation;
        auto p3 = glm::vec2(-d.x, d.y) + square.translation;

        lines.emplace_back(p0.x, p0.y, 0.001f);
        lines.emplace_back(p1.x, p1.y, 0.001f);

        lines.emplace_back(p1.x, p1.y, 0.001f);
        lines.emplace_back(p2.x, p2.y, 0.001f);

        lines.emplace_back(p2.x, p2.y, 0.001f);
        lines.emplace_back(p3.x, p3.y, 0.001f);

        lines.emplace_back(p3.x, p3.y, 0.001f);
        lines.emplace_back(p0.x, p0.y, 0.001f);
    }

    return scene->add_item(gvs::SetPositions3d(lines),
                           gvs::SetGeometryFormat(gvs::GeometryFormat::Lines),
                           gvs::SetParent(parent));
}

} // namespace ltb::example
