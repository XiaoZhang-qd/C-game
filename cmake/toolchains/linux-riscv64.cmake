set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR riscv64)

if(NOT DEFINED CMAKE_C_COMPILER)
    find_program(CMAKE_C_COMPILER riscv64-linux-gnu-gcc)
    if(NOT CMAKE_C_COMPILER)
        find_program(CMAKE_C_COMPILER clang)
    endif()
endif()

if(NOT DEFINED CMAKE_CXX_COMPILER)
    find_program(CMAKE_CXX_COMPILER riscv64-linux-gnu-g++)
    if(NOT CMAKE_CXX_COMPILER)
        find_program(CMAKE_CXX_COMPILER clang++)
    endif()
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=rv64gc -mabi=lp64d" CACHE STRING "C flags for RISC-V 64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=rv64gc -mabi=lp64d" CACHE STRING "CXX flags for RISC-V 64")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
