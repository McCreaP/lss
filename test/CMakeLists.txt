# We need thread support
find_package(Threads REQUIRED)
# Enable ExternalProject CMake module
include(ExternalProject)

ADD_SUBDIRECTORY(gtest)
ADD_SUBDIRECTORY(gmock)

file(GLOB_RECURSE SRCS
        "*.cc")

add_executable(unit_tests ${SRCS})

TARGET_LINK_LIBRARIES(unit_tests
        io
        libgtest
        libgmock
        )