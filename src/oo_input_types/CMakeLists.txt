file(GLOB OO_INPUT_TYPES_SRC *.cc)
file(GLOB OO_INPUT_TYPES_TEST *_test.cc)

foreach (test ${OO_INPUT_TYPES_TEST})
    list(REMOVE_ITEM OO_INPUT_TYPES_SRC ${test})
endforeach ()

add_library(oo_input_types ${OO_INPUT_TYPES_SRC})
add_library(oo_input_types_test STATIC ${OO_INPUT_TYPES_TEST})

target_link_libraries(oo_input_types io)
target_link_libraries(oo_input_types_test oo_input_types)
