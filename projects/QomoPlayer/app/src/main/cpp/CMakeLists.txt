cmake_minimum_required(VERSION 3.22.1)

project("qomoplayer_jni")

## FFmpeg
set(FFMPEG ${CMAKE_SOURCE_DIR}/third-party/ffmpeg_n4.4)
include_directories(${FFMPEG}/include)
link_directories(${FFMPEG}/lib)

## Media Player
set(MEDIA_PLAYER_SOURCE_FILES
        media/media_player.cpp
        )

## AVSynchronizer
set(AVSYNCHRONIZER_SOURCE_FILES
        synchronizer/av_synchronizer.cpp
        )

## Decoder
set(VIDEO_DECODER_SOURCE_FILES
        decoder/sw_video_decoder.cpp
        decoder/media_frame.cpp
        )

## Renderer
set(RENDERER_SOURCE_FILES
        renderer/video/egl_renderer.cpp
        renderer/video/qomo_egl.cpp
        renderer/video/shader.cpp
        )

## Logging
set(LOG_SOURCE_FILES utils/logging.cpp)

include_directories(utils
        media
        synchronizer
        decoder
        )

add_definitions("-D__STDC_CONSTANT_MACROS")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-Bsymbolic")

add_library(${CMAKE_PROJECT_NAME} SHARED
        ${MEDIA_PLAYER_SOURCE_FILES}
        ${AVSYNCHRONIZER_SOURCE_FILES}
        ${VIDEO_DECODER_SOURCE_FILES}
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
