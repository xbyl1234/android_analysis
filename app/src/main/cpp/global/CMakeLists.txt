cmake_minimum_required(VERSION 3.28.0+)
project("global")

file(GLOB global_src
        "*.h"
        "*.cpp"
        "*.c")

add_library(global
        STATIC
        ${global_src})