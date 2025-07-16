cmake_minimum_required(VERSION 3.28.0+)
project("analyse")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/${ANDROID_ABI}")
set(LINK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/static_lib/${ANDROID_ABI})

find_library(log-lib log)
add_library(dobby
        STATIC
        IMPORTED)

set_target_properties(dobby
        PROPERTIES IMPORTED_LOCATION
        ${LINK_DIR}/libdobby.a)


add_subdirectory(./base)
add_subdirectory(./global)
add_subdirectory(third/log2file)
add_subdirectory(./third)
#include_directories(./third/libunwind/include)
add_subdirectory(./trace)

file(GLOB analyse_src
        "*.h"
        "*.hpp"
        "*.cpp"

        "jni_trace/*.cpp"
        "jni_trace/*.h"

#        "analyse/*cpp"
#        "analyse/*h"
        )


add_library(
        analyse
        SHARED
        ${analyse_src}
)

target_link_libraries(
        analyse
        dobby
        libbase
        libutils
        libbyopen
        libjson
        libbase64
#        unwindstack
        trace
        global
        log2file
        ${log-lib}
)

file(GLOB test_src
        "test/*.cpp"
        "test/*.h"
        )


add_library(
        test
        SHARED
        ${test_src}
)

target_link_libraries(
        test
        libbase
        libutils
        libjson
        ${log-lib}
)

#add_subdirectory(sniffer)
#add_subdirectory(sqllite)
#add_subdirectory(republic)