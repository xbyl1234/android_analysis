cmake_minimum_required(VERSION 3.22.1)
project("third")

file(GLOB utils_src
        "utils/*.h"
        "utils/*.cpp"
        "utils/*.c")

add_library(libutils
        STATIC
        ${utils_src})

file(GLOB xml_src
        "tinyxml2/*.h"
        "tinyxml2/*.cpp"
        "tinyxml2/*.c")

add_library(libxml
        STATIC
        ${xml_src})


file(GLOB json_src
        "json/*.h"
        "json/*.cpp"
        "json/*.c")

add_library(libjson
        STATIC
        ${json_src})

add_library(selinux SHARED IMPORTED)
set_target_properties(
        selinux PROPERTIES
        IMPORTED_LOCATION ${LINK_DIR}/libselinux.so
        INTERFACE_INCLUDE_DIRECTORIES "selinux/libselinux/include"
)
set(SelinuxInclude "${CMAKE_CURRENT_SOURCE_DIR}/selinux/libselinux/include" PARENT_SCOPE)
include_directories("selinux/libselinux/include")

file(GLOB md5_src
        "md5/*.h"
        "md5/*.cpp"
        "md5/*.c"
        )

add_library(libmd5
        STATIC
        ${md5_src})

add_library(
        libcxx
        SHARED
        IMPORTED
)

set_target_properties(libcxx
        PROPERTIES IMPORTED_LOCATION
        ${LINK_DIR}/libcxx.a)

file(GLOB http_src
        "http_server/*.h"
        "http_server/*.cpp"
        "http_server/*.c")

add_library(libhttp
        STATIC
        ${http_src})

set(DobbyHome "./dobby")
#set(GENERATE_SHARED OFF)
#set(DOBBY_DEBUG OFF)
add_subdirectory(${DobbyHome} dobby)

file(GLOB byopen_src
        "byopen/*.h"
        "byopen/*.cpp"
        "byopen/*.c")

add_library(libbyopen
        STATIC
        ${byopen_src})

file(GLOB sqlite3_src
        "sqlite/*.h"
        "sqlite/*.cpp"
        "sqlite/*.c")

add_library(libsqlite3
        STATIC
        ${sqlite3_src})

add_library(
        libhv
        SHARED
        IMPORTED
)

set_target_properties(libhv
        PROPERTIES IMPORTED_LOCATION
        ${LINK_DIR}/libhv.a)


file(GLOB base64_src
        "base64/*.h"
        "base64/*.cpp"
        "base64/*.c")

add_library(libbase64
        STATIC
        ${base64_src})
#include(libunwind/lzma/cmake)
add_subdirectory(libunwind)