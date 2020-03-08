function(add_pch_target _target_name _full_path_to_header _additional_flags)
  if(CMAKE_CXX_FLAGS)
    set(TMP_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    separate_arguments(CMAKE_CXX_FLAGS)
  else()
    set(TMP_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS "")
  endif()

  separate_arguments(_additional_flags)
  add_custom_target(
    ${_target_name}
    COMMAND ${CMAKE_CXX_COMPILER}
            ${CMAKE_CXX_FLAGS}
            ${_additional_flags}
            ${_full_path_to_header}
  )

  set(CMAKE_CXX_FLAGS ${TMP_CXX_FLAGS})
endfunction(add_pch_target)
