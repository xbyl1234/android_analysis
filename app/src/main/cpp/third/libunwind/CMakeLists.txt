set(UNWINDSTACK_ROOT .)

#if (NOT TARGET lzma)
#    add_subdirectory(lzma)
#endif()

enable_language(ASM)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
include_directories(${UNWINDSTACK_ROOT} ${UNWINDSTACK_ROOT}/include ${UNWINDSTACK_ROOT}/lzma/C)

file(GLOB UNWINDSTACK_SOURCES
    ${UNWINDSTACK_ROOT}/ArmExidx.cpp
    ${UNWINDSTACK_ROOT}/DexFiles.cpp
    ${UNWINDSTACK_ROOT}/DwarfCfa.cpp
    ${UNWINDSTACK_ROOT}/DwarfDebugFrame.cpp
    ${UNWINDSTACK_ROOT}/DwarfEhFrame.cpp
    ${UNWINDSTACK_ROOT}/DwarfMemory.cpp
    ${UNWINDSTACK_ROOT}/DwarfOp.cpp
    ${UNWINDSTACK_ROOT}/DwarfSection.cpp
    ${UNWINDSTACK_ROOT}/Elf.cpp
    ${UNWINDSTACK_ROOT}/ElfInterface.cpp
    ${UNWINDSTACK_ROOT}/Global.cpp
    ${UNWINDSTACK_ROOT}/JitDebug.cpp
    ${UNWINDSTACK_ROOT}/Log.cpp
    ${UNWINDSTACK_ROOT}/MapInfo.cpp
    ${UNWINDSTACK_ROOT}/Maps.cpp
    ${UNWINDSTACK_ROOT}/Memory.cpp
    ${UNWINDSTACK_ROOT}/MemoryMte.cpp
    ${UNWINDSTACK_ROOT}/Regs.cpp
    ${UNWINDSTACK_ROOT}/Symbols.cpp
    ${UNWINDSTACK_ROOT}/ElfInterfaceArm.cpp
    ${UNWINDSTACK_ROOT}/LocalUnwinder.cpp
    ${UNWINDSTACK_ROOT}/android-base/stringprintf.cpp
    ${UNWINDSTACK_ROOT}/android-base/file.cpp
    ${UNWINDSTACK_ROOT}/android-base/strings.cpp
    ${UNWINDSTACK_ROOT}/getline.c
    ${UNWINDSTACK_ROOT}/RegsArm.cpp
    ${UNWINDSTACK_ROOT}/RegsArm64.cpp
    ${UNWINDSTACK_ROOT}/RegsMips.cpp
    ${UNWINDSTACK_ROOT}/RegsMips64.cpp
    ${UNWINDSTACK_ROOT}/RegsX86.cpp
    ${UNWINDSTACK_ROOT}/RegsX86_64.cpp
    ${UNWINDSTACK_ROOT}/DwarfEhFrameWithHdr.cpp
    ${UNWINDSTACK_ROOT}/Unwinder.cpp
)

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES arm)
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES x86_64)
    file(GLOB UNWINDSTACK_SOURCES_ASMGETREGS
             ${UNWINDSTACK_ROOT}/AsmGetRegsX86_64.S
        )
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES i686)
    file(GLOB UNWINDSTACK_SOURCES_ASMGETREGS
             ${UNWINDSTACK_ROOT}/AsmGetRegsX86.S
        )

else()
    add_definitions(-DEM_ARM=40)
endif()

add_library(unwindstack STATIC 
    ${UNWINDSTACK_SOURCES}
    ${UNWINDSTACK_SOURCES_ASMGETREGS}
)
# target_link_libraries(unwindstack lzma)
