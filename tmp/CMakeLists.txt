##########################################################################################
# LTB Distance Volume Hierarchy
# Copyright (c) 2020 Logan Barnes - All Rights Reserved
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
##########################################################################################
corrade_add_resource(ltb_dvh_display_RESOURCES ${CMAKE_CURRENT_LIST_DIR}/resources.conf)
add_library(ltb_dvh_display_resources ${ltb_dvh_display_RESOURCES})
target_link_libraries(ltb_dvh_display_resources PUBLIC ltb_gvs)

file(GLOB_RECURSE LTB_IGNORE_SOURCES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/dvh_viewer/*
        )

file(GLOB_RECURSE LTB_SOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/*.hpp
        ${CMAKE_CURRENT_LIST_DIR}/*.cu
        ${CMAKE_CURRENT_LIST_DIR}/*.cuh
        )
list(REMOVE_ITEM LTB_SOURCE_FILES ${LTB_IGNORE_SOURCES})

add_executable(ltb_tmp ${LTB_SOURCE_FILES})

target_link_libraries(ltb_tmp
        PUBLIC
        ltb_dvh_display_resources
        ltb_dvh
        doctest
        )
target_include_directories(ltb_tmp PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>")

target_compile_options(ltb_tmp PRIVATE ${LTB_COMPILE_FLAGS})
target_compile_definitions(ltb_tmp PRIVATE -DDOCTEST_CONFIG_DISABLE)
ltb_set_properties(ltb_tmp 17)
