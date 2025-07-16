cmake_minimum_required(VERSION 3.28.0+)
project("third")
enable_language(ASM)

file(GLOB utils_src
        "utils/*.h"
        "utils/*.cpp"
        "utils/*.c"
        "utils/*.asm"
)

add_library(libutils
        STATIC
        ${utils_src})

file(GLOB json_src
        "json/*.h"
        "json/*.cpp"
        "json/*.c")

add_library(libjson
        STATIC
        ${json_src})

file(GLOB md5_src
        "md5/*.h"
        "md5/*.cpp"
        "md5/*.c"
)

add_library(libmd5
        STATIC
        ${md5_src})

file(GLOB byopen_src
        "byopen/*.h"
        "byopen/*.cpp"
        "byopen/*.c")

add_library(libbyopen
        STATIC
        ${byopen_src})

file(GLOB base64_src
        "base64/*.h"
        "base64/*.cpp"
        "base64/*.c")

add_library(libbase64
        STATIC
        ${base64_src})

#add_subdirectory(lsplant)