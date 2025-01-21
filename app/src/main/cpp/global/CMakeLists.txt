cmake_minimum_required(VERSION 3.22.1)
project("global")

file(GLOB global_src
        "*.h"
        "*.cpp"
        "*.c")

add_library(global
        STATIC
        ${global_src})