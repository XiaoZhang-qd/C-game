# Cross-compilation toolchain for Linux ARM32 (armv7l)
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/linux-arm32.cmake ..

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

if(NOT DEFINED CMAKE_C_COMPILER)
    find_program(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
    if(NOT CMAKE_C_COMPILER)
        find_program(CMAKE_C_COMPILER armv7l-linux-gnueabihf-gcc)
    endif()
    if(NOT CMAKE_C_COMPILER)
        find_program(CMAKE_C_COMPILER clang)
    endif()
endif()

if(NOT DEFINED CMAKE_CXX_COMPILER)
    find_program(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
    if(NOT CMAKE_CXX_COMPILER)
        find_program(CMAKE_CXX_COMPILER armv7l-linux-gnueabihf-g++)
    endif()
    if(NOT CMAKE_CXX_COMPILER)
        find_program(CMAKE_CXX_COMPILER clang++)
    endif()
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv7-a -mfloat-abi=hard" CACHE STRING "C flags for ARM32")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a -mfloat-abi=hard" CACHE STRING "CXX flags for ARM32")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -march=armv7-a" CACHE STRING "Linker flags for ARM32")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
