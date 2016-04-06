cmake_minimum_required(VERSION 3.0)
project(lss)

set(CXX_WARNINGS_FLAGS "-Wall -Wextra -pedantic -Werror")
set(CXX_COVERAGE_COMPILE_FLAGS "--coverage")
set(CXX_COVERAGE_LINK_FLAGS "--coverage")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_WARNINGS_FLAGS} -std=c++14 -fdiagnostics-color=always")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CXX_COVERAGE_COMPILE_FLAGS} -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

add_subdirectory(src)
