cmake_minimum_required(VERSION 3.10)

project("pngviewer")

set(CMAKE_CXX_STANDARD 14)

find_library(log-lib log)

add_subdirectory(${PROJECT_SOURCE_DIR}/librenderer)

include_directories(
        ${PROJECT_SOURCE_DIR}/librenderer
        ${PROJECT_SOURCE_DIR}/libcommon)

add_library(pngviewer SHARED pngviewer_jni.cpp)
target_link_libraries(pngviewer ${log-lib} z android GLESv2 EGL renderer)
