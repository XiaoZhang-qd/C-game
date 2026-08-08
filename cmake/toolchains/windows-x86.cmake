# Cross-compilation toolchain for Windows x86 (32-bit)
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/windows-x86.cmake ..

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86)

if(NOT DEFINED CMAKE_C_COMPILER)
    find_program(CMAKE_C_COMPILER i686-w64-mingw32-gcc)
    if(NOT CMAKE_C_COMPILER)
        find_program(CMAKE_C_COMPILER gcc)
    endif()
endif()

if(NOT DEFINED CMAKE_CXX_COMPILER)
    find_program(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
    if(NOT CMAKE_CXX_COMPILER)
        find_program(CMAKE_CXX_COMPILER g++)
    endif()
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32" CACHE STRING "C flags for x86")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32" CACHE STRING "CXX flags for x86")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32" CACHE STRING "Linker flags for x86")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
