cmake_minimum_required(VERSION 3.5)


function(add_precompiled_header precompiled_header precompiled_source source)
  set_source_files_properties(${precompiled_source}
      PROPERTIES COMPILE_FLAGS "/Yc${precompiled_header}")
  
  foreach(file ${${source}})
    set_source_files_properties(${file}
      PROPERTIES COMPILE_FLAGS "/Yu${precompiled_header}")
  endforeach()
  set(${source} ${${source}};${precompiled_source} PARENT_SCOPE)
endfunction()


set(libbb_source
  window.cpp
  sdl_impl.cpp
  action_state.cpp
  manager.cpp
  canvas.cpp
  widget.cpp
)

add_precompiled_header("pch.hpp" "pch.cpp" libbb_source)

add_library(
  libbb STATIC
  ${libbb_source}
)

target_include_directories(
  libbb PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
