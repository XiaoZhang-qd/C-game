cmake_minimum_required(VERSION 3.15)

project(
    PXPT_Racer
    VERSION 1.0
    LANGUAGES C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ========================================
# CPU Architecture detection
# ========================================

set(PXPT_ARCH "${CMAKE_SYSTEM_PROCESSOR}")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64|AMD x86-64)$")
    set(PXPT_ARCH_X64 1)
    set(PXPT_ARCH_NAME "x64")
    set(PXPT_ARCH_BITS 64)
    set(PXPT_ARCH_X86_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i[3-6]86|x86)$")
    set(PXPT_ARCH_X86 1)
    set(PXPT_ARCH_NAME "x86")
    set(PXPT_ARCH_BITS 32)
    set(PXPT_ARCH_X86_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64|armv[78].*|arm64e)$")
    set(PXPT_ARCH_ARM64 1)
    set(PXPT_ARCH_NAME "arm64")
    set(PXPT_ARCH_BITS 64)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64e")
        set(PXPT_ARCH_ARM64E 1)
    endif()
    set(PXPT_ARCH_ARM_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm|armv[67].*|ARM|ARMv[67].*)$")
    set(PXPT_ARCH_ARM32 1)
    set(PXPT_ARCH_NAME "arm32")
    set(PXPT_ARCH_BITS 32)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "armv6")
        set(PXPT_ARCH_ARMV6 1)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "armv7")
        set(PXPT_ARCH_ARMV7 1)
    endif()
    set(PXPT_ARCH_ARM_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(riscv64|riscv|RISCV64|RISCV)$")
    set(PXPT_ARCH_RISCV64 1)
    set(PXPT_ARCH_NAME "riscv64")
    set(PXPT_ARCH_BITS 64)
    set(PXPT_ARCH_RISCV_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(riscv32|RISCV32)$")
    set(PXPT_ARCH_RISCV32 1)
    set(PXPT_ARCH_NAME "riscv32")
    set(PXPT_ARCH_BITS 32)
    set(PXPT_ARCH_RISCV_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(ppc64|powerpc64|PPC64|ppc64le|powerpc64le)$")
    set(PXPT_ARCH_PPC64 1)
    set(PXPT_ARCH_NAME "ppc64")
    set(PXPT_ARCH_BITS 64)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "(ppc64le|powerpc64le|ppc64el)")
        set(PXPT_ARCH_PPC64LE 1)
    endif()
    set(PXPT_ARCH_PPC_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(ppc|powerpc|PPC)$")
    set(PXPT_ARCH_PPC32 1)
    set(PXPT_ARCH_NAME "ppc32")
    set(PXPT_ARCH_BITS 32)
    set(PXPT_ARCH_PPC_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(s390x|s390|S390|IBM System z)$")
    set(PXPT_ARCH_S390X 1)
    set(PXPT_ARCH_NAME "s390x")
    set(PXPT_ARCH_BITS 64)
    set(PXPT_ARCH_S390_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(mips64|mips64el|MIPS64)$")
    set(PXPT_ARCH_MIPS64 1)
    set(PXPT_ARCH_NAME "mips64")
    set(PXPT_ARCH_BITS 64)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "(mips64el|mips64le)")
        set(PXPT_ARCH_MIPS64EL 1)
    endif()
    set(PXPT_ARCH_MIPS_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(mips|mipsel|MIPS)$")
    set(PXPT_ARCH_MIPS32 1)
    set(PXPT_ARCH_NAME "mips32")
    set(PXPT_ARCH_BITS 32)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "(mipsel|mipsle)")
        set(PXPT_ARCH_MIPS32EL 1)
    endif()
    set(PXPT_ARCH_MIPS_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(sparc64|sparcv9|SPARC64)$")
    set(PXPT_ARCH_SPARC64 1)
    set(PXPT_ARCH_NAME "sparc64")
    set(PXPT_ARCH_BITS 64)
    set(PXPT_ARCH_SPARC_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(wasm|WASM|emscripten|Emscripten)$")
    set(PXPT_ARCH_WASM 1)
    set(PXPT_ARCH_NAME "wasm")
    set(PXPT_ARCH_BITS 32)
else()
    set(PXPT_ARCH_NAME "${CMAKE_SYSTEM_PROCESSOR}")
    set(PXPT_ARCH_BITS 0)
    message(WARNING "Unknown CPU architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

message(STATUS "PXPT Racer - Target CPU: ${PXPT_ARCH_NAME} (${PXPT_ARCH_BITS}-bit)")

# ========================================
# Platform detection
# ========================================

if(WIN32)
    set(PXPT_PLATFORM "Windows")
    set(PXPT_PLATFORM_WINDOWS 1)
elseif(APPLE)
    if(IOS OR CMAKE_SYSTEM_NAME STREQUAL "iOS")
        set(PXPT_PLATFORM "iOS")
        set(PXPT_PLATFORM_IOS 1)
    else()
        set(PXPT_PLATFORM "macOS")
        set(PXPT_PLATFORM_MACOS 1)
    endif()
elseif(ANDROID)
    set(PXPT_PLATFORM "Android")
    set(PXPT_PLATFORM_ANDROID 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(PXPT_USE_DRM)
        set(PXPT_PLATFORM "LinuxDRM")
        set(PXPT_PLATFORM_DRM 1)
    elseif(PXPT_IS_CHROMEOS)
        set(PXPT_PLATFORM "ChromeOS")
        set(PXPT_PLATFORM_CHROMEOS 1)
        set(PXPT_PLATFORM_LINUX 1)
    else()
        set(PXPT_PLATFORM "Linux")
        set(PXPT_PLATFORM_LINUX 1)
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    set(PXPT_PLATFORM "FreeBSD")
    set(PXPT_PLATFORM_FREEBSD 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
    set(PXPT_PLATFORM "OpenBSD")
    set(PXPT_PLATFORM_OPENBSD 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
    set(PXPT_PLATFORM "NetBSD")
    set(PXPT_PLATFORM_NETBSD 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "DragonFly")
    set(PXPT_PLATFORM "DragonFly")
    set(PXPT_PLATFORM_DRAGONFLY 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(PXPT_PLATFORM "Web")
    set(PXPT_PLATFORM_WEB 1)
    set(PXPT_USE_EMSCRIPTEN 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "OHOS")
    set(PXPT_PLATFORM "HarmonyOS")
    set(PXPT_PLATFORM_HARMONYOS 1)
else()
    set(PXPT_PLATFORM "POSIX")
    set(PXPT_PLATFORM_POSIX 1)
endif()

message(STATUS "PXPT Racer - Target platform: ${PXPT_PLATFORM}")

# ========================================
# Architecture-specific compile flags
# ========================================

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    if(PXPT_ARCH_X86)
        add_compile_options(-march=i686 -mtune=i686)
    elseif(PXPT_ARCH_X64)
        add_compile_options(-march=x86-64 -mtune=generic)
    elseif(PXPT_ARCH_ARM32)
        if(PXPT_ARCH_ARMV6)
            add_compile_options(-march=armv6zk -mtune=arm1176jzf-s)
        elseif(PXPT_ARCH_ARMV7)
            add_compile_options(-march=armv7-a -mtune=cortex-a7 -mfpu=neon -mfloat-abi=hard)
        else()
            add_compile_options(-march=armv7-a -mtune=cortex-a7)
        endif()
    elseif(PXPT_ARCH_ARM64)
        if(PXPT_ARCH_ARM64E)
            add_compile_options(-march=armv8.3-a -mtune=cortex-a55)
        else()
            add_compile_options(-march=armv8-a -mtune=cortex-a53)
        endif()
    elseif(PXPT_ARCH_RISCV64)
        add_compile_options(-march=rv64gc -mabi=lp64d)
    elseif(PXPT_ARCH_RISCV32)
        add_compile_options(-march=rv32gc -mabi=ilp32d)
    elseif(PXPT_ARCH_PPC64)
        if(PXPT_ARCH_PPC64LE)
            add_compile_options(-march=powerpc64le -mtune=powerpc64le)
        else()
            add_compile_options(-march=powerpc64 -mtune=powerpc64)
        endif()
    elseif(PXPT_ARCH_PPC32)
        add_compile_options(-march=powerpc -mtune=powerpc)
    elseif(PXPT_ARCH_S390X)
        add_compile_options(-march=z10 -mtune=z10)
    elseif(PXPT_ARCH_MIPS64)
        if(PXPT_ARCH_MIPS64EL)
            add_compile_options(-march=mips64el -mtune=mips64el)
        else()
            add_compile_options(-march=mips64 -mtune=mips64)
        endif()
    elseif(PXPT_ARCH_MIPS32)
        if(PXPT_ARCH_MIPS32EL)
            add_compile_options(-march=mips32r5el -mtune=mips32r5el)
        else()
            add_compile_options(-march=mips32r5 -mtune=mips32r5)
        endif()
    elseif(PXPT_ARCH_SPARC64)
        add_compile_options(-march=sparc64 -mtune=sparc64)
    endif()

    if(NOT PXPT_PLATFORM_WINDOWS)
        add_compile_options(-Wall -Wextra -std=c99)
    endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
    if(PXPT_ARCH_X86)
        add_compile_options(/arch:x86)
    elseif(PXPT_ARCH_X64)
        add_compile_options(/arch:x64)
    elseif(PXPT_ARCH_ARM64)
        add_compile_options(/arch:ARM64)
    endif()
endif()

# ========================================
# Include directories
# ========================================

include_directories(
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/client
    ${CMAKE_SOURCE_DIR}/server
    ${CMAKE_SOURCE_DIR}/raylib/src
    ${CMAKE_SOURCE_DIR}/raygui-5.0/src
)

# ========================================
# Raylib static library
# ========================================

option(PXPT_SKIP_RAYLIB_BUILD "Skip building raylib from source (pre-built libraylib.a must exist)" OFF)

set(RAYLIB_LIB ${CMAKE_SOURCE_DIR}/raylib/src/libraylib.a)

if(NOT PXPT_SKIP_RAYLIB_BUILD AND NOT PXPT_USE_EMSCRIPTEN)
    if(NOT EXISTS "${RAYLIB_LIB}")
        message(WARNING "raylib static library not found at ${RAYLIB_LIB}")
        message(STATUS "Attempting to build raylib from source...")
        set(RAYLIB_DIR ${CMAKE_SOURCE_DIR}/raylib)

        set(RAYLIB_MAKE_ARGS "-j4")
        if(CMAKE_C_COMPILER)
            list(APPEND RAYLIB_MAKE_ARGS "CC=${CMAKE_C_COMPILER}")
        endif()
        if(CMAKE_AR)
            list(APPEND RAYLIB_MAKE_ARGS "AR=${CMAKE_AR}")
        endif()
        if(PXPT_PLATFORM_WINDOWS)
            list(APPEND RAYLIB_MAKE_ARGS "PLATFORM_OS=WINDOWS")
        elseif(PXPT_PLATFORM_MACOS)
            list(APPEND RAYLIB_MAKE_ARGS "PLATFORM_OS=OSX")
        elseif(PXPT_PLATFORM_ANDROID)
            list(APPEND RAYLIB_MAKE_ARGS "PLATFORM_ANDROID")
        endif()

        execute_process(
            COMMAND make ${RAYLIB_MAKE_ARGS}
            WORKING_DIRECTORY ${RAYLIB_DIR}
            RESULT_VARIABLE RAYLIB_BUILD_RESULT
        )
        if(NOT RAYLIB_BUILD_RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to build raylib. Check raylib directory.")
        endif()
        if(NOT EXISTS "${RAYLIB_LIB}")
            message(FATAL_ERROR "raylib build did not produce libraylib.a")
        endif()
    endif()
endif()

# ========================================
# Common sources
# ========================================

set(COMMON_SRC common/socket.c)

# ========================================
# Server
# ========================================

add_executable(pxpt-server
    server/main.c
    server/server.c
    ${COMMON_SRC}
)

# ========================================
# Client
# ========================================

if(NOT PXPT_PLATFORM_IOS AND NOT PXPT_PLATFORM_ANDROID)
    add_executable(pxpt-client
        client/main.c
        client/gui.c
        client/game.c
        client/network.c
        client/raygui_impl.c
        ${COMMON_SRC}
    )
endif()

# ========================================
# Web / Emscripten client
# ========================================

if(PXPT_USE_EMSCRIPTEN)
    set(PXPT_WEB_SOURCES
        client/main.c
        client/gui.c
        client/game.c
        client/network.c
        client/raygui_impl.c
        ${COMMON_SRC}
    )

    add_executable(pxpt-client-web ${PXPT_WEB_SOURCES})

    set_target_properties(pxpt-client-web PROPERTIES
        SUFFIX ".html"
        LINK_FLAGS "-s USE_SOCKETS=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=1 -s EXPORTED_RUNTIME_METHODS=['ccallstr'] --preload-file ${CMAKE_SOURCE_DIR}/assets@/assets"
    )
endif()

# ========================================
# Platform-specific linking
# ========================================

if(PXPT_PLATFORM_WINDOWS)
    target_link_libraries(pxpt-server ws2_32)
    if(NOT PXPT_PLATFORM_IOS AND NOT PXPT_PLATFORM_ANDROID)
        target_link_libraries(pxpt-client ${RAYLIB_LIB} ws2_32 winmm)
    endif()

elseif(PXPT_PLATFORM_MACOS)
    if(NOT PXPT_PLATFORM_IOS)
        target_link_libraries(pxpt-client
            ${RAYLIB_LIB}
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
            "-framework OpenGL"
        )
    endif()

elseif(PXPT_PLATFORM_IOS)
    target_link_libraries(pxpt-server pthread)
    target_link_libraries(pxpt-server m)

elseif(PXPT_PLATFORM_ANDROID)
    target_link_libraries(pxpt-server pthread)
    target_link_libraries(pxpt-server m)

elseif(PXPT_PLATFORM_WEB)
    # Web handled above

elseif(PXPT_PLATFORM_CHROMEOS)
    target_link_libraries(pxpt-server pthread m)
    if(NOT PXPT_PLATFORM_IOS AND NOT PXPT_PLATFORM_ANDROID)
        target_link_libraries(pxpt-client ${RAYLIB_LIB} pthread m)
    endif()

elseif(PXPT_PLATFORM_DRM)
    target_link_libraries(pxpt-server pthread m)
    target_link_libraries(pxpt-client ${RAYLIB_LIB} pthread m)
    target_link_libraries(pxpt-client drm EGL GLESv2)

elseif(PXPT_PLATFORM_HARMONYOS)
    target_link_libraries(pxpt-server pthread m)
    target_link_libraries(pxpt-client ${RAYLIB_LIB} pthread m)

elseif(PXPT_PLATFORM_FREEBSD OR PXPT_PLATFORM_OPENBSD OR PXPT_PLATFORM_NETBSD OR PXPT_PLATFORM_DRAGONFLY)
    target_link_libraries(pxpt-server pthread m)
    if(NOT PXPT_PLATFORM_IOS AND NOT PXPT_PLATFORM_ANDROID)
        target_link_libraries(pxpt-client ${RAYLIB_LIB} pthread m)
    endif()

else()
    target_link_libraries(pxpt-server pthread m)
    if(NOT PXPT_PLATFORM_IOS AND NOT PXPT_PLATFORM_ANDROID)
        target_link_libraries(pxpt-client ${RAYLIB_LIB} pthread m)
    endif()
endif()

# ========================================
# Platform definitions
# ========================================

if(PXPT_PLATFORM_WINDOWS)
    target_compile_definitions(pxpt-server PRIVATE PXPT_WINDOWS=1)
    if(NOT PXPT_PLATFORM_IOS AND NOT PXPT_PLATFORM_ANDROID)
        target_compile_definitions(pxpt-client PRIVATE PXPT_WINDOWS=1)
    endif()
elseif(PXPT_PLATFORM_ANDROID)
    target_compile_definitions(pxpt-server PRIVATE PXPT_ANDROID=1)
elseif(PXPT_PLATFORM_IOS)
    target_compile_definitions(pxpt-server PRIVATE PXPT_IOS=1)
elseif(PXPT_PLATFORM_MACOS)
    if(NOT PXPT_PLATFORM_IOS)
        target_compile_definitions(pxpt-client PRIVATE PXPT_MACOS=1)
    endif()
elseif(PXPT_PLATFORM_WEB)
    target_compile_definitions(pxpt-server PRIVATE PXPT_WEB=1)
elseif(PXPT_PLATFORM_FREEBSD)
    target_compile_definitions(pxpt-server PRIVATE PXPT_FREEBSD=1)
    if(NOT PXPT_PLATFORM_IOS AND NOT PXPT_PLATFORM_ANDROID)
        target_compile_definitions(pxpt-client PRIVATE PXPT_FREEBSD=1)
    endif()
elseif(PXPT_PLATFORM_OPENBSD)
    target_compile_definitions(pxpt-server PRIVATE PXPT_OPENBSD=1)
    if(NOT PXPT_PLATFORM_IOS AND NOT PXPT_PLATFORM_ANDROID)
        target_compile_definitions(pxpt-client PRIVATE PXPT_OPENBSD=1)
    endif()
elseif(PXPT_PLATFORM_NETBSD)
    target_compile_definitions(pxpt-server PRIVATE PXPT_NETBSD=1)
elseif(PXPT_PLATFORM_DRAGONFLY)
    target_compile_definitions(pxpt-server PRIVATE PXPT_DRAGONFLY=1)
elseif(PXPT_PLATFORM_HARMONYOS)
    target_compile_definitions(pxpt-server PRIVATE PXPT_HARMONYOS=1)
    target_compile_definitions(pxpt-client PRIVATE PXPT_HARMONYOS=1)
elseif(PXPT_PLATFORM_CHROMEOS)
    target_compile_definitions(pxpt-server PRIVATE PXPT_CHROMEOS=1 PXPT_LINUX=1)
    if(NOT PXPT_PLATFORM_IOS AND NOT PXPT_PLATFORM_ANDROID)
        target_compile_definitions(pxpt-client PRIVATE PXPT_CHROMEOS=1 PXPT_LINUX=1)
    endif()
else()
    target_compile_definitions(pxpt-server PRIVATE PXPT_LINUX=1)
    if(NOT PXPT_PLATFORM_IOS AND NOT PXPT_PLATFORM_ANDROID)
        target_compile_definitions(pxpt-client PRIVATE PXPT_LINUX=1)
    endif()
endif()

# ========================================
# Architecture definitions
# ========================================

foreach(target_name pxpt-server)
    if(PXPT_ARCH_X86)
        target_compile_definitions(${target_name} PRIVATE PXPT_ARCH_X86=1)
    elseif(PXPT_ARCH_X64)
        target_compile_definitions(${target_name} PRIVATE PXPT_ARCH_X64=1)
    elseif(PXPT_ARCH_ARM32)
        target_compile_definitions(${target_name} PRIVATE PXPT_ARCH_ARM32=1)
        if(PXPT_ARCH_ARMV6)
            target_compile_definitions(${target_name} PRIVATE PXPT_ARCH_ARMV6=1)
        elseif(PXPT_ARCH_ARMV7)
            target_compile_definitions(${target_name} PRIVATE PXPT_ARCH_ARMV7=1)
        endif()
    elseif(PXPT_ARCH_ARM64)
        target_compile_definitions(${target_name} PRIVATE PXPT_ARCH_ARM64=1)
        if(PXPT_ARCH_ARM64E)
            target_compile_definitions(${target_name} PRIVATE PXPT_ARCH_ARM64E=1)
        endif()
    elseif(PXPT_ARCH_RISCV64)
        target_compile_definitions(${target_name} PRIVATE PXPT_ARCH_RISCV64=1)
    elseif(PXPT_ARCH_RISCV32)
        target_compile_definitions(${target_name} PRIVATE PXPT_ARCH_RISCV32=1)
    elseif(PXPT_ARCH_PPC64)
        target_compile_definitions(${target_name} PRIVATE PXPT_ARCH_PPC64=1)
        if(PXPT_ARCH_PPC64LE)
            target_compile_definitions(${target_name} PRIVATE PXPT_ARCH_PPC64LE=1)
        endif()
    elseif(PXPT_ARCH_PPC32)
        target_compile_definitions(${target_name} PRIVATE PXPT_ARCH_PPC32=1)
    elseif(PXPT_ARCH_S390X)
        target_compile_definitions(${target_name} PRIVATE PXPT_ARCH_S390X=1)
    elseif(PXPT_ARCH_MIPS64)
        target_compile_definitions(${target_name} PRIVATE PXPT_ARCH_MIPS64=1)
        if(PXPT_ARCH_MIPS64EL)
            target_compile_definitions(${target_name} PRIVATE PXPT_ARCH_MIPS64EL=1)
        endif()
    elseif(PXPT_ARCH_MIPS32)
        target_compile_definitions(${target_name} PRIVATE PXPT_ARCH_MIPS32=1)
        if(PXPT_ARCH_MIPS32EL)
            target_compile_definitions(${target_name} PRIVATE PXPT_ARCH_MIPS32EL=1)
        endif()
    elseif(PXPT_ARCH_SPARC64)
        target_compile_definitions(${target_name} PRIVATE PXPT_ARCH_SPARC64=1)
    elseif(PXPT_ARCH_WASM)
        target_compile_definitions(${target_name} PRIVATE PXPT_ARCH_WASM=1)
    endif()
endforeach()

if(NOT PXPT_PLATFORM_IOS AND NOT PXPT_PLATFORM_ANDROID)
    if(TARGET pxpt-client)
        if(PXPT_ARCH_X86)
            target_compile_definitions(pxpt-client PRIVATE PXPT_ARCH_X86=1)
        elseif(PXPT_ARCH_X64)
            target_compile_definitions(pxpt-client PRIVATE PXPT_ARCH_X64=1)
        elseif(PXPT_ARCH_ARM32)
            target_compile_definitions(pxpt-client PRIVATE PXPT_ARCH_ARM32=1)
            if(PXPT_ARCH_ARMV6)
                target_compile_definitions(pxpt-client PRIVATE PXPT_ARCH_ARMV6=1)
            elseif(PXPT_ARCH_ARMV7)
                target_compile_definitions(pxpt-client PRIVATE PXPT_ARCH_ARMV7=1)
            endif()
        elseif(PXPT_ARCH_ARM64)
            target_compile_definitions(pxpt-client PRIVATE PXPT_ARCH_ARM64=1)
            if(PXPT_ARCH_ARM64E)
                target_compile_definitions(pxpt-client PRIVATE PXPT_ARCH_ARM64E=1)
            endif()
        elseif(PXPT_ARCH_RISCV64)
            target_compile_definitions(pxpt-client PRIVATE PXPT_ARCH_RISCV64=1)
        elseif(PXPT_ARCH_RISCV32)
            target_compile_definitions(pxpt-client PRIVATE PXPT_ARCH_RISCV32=1)
        elseif(PXPT_ARCH_PPC64)
            target_compile_definitions(pxpt-client PRIVATE PXPT_ARCH_PPC64=1)
            if(PXPT_ARCH_PPC64LE)
                target_compile_definitions(pxpt-client PRIVATE PXPT_ARCH_PPC64LE=1)
            endif()
        elseif(PXPT_ARCH_PPC32)
            target_compile_definitions(pxpt-client PRIVATE PXPT_ARCH_PPC32=1)
        elseif(PXPT_ARCH_S390X)
            target_compile_definitions(pxpt-client PRIVATE PXPT_ARCH_S390X=1)
        elseif(PXPT_ARCH_MIPS64)
            target_compile_definitions(pxpt-client PRIVATE PXPT_ARCH_MIPS64=1)
            if(PXPT_ARCH_MIPS64EL)
                target_compile_definitions(pxpt-client PRIVATE PXPT_ARCH_MIPS64EL=1)
            endif()
        elseif(PXPT_ARCH_MIPS32)
            target_compile_definitions(pxpt-client PRIVATE PXPT_ARCH_MIPS32=1)
            if(PXPT_ARCH_MIPS32EL)
                target_compile_definitions(pxpt-client PRIVATE PXPT_ARCH_MIPS32EL=1)
            endif()
        elseif(PXPT_ARCH_SPARC64)
            target_compile_definitions(pxpt-client PRIVATE PXPT_ARCH_SPARC64=1)
        elseif(PXPT_ARCH_WASM)
            target_compile_definitions(pxpt-client PRIVATE PXPT_ARCH_WASM=1)
        endif()
    endif()
endif()

# ========================================
# Architecture-specific output naming
# ========================================

set(PXPT_OUTPUT_SUFFIX "")
if(PXPT_PLATFORM_WINDOWS)
    if(PXPT_ARCH_X86)
        set(PXPT_OUTPUT_SUFFIX "-x86")
    elseif(PXPT_ARCH_X64)
        set(PXPT_OUTPUT_SUFFIX "-x64")
    elseif(PXPT_ARCH_ARM32)
        set(PXPT_OUTPUT_SUFFIX "-arm32")
    elseif(PXPT_ARCH_ARM64)
        set(PXPT_OUTPUT_SUFFIX "-arm64")
    endif()
elseif(PXPT_PLATFORM_LINUX OR PXPT_PLATFORM_CHROMEOS)
    if(PXPT_ARCH_X86)
        set(PXPT_OUTPUT_SUFFIX "-x86")
    elseif(PXPT_ARCH_X64)
        set(PXPT_OUTPUT_SUFFIX "-x64")
    elseif(PXPT_ARCH_ARM32)
        set(PXPT_OUTPUT_SUFFIX "-arm32")
    elseif(PXPT_ARCH_ARM64)
        set(PXPT_OUTPUT_SUFFIX "-arm64")
    elseif(PXPT_ARCH_RISCV64)
        set(PXPT_OUTPUT_SUFFIX "-riscv64")
    elseif(PXPT_ARCH_RISCV32)
        set(PXPT_OUTPUT_SUFFIX "-riscv32")
    elseif(PXPT_ARCH_PPC64)
        set(PXPT_OUTPUT_SUFFIX "-ppc64")
    elseif(PXPT_ARCH_PPC32)
        set(PXPT_OUTPUT_SUFFIX "-ppc32")
    elseif(PXPT_ARCH_S390X)
        set(PXPT_OUTPUT_SUFFIX "-s390x")
    elseif(PXPT_ARCH_MIPS64)
        set(PXPT_OUTPUT_SUFFIX "-mips64")
    elseif(PXPT_ARCH_MIPS32)
        set(PXPT_OUTPUT_SUFFIX "-mips32")
    elseif(PXPT_ARCH_SPARC64)
        set(PXPT_OUTPUT_SUFFIX "-sparc64")
    endif()
elseif(PXPT_PLATFORM_MACOS)
    if(PXPT_ARCH_X64)
        set(PXPT_OUTPUT_SUFFIX "-x64")
    elseif(PXPT_ARCH_ARM64)
        set(PXPT_OUTPUT_SUFFIX "-arm64")
    endif()
elseif(PXPT_PLATFORM_FREEBSD OR PXPT_PLATFORM_OPENBSD OR PXPT_PLATFORM_NETBSD OR PXPT_PLATFORM_DRAGONFLY)
    if(PXPT_ARCH_X86)
        set(PXPT_OUTPUT_SUFFIX "-x86")
    elseif(PXPT_ARCH_X64)
        set(PXPT_OUTPUT_SUFFIX "-x64")
    elseif(PXPT_ARCH_ARM64)
        set(PXPT_OUTPUT_SUFFIX "-arm64")
    endif()
elseif(PXPT_PLATFORM_ANDROID)
    if(PXPT_ARCH_ARM64)
        set(PXPT_OUTPUT_SUFFIX "-arm64")
    elseif(PXPT_ARCH_ARM32)
        set(PXPT_OUTPUT_SUFFIX "-arm32")
    elseif(PXPT_ARCH_X86)
        set(PXPT_OUTPUT_SUFFIX "-x86")
    elseif(PXPT_ARCH_X64)
        set(PXPT_OUTPUT_SUFFIX "-x64")
    endif()
elseif(PXPT_PLATFORM_IOS)
    if(PXPT_ARCH_ARM64)
        set(PXPT_OUTPUT_SUFFIX "-arm64")
    elseif(PXPT_ARCH_X64)
        set(PXPT_OUTPUT_SUFFIX "-x64-sim")
    endif()
elseif(PXPT_PLATFORM_HARMONYOS)
    if(PXPT_ARCH_ARM64)
        set(PXPT_OUTPUT_SUFFIX "-arm64")
    endif()
elseif(PXPT_PLATFORM_WEB)
    set(PXPT_OUTPUT_SUFFIX "-wasm")
endif()

# ========================================
# Install rules
# ========================================

set_target_properties(pxpt-server PROPERTIES
    OUTPUT_NAME "pxpt-server${PXPT_OUTPUT_SUFFIX}"
)

install(TARGETS pxpt-server DESTINATION bin)
if(NOT PXPT_PLATFORM_IOS AND NOT PXPT_PLATFORM_ANDROID AND NOT PXPT_PLATFORM_WEB)
    set_target_properties(pxpt-client PROPERTIES
        OUTPUT_NAME "pxpt-client${PXPT_OUTPUT_SUFFIX}"
    )
    install(TARGETS pxpt-client DESTINATION bin)
endif()