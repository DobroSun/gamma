cmake_minimum_required(VERSION 3.5)


function(create_pch_target _target_name _full_path_to_header _additional_flags)
  if(_additional_flags)
    separate_arguments(_additional_flags)
  endif()

  separate_arguments(CMAKE_CXX_FLAGS)
  add_custom_target(
    ${_target_name}
    COMMAND ${CMAKE_CXX_COMPILER}
            ${CMAKE_CXX_FLAGS}
            ${_additional_flags}
            ${_full_path_to_header}
  )
  # FIXME:
  # Somehow deal with
  # separate_arguments(CMAKE_CXX_FLAGS)
  # It changes global variable and breaks build.
  set(CMAKE_CXX_FLAGS ${TMP_CXX_FLAGS})
endfunction(create_pch_target _target_name _full_path_to_header _additional_flags)


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
create_pch_target(
    precompile_gamma_header
    "${GAMMA_INCLUDE_FULL_DIR}/${precompiled_header_name}"
    ""
)

add_dependencies(libbb precompile_gamma_header)
target_include_directories(
  libbb PUBLIC
    ${GAMMA_INCLUDE_DIR}
)
