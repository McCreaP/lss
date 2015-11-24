file(GLOB IO_SRC *.cc)
file(GLOB IO_TEST *_test.cc)

foreach (test ${IO_TEST})
    list(REMOVE_ITEM IO_SRC ${test})
endforeach ()

add_library(io ${IO_SRC})
add_library(io_test STATIC ${IO_TEST})

target_link_libraries(io_test io)