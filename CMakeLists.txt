cmake_minimum_required(VERSION 3.0)
project(lss)

set(CXX_WARNINGS_FLAGS "-Wall -Wextra -pedantic -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_WARNINGS_FLAGS} -std=c++14")

add_subdirectory(src)

