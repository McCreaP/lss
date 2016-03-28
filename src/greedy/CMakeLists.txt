file(GLOB GREEDY_SRC *.cc)
file(GLOB GREEDY_TEST *_test.cc)

foreach (test ${GREEDY_TEST})
    list(REMOVE_ITEM GREEDY_SRC ${test})
endforeach ()

add_library(greedy ${GREEDY_SRC})
add_library(greedy_test STATIC ${GREEDY_TEST})

target_link_libraries(greedy_test greedy)
