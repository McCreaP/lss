cmake_minimum_required(VERSION 3.0)
project(lss)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

INCLUDE_DIRECTORIES(src)

ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(src)

