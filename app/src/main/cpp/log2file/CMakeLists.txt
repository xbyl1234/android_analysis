cmake_minimum_required(VERSION 3.22.1)
project("log2file")

file(GLOB log2file_src
        "*.h"
        "*.cpp"
        "*.c")

add_library(log2file
        STATIC
        ${log2file_src})