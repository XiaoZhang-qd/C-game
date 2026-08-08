# Cross-compilation toolchain for Linux ARM64 (aarch64)
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/linux-arm64.cmake ..

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

if(NOT DEFINED CMAKE_C_COMPILER)
    find_program(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
    if(NOT CMAKE_C_COMPILER)
        find_program(CMAKE_C_COMPILER clang)
    endif()
endif()

if(NOT DEFINED CMAKE_CXX_COMPILER)
    find_program(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
    if(NOT CMAKE_CXX_COMPILER)
        find_program(CMAKE_CXX_COMPILER clang++)
    endif()
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a" CACHE STRING "C flags for ARM64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a" CACHE STRING "CXX flags for ARM64")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
