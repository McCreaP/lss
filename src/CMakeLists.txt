add_executable(lss main.cc
        io/basic_input.cc)
target_link_libraries(lss)
install(TARGETS lss DESTINATION ${CMAKE_SOURCE_DIR})
set_target_properties(lss
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        )