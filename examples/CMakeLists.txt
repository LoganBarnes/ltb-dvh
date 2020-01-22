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
file(GLOB_RECURSE LTB_SOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/gui/*
        )

### Optional CUDA ###
if (CMAKE_CUDA_COMPILER)
    file(GLOB_RECURSE LTB_CUDA_SOURCE_FILES
            LIST_DIRECTORIES false
            CONFIGURE_DEPENDS
            ${CMAKE_CURRENT_LIST_DIR}/cuda/*
            )
    list(APPEND LTB_SOURCE_FILES ${LTB_CUDA_SOURCE_FILES})
endif ()

ltb_add_executable(ltb_dvh_example 17 ${CMAKE_CURRENT_LIST_DIR}/exec/main.cpp ${LTB_SOURCE_FILES})
ltb_link_libraries(ltb_dvh_example
        PUBLIC
        ltb_gvs
        ltb_dvh
        doctest
        )
target_link_libraries(test_ltb_dvh_example PRIVATE ltb_gvs_testing)

ltb_include_directories(ltb_dvh_example PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>")