add_library(scheduler greedy_scheduler.cc)
add_library(scheduler_test STATIC greedy_scheduler_test.cc)

target_link_libraries(scheduler io oo_input_types)
target_link_libraries(scheduler_test scheduler)
