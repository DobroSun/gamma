cmake_minimum_required(VERSION 3.5)

add_library(
  libbb STATIC
    window.cpp
    renderer.cpp
    sdl_impl.cpp
    action_state.cpp
    manager.cpp
)

target_include_directories(
  libbb PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
