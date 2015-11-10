# We need thread support
find_package(Threads REQUIRED)
# Enable ExternalProject CMake module

set(GMOCK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/gmock-1.7.0)
set(GTEST_DIR ${GMOCK_DIR}/gtest)

add_library(libgmock ${GTEST_DIR}/src/gtest-all.cc ${GMOCK_DIR}/src/gmock-all.cc)
target_link_libraries(libgmock pthread)

include_directories("${GTEST_DIR}/include"
        "${GTEST_DIR}"
        "${GMOCK_DIR}/include"
        "${GMOCK_DIR}")

add_executable(unit_tests
        main.cc
        io/raw_input_types_test.cc)

TARGET_LINK_LIBRARIES(unit_tests libgmock)