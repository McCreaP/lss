file(GLOB PERMUTATION_CHROMOSOME_SRC *.cc)
file(GLOB PERMUTATION_CHROMOSOME_TEST *_test.cc)

foreach (test ${PERMUTATION_CHROMOSOME_TEST})
    list(REMOVE_ITEM PERMUTATION_CHROMOSOME_SRC ${test})
endforeach ()

add_library(permutation_chromosome ${PERMUTATION_CHROMOSOME_SRC})
add_library(permutation_chromosome_test STATIC ${PERMUTATION_CHROMOSOME_TEST})

target_link_libraries(permutation_chromosome_test permutation_chromosome)
