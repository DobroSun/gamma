cmake_minimum_required(VERSION 3.5)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})
add_executable(
  global_tests
    global_test.cpp
)

target_link_libraries(
  global_tests
    ${GTEST_LIBRARIES}
    pthread
)

add_test(NAME exe_tests
         COMMAND global_tests
         DEPENDS main_test
         )
