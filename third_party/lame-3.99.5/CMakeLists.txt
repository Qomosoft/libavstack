cmake_minimum_required(VERSION 3.6)

project (mp3lame)

if(WIN32)
    add_subdirectory(${project_root}/Dll)
endif()

add_definitions(-DHAVE_STDINT_H)
add_definitions(-DHAVE_MPGLIB)
add_definitions(-DDECODE_ON_THE_FLY)
add_definitions(-DUSE_FAST_LOG)
add_definitions(-DTAKEHIRO_IEEE754_HACK)

if((${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC))
    add_definitions(-DUSE_LAYER_2)
    add_definitions(-DHAVE_CONFIG_H)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATED)
    configure_file(configMS.h config.h @ONLY)
else()
    add_definitions(-DSTDC_HEADERS)
    if(NOT ieee754_float32_t)
        add_definitions(-Dieee754_float32_t=float)
    endif()
endif()

set(project_root ${CMAKE_CURRENT_SOURCE_DIR})

aux_source_directory(${project_root}/libmp3lame source_files)
aux_source_directory(${project_root}/libmp3lame/vector source_files)
aux_source_directory(${project_root}/mpglib source_files)

include_directories(${project_root}
    ${project_root}/include
    ${project_root}/libmp3lame
    ${project_root}/mpglib
    ${CMAKE_CURRENT_BINARY_DIR})

add_library(${PROJECT_NAME} STATIC ${source_files})
target_include_directories(${PROJECT_NAME} PUBLIC ${project_root}/include)
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
