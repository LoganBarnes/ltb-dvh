##########################################################################################
# Geometry Visualization Server
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
function(add_example _directory _cpp17_enabled)
    set(target run_${_directory}_example)

    file(GLOB_RECURSE LTB_SOURCE_FILES
            LIST_DIRECTORIES false
            CONFIGURE_DEPENDS
            ${CMAKE_CURRENT_LIST_DIR}/common/*
            ${CMAKE_CURRENT_LIST_DIR}/${_directory}/*
            )

    add_executable(${target} ${LTB_SOURCE_FILES})
    target_link_libraries(${target} PUBLIC ltb_gvs)

    if (_cpp17_enabled)
        set(CPP_STANDARD 17)
    else ()
        set(CPP_STANDARD 11)
    endif ()

    target_compile_options(${target} PRIVATE ${LTB_COMPILE_FLAGS})
    set_target_properties(
            ${target}
            PROPERTIES
            # C++ flags
            CXX_STANDARD ${CPP_STANDARD}
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
            POSITION_INDEPENDENT_CODE ON
            # CCache
            COMPILER_LAUNCHER "${LTB_CCACHE_PROGRAM}"
            # Clang-Tidy
            CXX_CLANG_TIDY "${LTB_CLANG_TIDY}"
    )

endfunction()

if (LTB_ENABLE_GVS_DISPLAY)
    add_example(display_window TRUE)
    add_example(intersection TRUE)
    add_example(primitives TRUE)
    add_example(scene TRUE)
else ()
    message("-- (LTB_ENABLE_GVS_DISPLAY=OFF) Some examples will not be built: display_window intersection primitives scene")
endif ()
