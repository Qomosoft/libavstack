
cmake_minimum_required(VERSION 3.10)

project("rgbviewer")

add_library(${PROJECT_NAME} SHARED
        qomo_egl.cpp
        egl_renderer.cpp
        logging.cpp
        shader.cpp
        rgbviewer_jni.cpp)

find_library(log-lib log)

target_link_libraries(rgbviewer
        z
        android
        GLESv3
        EGL
        ${log-lib})
