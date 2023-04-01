cmake_minimum_required(VERSION 3.10)
project(renderer)

set(source_root ${PROJECT_SOURCE_DIR})
set(source_files
    ${source_root}/pic_preview_controller.cpp
    ${source_root}/pic_preview_render.cpp
    ${source_root}/egl_core.cpp
    ${source_root}/pic_preview_texture.cpp)

include_directories(
        ${source_root}/../libcommon/)

add_library(${PROJECT_NAME} STATIC ${source_files})
