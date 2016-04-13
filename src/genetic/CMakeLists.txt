file(GLOB GENETIC_SRC *.cc)
file(GLOB GENETIC_TEST *_test.cc)

foreach (test ${GENETIC_TEST})
    list(REMOVE_ITEM GENETIC_SRC ${test})
endforeach ()

#add_library(genetic ${GENETIC_SRC})
add_library(genetic_test STATIC ${GENETIC_TEST} test_utils.cc)

#target_link_libraries(genetic_test genetic)
