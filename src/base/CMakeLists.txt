file(GLOB BASE_SRC *.cc)
file(GLOB BASE_TEST *_test.cc)

foreach (test ${BASE_TEST})
    list(REMOVE_ITEM BASE_SRC ${test})
endforeach ()

add_library(base ${BASE_SRC})
add_library(base_test STATIC ${BASE_TEST})

target_link_libraries(base_test base)
