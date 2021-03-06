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
cmake_minimum_required(VERSION 3.15)
project(LtbDistanceVolumeHierarchy LANGUAGES CXX)

option(LTB_BUILD_DVH_EXAMPLES "Build example programs" OFF)

include(ltb-gvs/ltb-util/cmake/LtbConfig.cmake) # <-- Additional project options are in here.

if (CMAKE_CUDA_COMPILER)
    # Fix CUDA compilation warnings
    list(APPEND
            LTB_COMPILE_FLAGS
            $<$<COMPILE_LANGUAGE:CXX>:-fsized-deallocation>
            $<$<COMPILE_LANGUAGE:CUDA>:-Xcudafe --diag_suppress=esa_on_defaulted_function_ignored>
            )
endif ()

################################
### Third Party Dependencies ###
################################
include(cmake/ThirdParty.cmake)

############
### SDFs ###
############
file(GLOB_RECURSE LTB_SOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/sdf/*
        )

ltb_add_library(ltb_sdf 17 ${LTB_SOURCE_FILES})
ltb_link_libraries(ltb_sdf
        PUBLIC
        ltb_util
        glm
        doctest
        )
ltb_include_directories(ltb_sdf PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>")

###########
### DVH ###
###########
file(GLOB_RECURSE LTB_SOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/dvh/*
        )

ltb_add_library(ltb_dvh 17 ${LTB_SOURCE_FILES})
ltb_link_libraries(ltb_dvh
        PUBLIC
        ltb_sdf
        )

if (CMAKE_CUDA_COMPILER)
    ltb_include_directories(ltb_dvh SYSTEM PUBLIC ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})
endif ()

################
### Examples ###
################
if (LTB_BUILD_DVH_EXAMPLES)

    ### GVS ###
    add_subdirectory(ltb-gvs)

    ### Examples ###
    add_subdirectory(examples)

    if (CMAKE_CUDA_COMPILER)
        add_subdirectory(tmp)
    endif ()
else ()

    ### Util ###
    add_subdirectory(ltb-gvs/ltb-util)
endif ()

ltb_symlink(cache)
