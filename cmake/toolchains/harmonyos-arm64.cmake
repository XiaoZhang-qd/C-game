set(CMAKE_SYSTEM_NAME OHOS)
set(CMAKE_SYSTEM_PROCESSOR arm64)

if(NOT DEFINED HOS_SDK_HOME)
    if(DEFINED ENV{HOS_SDK_HOME})
        set(HOS_SDK_HOME $ENV{HOS_SDK_HOME})
    elseif(DEFINED ENV{HARMONYOS_SDK_HOME})
        set(HOS_SDK_HOME $ENV{HARMONYOS_SDK_HOME})
    else()
        message(FATAL_ERROR "HOS_SDK_HOME or HARMONYOS_SDK_HOME environment variable not set.")
    endif()
endif()

set(CMAKE_TOOLCHAIN_FILE "${HOS_SDK_HOME}/native/llvm/build/cmake/ohos.toolchain.cmake" CACHE FILEPATH "HarmonyOS toolchain file")

set(OHOS_ARCH "arm64-v8a" CACHE STRING "HarmonyOS architecture")
set(OHOS_PLATFORM "ohos-12" CACHE STRING "HarmonyOS platform version")

if(NOT DEFINED CMAKE_C_COMPILER)
    find_program(CMAKE_C_COMPILER clang)
endif()

if(NOT DEFINED CMAKE_CXX_COMPILER)
    find_program(CMAKE_CXX_COMPILER clang++)
endif()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
