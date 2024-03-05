cmake_minimum_required(VERSION 3.22.1)

project("qomoplayer")

set(FFMPEG ${CMAKE_SOURCE_DIR}/third-party/ffmpeg_n4.4)
include_directories(${FFMPEG}/include)
link_directories(${FFMPEG}/lib)

add_definitions("-D__STDC_CONSTANT_MACROS")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-Bsymbolic")

add_library(${CMAKE_PROJECT_NAME} SHARED
        native-lib.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        log
        z
        avfilter
        avutil
        avformat
        avcodec
        swresample
        swscale
        )
