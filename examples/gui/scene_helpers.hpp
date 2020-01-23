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
#pragma once

// project
#include "ltb/gvs/core/scene.hpp"
#include "ltb/sdf/box.hpp"
#include "ltb/sdf/geometry.hpp"
#include "ltb/sdf/oriented_line.hpp"

namespace ltb::example {

auto add_boxes_to_scene(gvs::Scene*                                    scene,
                        std::vector<sdf::Geometry<sdf::Box, 2>> const& boxes,
                        gvs::SceneId const&                            parent = gvs::nil_id()) -> gvs::SceneId;

auto add_lines_to_scene(gvs::Scene*                             scene,
                        std::vector<sdf::OrientedLine<>> const& oriented_lines,
                        gvs::SceneId const&                     parent = gvs::nil_id()) -> gvs::SceneId;

} // namespace ltb::example
