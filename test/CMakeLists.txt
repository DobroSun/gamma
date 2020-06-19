cmake_minimum_required(VERSION 3.5)


add_executable(
  tests
    run.cpp
)

target_include_directories(
  tests PRIVATE 
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/test
)

target_link_libraries(
  tests
    pthread
    libgamma
    stdc++fs
    "${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES}"
)

add_test(NAME exe_tests
         COMMAND tests 
         )
