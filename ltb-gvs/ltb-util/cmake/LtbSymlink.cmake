##########################################################################################
# LTB Utilities
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
# create a symlink in the binary directory to the source specified as target
function(ltb_symlink target)
    get_filename_component(file ${target} NAME)

    if (UNIX)
        execute_process(
                COMMAND
                ln;-nsf;${CMAKE_CURRENT_SOURCE_DIR}/${target};${CMAKE_BINARY_DIR}/${file}
                RESULT_VARIABLE
                result
        )
    else ()
        message(STATUS "Symbolic Links not supported on this OS")
        return()
    endif ()

    if (NOT "${result}" STREQUAL "0")
        message(FATAL_ERROR "failed to create symlink ${target}")
    endif ()
endfunction()