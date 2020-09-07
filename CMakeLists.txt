cmake_minimum_required(VERSION 3.5)

project(gamma)


set(GAMMA_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(GAMMA_INCLUDE_FULL_DIR "${GAMMA_INCLUDE_DIR}/gamma")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2 -std=c++17")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")


find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})



if(${SDL2_FOUND})
  message(STATUS "Found SDL2 library: ${SDL2_LIBRARIES}.")
else()
  message(STATUS "Not found SDL2.")
endif()
if(${SDL2_IMAGE_FOUND})
  message(STATUS "Found SDL2_image library: ${SDL2_IMAGE_LIBRARIES}.")
else()
  message(STATUS "Not found SDL2_image.")
endif()
if(${SDL2_TTF_FOUND})
  message(STATUS "Found SDL2_ttf library: ${SDL2_TTF_LIBRARIES}.")
else()
  message(STATUS "Not found SDL2_ttf.")
endif()


include(PrecompiledHeader)
add_subdirectory(src)

set(BUILD_BENCHMARK "${BUILD_BENCHMARK}")
set(BUILD_TESTING "${BUILD_TESTING}")
if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
elseif(BUILD_BENCHMARK)
  enable_testing()
  add_subdirectory(benchmark)
else()
  add_executable(
    main
      src/main.cpp
  )

  target_link_libraries(main libgamma "${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES}")
endif()
