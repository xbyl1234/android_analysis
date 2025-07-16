cmake_minimum_required(VERSION 3.22.1)
project("trace")

add_library(libqbdi
        STATIC
        IMPORTED)

set_target_properties(libqbdi
        PROPERTIES IMPORTED_LOCATION
        ${LINK_DIR}/libQBDI.a)

file(GLOB trace_src
        "*.h"
        "*.hpp"
        "*.cpp"
)

add_library(
        trace
        STATIC
        ${trace_src}
)

target_link_libraries(
        trace
        libqbdi
)