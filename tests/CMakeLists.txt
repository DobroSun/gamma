cmake_minimum_required(VERSION 3.5)


set(GOOGLE_TEST_DIR "${PROJECT_SOURCE_DIR}/googletest/googletest")
set(GOOGLE_MOCK_DIR "${PROJECT_SOURCE_DIR}/googletest/googlemock")

enable_testing()
#find_package(GTest REQUIRED)
#include_directories(${GTEST_INCLUDE_DIR})
add_executable(
  global_tests
    global_test.cpp
    ${GOOGLE_TEST_DIR}/src/gtest-all.cc
    ${GOOGLE_MOCK_DIR}/src/gmock-all.cc
)

target_include_directories(
  global_tests PRIVATE 
    ${GOOGLE_TEST_DIR}
    ${GOOGLE_MOCK_DIR}
    ${GOOGLE_TEST_DIR}/include
    ${GOOGLE_MOCK_DIR}/include

    ${PROJECT_SOURCE_DIR}/tests/include
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(
  global_tests
    pthread
)

add_test(NAME exe_tests
         COMMAND global_tests
         DEPENDS main_test
         )
