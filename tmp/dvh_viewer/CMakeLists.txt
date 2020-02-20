file(GLOB_RECURSE SOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/*
        )

add_executable(dvh_viewer ${SOURCE_FILES})

target_link_libraries(dvh_viewer
        PRIVATE
        sim-loop
        gpu-gl-interop
        )
target_include_directories(dvh_viewer
        SYSTEM PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>"
        )

set_target_properties(dvh_viewer PROPERTIES
        # C++ flags
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE ON
        # CUDA flags
        CUDA_STANDARD 14
        CUDA_STANDARD_REQUIRED ON
        CUDA_EXTENSIONS OFF
        CUDA_SEPARABLE_COMPILATION ON
        )
