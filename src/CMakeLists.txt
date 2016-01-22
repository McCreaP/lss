include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(lss main.cc)
add_executable(unit_tests main_test.cc)

add_subdirectory(io)
add_subdirectory(oo_input_types)
add_subdirectory(scheduler)

target_link_libraries(lss scheduler glog)

# Gtest doesn't find tests from separate compilation units.
# unit_tests contains no references to anything in the library that contains test definitions
# so linker doesn't need to link any object from that library to link program.
# Put your test libraries between "-Wl,--whole-archive" and "-Wl,--no-whole-archive"
# to prevent linker optimization
target_link_libraries(unit_tests -Wl,--whole-archive)
target_link_libraries(unit_tests io_test oo_input_types_test)
target_link_libraries(unit_tests -Wl,--no-whole-archive)

find_library(gtest gmock)
find_package(Threads REQUIRED)
target_link_libraries(unit_tests gtest gmock glog pthread ${CXX_COVERAGE_LINK_FLAGS})

install(TARGETS unit_tests DESTINATION ${CMAKE_SOURCE_DIR}/bin)
install(TARGETS lss DESTINATION ${CMAKE_SOURCE_DIR}/bin)
