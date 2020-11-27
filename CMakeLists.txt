cmake_minimum_required(VERSION 3.9.6)

project(vim)

add_library(vimlib INTERFACE)

target_include_directories(vimlib INTERFACE "${PROJECT_SOURCE_DIR}/include")

target_compile_features(vimlib INTERFACE cxx_std_17)

add_subdirectory(src)
add_subdirectory(unit_tests)
