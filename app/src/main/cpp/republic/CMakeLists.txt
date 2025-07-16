cmake_minimum_required(VERSION 3.22.1)
project("republic")

add_library(
        republic
        SHARED
        "hide_api.cpp")

target_link_libraries(
        republic
        log)
