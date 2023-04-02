cmake_minimum_required(VERSION 3.10)
project(renderer)

set(source_root ${PROJECT_SOURCE_DIR})
set(source_files
    ${source_root}/egl_renderer.cpp
    ${source_root}/logging.cpp
    ${source_root}/shader.cpp
    ${source_root}/qomo_egl.cpp)

add_library(${PROJECT_NAME} STATIC ${source_files})
