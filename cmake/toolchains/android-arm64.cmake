if(NOT DEFINED ANDROID_NDK_HOME AND NOT DEFINED ANDROID_NDK)
    if(DEFINED ENV{ANDROID_NDK_HOME})
        set(ANDROID_NDK_HOME $ENV{ANDROID_NDK_HOME})
    elseif(DEFINED ENV{ANDROID_NDK})
        set(ANDROID_NDK_HOME $ENV{ANDROID_NDK})
    else()
        message(FATAL_ERROR "ANDROID_NDK_HOME or ANDROID_NDK environment variable not set.")
    endif()
endif()

set(CMAKE_TOOLCHAIN_FILE "${ANDROID_NDK_HOME}/build/cmake/android.toolchain.cmake" CACHE FILEPATH "Android toolchain file")

set(ANDROID_ABI "arm64-v8a" CACHE STRING "Android ABI")
set(ANDROID_PLATFORM "android-24" CACHE STRING "Android platform level")
set(ANDROID_STL "c++_shared" CACHE STRING "Android STL")

set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
