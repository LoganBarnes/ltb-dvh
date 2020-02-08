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
#include "add_volume.cuh"
#include "subtract_volumes.cuh"

#define LTB_DVH_REGISTER_GEOMETRY_TYPE_2D(Type)                                                                        \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, float>::add_volume(                                        \
        const std::vector<Type<float>>& geometries);                                                                   \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, double>::add_volume(                                       \
        const std::vector<Type<double>>& geometries);                                                                  \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, float>::subtract_volumes(                                  \
        const std::vector<Type<float>>& geometries);                                                                   \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, double>::subtract_volumes(                                 \
        const std::vector<Type<double>>& geometries);

#define LTB_DVH_REGISTER_GEOMETRY_TYPE_3D(Type)                                                                        \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, float>::add_volume(                                        \
        const std::vector<Type<float>>& geometries);                                                                   \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, double>::add_volume(                                       \
        const std::vector<Type<double>>& geometries);                                                                  \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, float>::subtract_volumes(                                  \
        const std::vector<Type<float>>& geometries);                                                                   \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, double>::subtract_volumes(                                 \
        const std::vector<Type<double>>& geometries);

#define LTB_DVH_REGISTER_GEOMETRY_TYPE(Type)                                                                           \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, float>::add_volume(                                        \
        const std::vector<Type<2, float>>& geometries);                                                                \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, float>::add_volume(                                        \
        const std::vector<Type<3, float>>& geometries);                                                                \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, double>::add_volume(                                       \
        const std::vector<Type<2, double>>& geometries);                                                               \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, double>::add_volume(                                       \
        const std::vector<Type<3, double>>& geometries);                                                               \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, float>::add_volume(                                        \
        const std::vector<sdf::TransformedGeometry<Type, 2, float>>& geometries);                                      \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, float>::add_volume(                                        \
        const std::vector<sdf::TransformedGeometry<Type, 3, float>>& geometries);                                      \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, double>::add_volume(                                       \
        const std::vector<sdf::TransformedGeometry<Type, 2, double>>& geometries);                                     \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, double>::add_volume(                                       \
        const std::vector<sdf::TransformedGeometry<Type, 3, double>>& geometries);                                     \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, float>::subtract_volumes(                                  \
        const std::vector<Type<2, float>>& geometries);                                                                \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, float>::subtract_volumes(                                  \
        const std::vector<Type<3, float>>& geometries);                                                                \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, double>::subtract_volumes(                                 \
        const std::vector<Type<2, double>>& geometries);                                                               \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, double>::subtract_volumes(                                 \
        const std::vector<Type<3, double>>& geometries);                                                               \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, float>::subtract_volumes(                                  \
        const std::vector<sdf::TransformedGeometry<Type, 2, float>>& geometries);                                      \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, float>::subtract_volumes(                                  \
        const std::vector<sdf::TransformedGeometry<Type, 3, float>>& geometries);                                      \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<2, double>::subtract_volumes(                                 \
        const std::vector<sdf::TransformedGeometry<Type, 2, double>>& geometries);                                     \
    template void ::ltb::dvh::DistanceVolumeHierarchyGpu<3, double>::subtract_volumes(                                 \
        const std::vector<sdf::TransformedGeometry<Type, 3, double>>& geometries);
