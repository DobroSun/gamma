cmake_minimum_required(VERSION 3.5)

project(gamma)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

add_subdirectory(src)

if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()

add_executable(
  main
    src/main.cpp
)

target_link_libraries(main libbb ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
