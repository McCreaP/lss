ADD_SUBDIRECTORY(io)
add_executable(lss main.cc)
target_link_libraries(lss io)