cmake_minimum_required(VERSION 3.28.0+)
project("sql")

file(GLOB sql_src
        "*.h"
        "*.cpp"
        "*.c")

add_executable(sql
        ${sql_src})

target_link_libraries(
        sql
        libutils
        libsqlite3
        ${log-lib}
)

set_target_properties(sql PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../assets")
