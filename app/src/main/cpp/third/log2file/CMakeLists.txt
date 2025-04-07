cmake_minimum_required(VERSION 3.28.0+)
project("log2file")

file(GLOB log2file_src
        "*.h"
        "*.cpp"
        "*.c")

add_library(log2file
        STATIC
        ${log2file_src})