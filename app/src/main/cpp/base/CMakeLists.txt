cmake_minimum_required(VERSION 3.22.1)
project("base")

file(GLOB base_src
        "*.h"
        "*.cpp"
        "*.c")


add_library(libbase
        STATIC
        ${base_src})