cmake_minimum_required(VERSION 3.10.2)
project("sniffer")


find_library(
        log-lib
        log)

file(GLOB sniffer_src
        "./*.cpp"
        "./*.h"
        "../utils/log.h"
        "../utils/log.cpp"
        "../utils/utils.h"
        "../utils/utils.cpp"
        "../tinyxml2/*.h"
        "../tinyxml2/*.cpp"
        )

add_executable(
        sniffer
        ${sniffer_src})

target_link_libraries(
        sniffer
        libutils
        libxml
        ${log-lib})


set_target_properties(sniffer PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../assets")
