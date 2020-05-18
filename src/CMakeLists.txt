cmake_minimum_required(VERSION 3.5)


set(gamma_source
  utility.cpp
  cache.cpp
)

add_library(
  libgamma STATIC
  ${gamma_source}
)

set(precompiled_header_name "pch.h")
add_pch_target(
    precompile_gamma_header
    "${GAMMA_INCLUDE_FULL_DIR}/${precompiled_header_name}"
    ""
)

add_dependencies(libgamma precompile_gamma_header)
target_include_directories(
  libgamma PUBLIC
    ${GAMMA_INCLUDE_DIR}
)
