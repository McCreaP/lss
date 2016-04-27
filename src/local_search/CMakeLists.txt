file(GLOB SRC *.cc)
file(GLOB TEST *_test.cc)

foreach (test ${TEST})
    list(REMOVE_ITEM SRC ${test})
endforeach ()

add_library(local_search ${SRC})
add_library(local_search_test STATIC ${TEST})

target_link_libraries(local_search_test local_search)
