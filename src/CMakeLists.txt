cmake_minimum_required(VERSION 3.5)


set(libbb_source
  window.cpp
  sdl_impl.cpp
  action_state.cpp
  manager.cpp
  canvas.cpp
  widget.cpp
)

add_library(
  libbb STATIC
  ${libbb_source}
)

set(precompiled_header_name "pch.hpp")
add_pch_target(
    precompile_gamma_header
    "${GAMMA_INCLUDE_FULL_DIR}/${precompiled_header_name}"
    ""
)

add_dependencies(libbb precompile_gamma_header)
target_include_directories(
  libbb PUBLIC
    ${GAMMA_INCLUDE_DIR}
)
