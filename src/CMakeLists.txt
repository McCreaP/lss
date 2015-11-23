include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(lss main.cc)
add_executable(unit_tests main_test.cc)

add_subdirectory(io)

target_link_libraries(lss io)
target_link_libraries(unit_tests io_test)

find_library(gtest gmock)
find_package(Threads REQUIRED)
target_link_libraries(unit_tests gtest pthread)

install(TARGETS unit_tests DESTINATION ${CMAKE_BINARY_DIR})
install(TARGETS lss DESTINATION ${CMAKE_BINARY_DIR})