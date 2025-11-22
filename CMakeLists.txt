cmake_minimum_required(VERSION 3.22)

project(itertools CXX)

find_package(benchmark REQUIRED)

add_compile_options(--std=c++17 -Wall -Wextra -Werror -Wconversion -Wsign-conversion)

include_directories(include/)

add_executable(test src/main.cpp)

target_link_libraries(test)

target_include_directories(test PRIVATE include)
