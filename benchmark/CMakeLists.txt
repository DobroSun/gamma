cmake_minimum_required(VERSION 3.5)

add_executable(
  benchmarks 
    run.cpp
)

find_package(benchmark REQUIRED)
target_include_directories(
  benchmarks PRIVATE 
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/benchmark
)

target_link_libraries(
  benchmarks 
    pthread
    libgamma
    benchmark::benchmark
)

add_test(NAME exe_benchmark
         COMMAND benchmarks
         )
