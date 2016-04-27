include_directories(${CMAKE_SOURCE_DIR}/src)

find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_executable(lss main.cc)
add_executable(unit_tests main_test.cc)

add_subdirectory(base)
add_subdirectory(genetic)
add_subdirectory(greedy)
add_subdirectory(io)
add_subdirectory(local_search)

target_link_libraries(
        lss
        base genetic greedy io local_search permutation_chromosome
        glog ${Boost_LIBRARIES}
)

# Gtest doesn't find tests from separate compilation units.
# unit_tests contains no references to anything in the library that contains test definitions
# so linker doesn't need to link any object from that library to link program.
# Put your test libraries between "-Wl,--whole-archive" and "-Wl,--no-whole-archive"
# to prevent linker optimization
target_link_libraries(unit_tests -Wl,--whole-archive)
target_link_libraries(
        unit_tests
        base_test io_test local_search_test greedy_test genetic_test permutation_chromosome_test
        ${CXX_COVERAGE_LINK_FLAGS}
)
target_link_libraries(unit_tests -Wl,--no-whole-archive)

find_library(gtest gmock)
find_package(Threads REQUIRED)
target_link_libraries(unit_tests gtest gmock glog pthread ${CXX_COVERAGE_LINK_FLAGS})

install(TARGETS unit_tests DESTINATION ${CMAKE_SOURCE_DIR}/bin)
install(TARGETS lss DESTINATION ${CMAKE_SOURCE_DIR}/bin)
