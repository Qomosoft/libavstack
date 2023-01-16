cmake_minimum_required(VERSION 3.6)
project(algorithm)
set(CMAKE_CXX_STANDARD 14)

file(GLOB source_files "*/*/*.cpp")
foreach(target_source_file ${source_files})
    get_filename_component(target_name ${target_source_file} NAME_WE)
    add_executable(${target_name} ${target_source_file})
endforeach(target_source_file ${source_files})
