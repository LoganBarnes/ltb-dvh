#!/usr/bin/env sh
FILE_LIST="$(find src -type f -name '*.cpp' -o -name '*.hpp' -o -name '*.cu' -o -name '*.cuh')"
clang-format-8 -i -style=file $FILE_LIST
