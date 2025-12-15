cmake_minimum_required(VERSION 3.22)

project(itertools CXX)

find_package(benchmark REQUIRED)

add_compile_options(--std=c++17 -Wall -Wextra -Werror
  -Wconversion -Wsign-conversion
)

add_library(
  itertools INTERFACE
  include/itertools.hpp
)

add_executable(demo src/main.cpp)

target_link_libraries(demo itertools)

target_include_directories(demo PRIVATE include)

if (TESTING)
  enable_testing()
  find_package(GTest REQUIRED)

  add_executable(tests
    tests/test_basic.cpp
    tests/test_ranges.cpp
    tests/test_transformations.cpp
    tests/test_edge_cases.cpp
    tests/test_chaining.cpp
  )

  target_link_libraries(tests PRIVATE itertools GTest::GTest GTest::Main)
  target_include_directories(tests PRIVATE include)
  add_test(NAME tests COMMAND tests)
endif()
