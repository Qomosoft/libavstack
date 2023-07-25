cmake_minimum_required(VERSION 3.6.0)

project("qomoopensles")

add_library(${PROJECT_NAME} SHARED
        opensles_jni.cpp
        logging.cpp
        opensles_player.cpp)

target_link_libraries(${PROJECT_NAME}
        android
        log
        OpenSLES)