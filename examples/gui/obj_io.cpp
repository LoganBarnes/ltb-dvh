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
#include "obj_io.hpp"

// project
#include "ltb/util/file_utils.hpp"
#include "ltb/util/string.hpp"

// standard
#include <sstream>

namespace ltb {
namespace io {
namespace {

// Unchecked code copied from elsewhere.
std::vector<std::string> split(const std::string& string, const std::string& token, bool ignore_empty = false) {
    std::vector<std::string> tokens;

    size_t pos          = 0;
    size_t length       = string.length();
    size_t token_length = token.length();

    while (pos < length) {
        size_t location = string.find(token, pos);
        if (location == std::string::npos) {
            location = length;
        }

        if (ignore_empty || location - pos > 0) {
            tokens.push_back(string.substr(pos, location - pos));
        }

        pos = location + token_length;
    }

    return tokens;
}

} // namespace

auto load_obj(std::string const& path) -> Mesh3 {
    Mesh3 mesh           = {};
    mesh.geometry_format = gvs::GeometryFormat::Triangles;

    auto obj_data_result = util::read_file_to_string(path); // throw on bad read
    if (!obj_data_result) {
        throw std::runtime_error(obj_data_result.error().debug_error_message());
    }

    std::istringstream gcode_stream(obj_data_result.value());

    for (std::string line; std::getline(gcode_stream, line);) {
        std::vector<std::string> components = split(line, " ");

        if (components.empty()) {
            continue;
        }

        std::string& command = components.at(0);
        if (command == "v") {
            float x = std::stof(components.at(1));
            float y = std::stof(components.at(2));
            float z = std::stof(components.at(3));

            mesh.vertices.emplace_back(x, y, z);

        } else if (command == "f") {

            for (auto i = 1ul; i < components.size(); i++) {
                std::string const& component = components[i];
                auto const         indices   = split(component, "/");
                std::uint32_t      index     = std::stoul(indices[0]) - 1u;
                mesh.indices.emplace_back(index);
            }
        }
    }

    return mesh;
}

} // namespace io
} // namespace ltb
