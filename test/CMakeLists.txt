cmake_minimum_required(VERSION 3.5)


set(GAMMA_INCLUDE_TEST_DIR "${PROJECT_SOURCE_DIR}/test/include")
set(GAMMA_INCLUDE_TEST_FULL_DIR "${GAMMA_INCLUDE_TEST_DIR}/tests")
set(GOOGLE_TEST_DIR "${PROJECT_SOURCE_DIR}/third_party/googletest/googletest")
set(GOOGLE_MOCK_DIR "${PROJECT_SOURCE_DIR}/third_party/googletest/googlemock")

add_executable(
  global_tests
    global_test.cpp
    ExitWindow/test.cpp
    MainWindow/test.cpp
    Manager/test.cpp
    ${GOOGLE_TEST_DIR}/src/gtest-all.cc
    ${GOOGLE_MOCK_DIR}/src/gmock-all.cc
)

set(GOOGLE_INCLUDES ${GOOGLE_TEST_DIR} ${GOOGLE_MOCK_DIR} ${GOOGLE_TEST_DIR}/include ${GOOGLE_MOCK_DIR}/include)
target_include_directories(
  global_tests PRIVATE 
    ${GOOGLE_INCLUDES}

    ${GAMMA_INCLUDE_DIR}
    ${GAMMA_INCLUDE_TEST_DIR}
)


set(precompiled_header_name "test_pch.hpp")
set(ADDITIONAL_FLAGS "-I${GOOGLE_TEST_DIR} -I${GOOGLE_MOCK_DIR} -I${GOOGLE_TEST_DIR}/include -I${GOOGLE_MOCK_DIR}/include -I${GAMMA_INCLUDE_DIR}")
add_pch_target(
    precompile_gamma_test_header
    "${GAMMA_INCLUDE_TEST_FULL_DIR}/${precompiled_header_name}"
    ${ADDITIONAL_FLAGS}
)

add_dependencies(global_tests precompile_gamma_test_header)
target_link_libraries(
  global_tests
    pthread
    libbb

    # During travis build it lefts 
    # trailing spaces in macroses, and cmake fails.
    "${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES}"
)

add_test(NAME exe_tests
         COMMAND global_tests
         )
