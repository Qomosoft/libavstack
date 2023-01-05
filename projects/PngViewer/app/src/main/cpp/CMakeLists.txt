cmake_minimum_required(VERSION 3.10)

project("pngviewer")

add_subdirectory(${PROJECT_SOURCE_DIR}/3rdparty/libpng)

find_library(log-lib log)

add_library(pngviewer SHARED pngviewer_jni.cpp)

target_link_libraries(pngviewer ${log-lib} z pngdecoder)
