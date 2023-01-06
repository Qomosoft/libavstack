cmake_minimum_required(VERSION 3.10)
project(pngdecoder)

## fix arm64-v8a undefined reference to `png_init_filter_functions_neon'
if (${CMAKE_ANDROID_ARCH_ABI} STREQUAL "arm64-v8a")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPNG_ARM_NEON_OPT=0")
endif()

set(source_root ${PROJECT_SOURCE_DIR})

set(source_files
        ${source_root}/png.c
        ${source_root}/pngerror.c
        ${source_root}/pngget.c
        ${source_root}/pngmem.c
        ${source_root}/pngpread.c
        ${source_root}/pngread.c
        ${source_root}/pngrio.c
        ${source_root}/pngrtran.c
        ${source_root}/pngrutil.c
        ${source_root}/pngset.c
        ${source_root}/pngtrans.c
        ${source_root}/pngwio.c
        ${source_root}/pngwrite.c
        ${source_root}/pngwtran.c
        ${source_root}/pngwutil.c
        ${source_root}/image.c
        ${source_root}/png_decoder.cpp)

add_library(${PROJECT_NAME} STATIC ${source_files})
