cmake_minimum_required(VERSION 3.22.1)

project("qomoplayer")

## FFmpeg
set(FFMPEG ${CMAKE_SOURCE_DIR}/third-party/ffmpeg_n4.4)
include_directories(${FFMPEG}/include)
link_directories(${FFMPEG}/lib)

## Renderer
set(RENDERER_SOURCE_FILES
        renderer/egl_renderer.cpp
        renderer/qomo_egl.cpp
        renderer/shader.cpp
        )

## Logging
set(LOG_SOURCE_FILES utils/logging.cpp)

include_directories(utils)

add_definitions("-D__STDC_CONSTANT_MACROS")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-Bsymbolic")

add_library(${CMAKE_PROJECT_NAME} SHARED
        ${RENDERER_SOURCE_FILES}
        ${LOG_SOURCE_FILES}
        qomoplayer_jni.cpp
        )

target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        log
        z
        GLESv2
        EGL
        avfilter
        avutil
        avformat
        avcodec
        swresample
        swscale
        )
