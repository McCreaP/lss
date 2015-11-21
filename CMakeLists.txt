cmake_minimum_required(VERSION 3.0)
project(lss CXX)

set(CXX_WARNINGS_FLAGS "-Wall -Wextra -pedantic -Werror")
# Gtest contains warnings of this type
set(CXX_EXCLUDE_WARNINGS_FLAGS "-Wno-missing-field-initializers")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_WARNINGS_FLAGS} ${CXX_EXCLUDE_WARNINGS_FLAGS} -std=c++14")

include_directories(${CMAKE_SOURCE_DIR}/src)

# gtest gmock
find_package(Threads REQUIRED)
set(GMOCK_DIR ${CMAKE_SOURCE_DIR}/../gmock-1.7.0)
set(GTEST_DIR ${GMOCK_DIR}/gtest)
add_library(libgmock ${GTEST_DIR}/src/gtest-all.cc ${GMOCK_DIR}/src/gmock-all.cc)
target_link_libraries(libgmock pthread)
include_directories("${GTEST_DIR}/include"
        "${GTEST_DIR}"
        "${GMOCK_DIR}/include"
        "${GMOCK_DIR}")

# building io library and it's test
file(GLOB_RECURSE IO_SRC src/io/*.cc)
file(GLOB_RECURSE IO_TEST src/io/*test.cc)
foreach (test ${IO_TEST})
    list(REMOVE_ITEM IO_SRC ${test})
endforeach ()
add_library(io ${IO_SRC})
add_library(io_test ${IO_TEST})

add_executable(unit_tests
        "src/main_test.cc"
        ${IO_SRC}
        ${IO_TEST})
target_link_libraries(unit_tests libgmock)
install(TARGETS unit_tests DESTINATION ${CMAKE_SOURCE_DIR})

add_executable(lss
        src/main.cc
        ${IO_SRC})
install(TARGETS lss DESTINATION ${CMAKE_BINARY_DIR})

