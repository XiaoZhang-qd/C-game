# Cross-compilation toolchain for Windows ARM64
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/windows-arm64.cmake ..

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR ARM64)

if(NOT DEFINED CMAKE_C_COMPILER)
    find_program(CMAKE_C_COMPILER aarch64-w64-mingw32-gcc)
    if(NOT CMAKE_C_COMPILER)
        find_program(CMAKE_C_COMPILER clang)
    endif()
endif()

if(NOT DEFINED CMAKE_CXX_COMPILER)
    find_program(CMAKE_CXX_COMPILER aarch64-w64-mingw32-g++)
    if(NOT CMAKE_CXX_COMPILER)
        find_program(CMAKE_CXX_COMPILER clang++)
    endif()
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -target aarch64-windows-gnu" CACHE STRING "C flags for ARM64 Windows")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -target aarch64-windows-gnu" CACHE STRING "CXX flags for ARM64 Windows")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -target aarch64-windows-gnu" CACHE STRING "Linker flags for ARM64 Windows")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
