cmake_minimum_required(VERSION 3.15)

project(
    CGAME_Racer
    VERSION 1.0
    LANGUAGES C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ========================================
# CPU Architecture detection
# ========================================

set(CGAME_ARCH "${CMAKE_SYSTEM_PROCESSOR}")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64|AMD x86-64)$")
    set(CGAME_ARCH_X64 1)
    set(CGAME_ARCH_NAME "x64")
    set(CGAME_ARCH_BITS 64)
    set(CGAME_ARCH_X86_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i[3-6]86|x86)$")
    set(CGAME_ARCH_X86 1)
    set(CGAME_ARCH_NAME "x86")
    set(CGAME_ARCH_BITS 32)
    set(CGAME_ARCH_X86_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64|armv[78].*|arm64e)$")
    set(CGAME_ARCH_ARM64 1)
    set(CGAME_ARCH_NAME "arm64")
    set(CGAME_ARCH_BITS 64)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64e")
        set(CGAME_ARCH_ARM64E 1)
    endif()
    set(CGAME_ARCH_ARM_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm|armv[67].*|ARM|ARMv[67].*)$")
    set(CGAME_ARCH_ARM32 1)
    set(CGAME_ARCH_NAME "arm32")
    set(CGAME_ARCH_BITS 32)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "armv6")
        set(CGAME_ARCH_ARMV6 1)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "armv7")
        set(CGAME_ARCH_ARMV7 1)
    endif()
    set(CGAME_ARCH_ARM_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(riscv64|riscv|RISCV64|RISCV)$")
    set(CGAME_ARCH_RISCV64 1)
    set(CGAME_ARCH_NAME "riscv64")
    set(CGAME_ARCH_BITS 64)
    set(CGAME_ARCH_RISCV_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(riscv32|RISCV32)$")
    set(CGAME_ARCH_RISCV32 1)
    set(CGAME_ARCH_NAME "riscv32")
    set(CGAME_ARCH_BITS 32)
    set(CGAME_ARCH_RISCV_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(ppc64|powerpc64|PPC64|ppc64le|powerpc64le)$")
    set(CGAME_ARCH_PPC64 1)
    set(CGAME_ARCH_NAME "ppc64")
    set(CGAME_ARCH_BITS 64)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "(ppc64le|powerpc64le|ppc64el)")
        set(CGAME_ARCH_PPC64LE 1)
    endif()
    set(CGAME_ARCH_PPC_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(ppc|powerpc|PPC)$")
    set(CGAME_ARCH_PPC32 1)
    set(CGAME_ARCH_NAME "ppc32")
    set(CGAME_ARCH_BITS 32)
    set(CGAME_ARCH_PPC_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(s390x|s390|S390|IBM System z)$")
    set(CGAME_ARCH_S390X 1)
    set(CGAME_ARCH_NAME "s390x")
    set(CGAME_ARCH_BITS 64)
    set(CGAME_ARCH_S390_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(mips64|mips64el|MIPS64)$")
    set(CGAME_ARCH_MIPS64 1)
    set(CGAME_ARCH_NAME "mips64")
    set(CGAME_ARCH_BITS 64)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "(mips64el|mips64le)")
        set(CGAME_ARCH_MIPS64EL 1)
    endif()
    set(CGAME_ARCH_MIPS_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(mips|mipsel|MIPS)$")
    set(CGAME_ARCH_MIPS32 1)
    set(CGAME_ARCH_NAME "mips32")
    set(CGAME_ARCH_BITS 32)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "(mipsel|mipsle)")
        set(CGAME_ARCH_MIPS32EL 1)
    endif()
    set(CGAME_ARCH_MIPS_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(sparc64|sparcv9|SPARC64)$")
    set(CGAME_ARCH_SPARC64 1)
    set(CGAME_ARCH_NAME "sparc64")
    set(CGAME_ARCH_BITS 64)
    set(CGAME_ARCH_SPARC_LIKE 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(wasm|WASM|emscripten|Emscripten)$")
    set(CGAME_ARCH_WASM 1)
    set(CGAME_ARCH_NAME "wasm")
    set(CGAME_ARCH_BITS 32)
else()
    set(CGAME_ARCH_NAME "${CMAKE_SYSTEM_PROCESSOR}")
    set(CGAME_ARCH_BITS 0)
    message(WARNING "Unknown CPU architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

message(STATUS "CGame Racer - Target CPU: ${CGAME_ARCH_NAME} (${CGAME_ARCH_BITS}-bit)")

# ========================================
# Platform detection
# ========================================

if(WIN32)
    set(CGAME_PLATFORM "Windows")
    set(CGAME_PLATFORM_WINDOWS 1)
elseif(APPLE)
    if(IOS OR CMAKE_SYSTEM_NAME STREQUAL "iOS")
        set(CGAME_PLATFORM "iOS")
        set(CGAME_PLATFORM_IOS 1)
    else()
        set(CGAME_PLATFORM "macOS")
        set(CGAME_PLATFORM_MACOS 1)
    endif()
elseif(ANDROID)
    set(CGAME_PLATFORM "Android")
    set(CGAME_PLATFORM_ANDROID 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(CGAME_USE_DRM)
        set(CGAME_PLATFORM "LinuxDRM")
        set(CGAME_PLATFORM_DRM 1)
    elseif(CGAME_IS_CHROMEOS)
        set(CGAME_PLATFORM "ChromeOS")
        set(CGAME_PLATFORM_CHROMEOS 1)
        set(CGAME_PLATFORM_LINUX 1)
    else()
        set(CGAME_PLATFORM "Linux")
        set(CGAME_PLATFORM_LINUX 1)
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    set(CGAME_PLATFORM "FreeBSD")
    set(CGAME_PLATFORM_FREEBSD 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
    set(CGAME_PLATFORM "OpenBSD")
    set(CGAME_PLATFORM_OPENBSD 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
    set(CGAME_PLATFORM "NetBSD")
    set(CGAME_PLATFORM_NETBSD 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "DragonFly")
    set(CGAME_PLATFORM "DragonFly")
    set(CGAME_PLATFORM_DRAGONFLY 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(CGAME_PLATFORM "Web")
    set(CGAME_PLATFORM_WEB 1)
    set(CGAME_USE_EMSCRIPTEN 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "OHOS")
    set(CGAME_PLATFORM "HarmonyOS")
    set(CGAME_PLATFORM_HARMONYOS 1)
else()
    set(CGAME_PLATFORM "POSIX")
    set(CGAME_PLATFORM_POSIX 1)
endif()

message(STATUS "CGame Racer - Target platform: ${CGAME_PLATFORM}")

# ========================================
# Architecture-specific compile flags
# ========================================

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    if(CGAME_ARCH_X86)
        add_compile_options(-march=i686 -mtune=i686)
    elseif(CGAME_ARCH_X64)
        add_compile_options(-march=x86-64 -mtune=generic)
    elseif(CGAME_ARCH_ARM32)
        if(CGAME_ARCH_ARMV6)
            add_compile_options(-march=armv6zk -mtune=arm1176jzf-s)
        elseif(CGAME_ARCH_ARMV7)
            add_compile_options(-march=armv7-a -mtune=cortex-a7 -mfpu=neon -mfloat-abi=hard)
        else()
            add_compile_options(-march=armv7-a -mtune=cortex-a7)
        endif()
    elseif(CGAME_ARCH_ARM64)
        if(CGAME_ARCH_ARM64E)
            add_compile_options(-march=armv8.3-a -mtune=cortex-a55)
        else()
            add_compile_options(-march=armv8-a -mtune=cortex-a53)
        endif()
    elseif(CGAME_ARCH_RISCV64)
        add_compile_options(-march=rv64gc -mabi=lp64d)
    elseif(CGAME_ARCH_RISCV32)
        add_compile_options(-march=rv32gc -mabi=ilp32d)
    elseif(CGAME_ARCH_PPC64)
        if(CGAME_ARCH_PPC64LE)
            add_compile_options(-march=powerpc64le -mtune=powerpc64le)
        else()
            add_compile_options(-march=powerpc64 -mtune=powerpc64)
        endif()
    elseif(CGAME_ARCH_PPC32)
        add_compile_options(-march=powerpc -mtune=powerpc)
    elseif(CGAME_ARCH_S390X)
        add_compile_options(-march=z10 -mtune=z10)
    elseif(CGAME_ARCH_MIPS64)
        if(CGAME_ARCH_MIPS64EL)
            add_compile_options(-march=mips64el -mtune=mips64el)
        else()
            add_compile_options(-march=mips64 -mtune=mips64)
        endif()
    elseif(CGAME_ARCH_MIPS32)
        if(CGAME_ARCH_MIPS32EL)
            add_compile_options(-march=mips32r5el -mtune=mips32r5el)
        else()
            add_compile_options(-march=mips32r5 -mtune=mips32r5)
        endif()
    elseif(CGAME_ARCH_SPARC64)
        add_compile_options(-march=sparc64 -mtune=sparc64)
    endif()

    if(NOT CGAME_PLATFORM_WINDOWS)
        add_compile_options(-Wall -Wextra -std=c99)
    endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
    if(CGAME_ARCH_X86)
        add_compile_options(/arch:x86)
    elseif(CGAME_ARCH_X64)
        add_compile_options(/arch:x64)
    elseif(CGAME_ARCH_ARM64)
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

option(CGAME_SKIP_RAYLIB_BUILD "Skip building raylib from source (pre-built libraylib.a must exist)" OFF)

set(RAYLIB_LIB ${CMAKE_SOURCE_DIR}/raylib/src/libraylib.a)

if(NOT CGAME_SKIP_RAYLIB_BUILD AND NOT CGAME_USE_EMSCRIPTEN)
    if(NOT EXISTS "${RAYLIB_LIB}")
        message(WARNING "raylib static library not found at ${RAYLIB_LIB}")
        message(STATUS "Attempting to build raylib from source...")
        set(RAYLIB_DIR ${CMAKE_SOURCE_DIR}/raylib/src)

        set(RAYLIB_MAKE_ARGS "-j4")
        if(CMAKE_C_COMPILER)
            list(APPEND RAYLIB_MAKE_ARGS "CC=${CMAKE_C_COMPILER}")
        endif()
        if(CMAKE_AR)
            list(APPEND RAYLIB_MAKE_ARGS "AR=${CMAKE_AR}")
        endif()
        if(CGAME_PLATFORM_WINDOWS)
            list(APPEND RAYLIB_MAKE_ARGS "PLATFORM_OS=WINDOWS")
        elseif(CGAME_PLATFORM_MACOS)
            list(APPEND RAYLIB_MAKE_ARGS "PLATFORM_OS=OSX")
        elseif(CGAME_PLATFORM_ANDROID)
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

add_executable(c-game-server
    server/main.c
    server/server.c
    ${COMMON_SRC}
)

# ========================================
# Client
# ========================================

if(NOT CGAME_PLATFORM_IOS AND NOT CGAME_PLATFORM_ANDROID)
    add_executable(c-game-client
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

if(CGAME_USE_EMSCRIPTEN)
    set(CGAME_WEB_SOURCES
        client/main.c
        client/gui.c
        client/game.c
        client/network.c
        client/raygui_impl.c
        ${COMMON_SRC}
    )

    add_executable(c-game-client-web ${CGAME_WEB_SOURCES})

    set_target_properties(c-game-client-web PROPERTIES
        SUFFIX ".html"
        LINK_FLAGS "-s USE_SOCKETS=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=1 -s EXPORTED_RUNTIME_METHODS=['ccallstr'] --preload-file ${CMAKE_SOURCE_DIR}/assets@/assets"
    )
endif()

# ========================================
# Terminal hiding (client only)
# ========================================

if(NOT CGAME_PLATFORM_IOS AND NOT CGAME_PLATFORM_ANDROID AND NOT CGAME_PLATFORM_WEB)
    if(TARGET c-game-client)
        if(CGAME_PLATFORM_WINDOWS)
            set_target_properties(c-game-client PROPERTIES WIN32_EXECUTABLE TRUE)
            message(STATUS "Terminal hiding: WIN32_EXECUTABLE enabled for Windows client")

        elseif(CGAME_PLATFORM_MACOS)
            set(CGAME_PLIST_PATH "${CMAKE_BINARY_DIR}/c-game-info.plist")
            file(WRITE "${CGAME_PLIST_PATH}"
                "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
                "<plist version=\"1.0\">\n"
                "<dict>\n"
                "  <key>LSUIElement</key>\n"
                "  <true/>\n"
                "</dict>\n"
                "</plist>\n"
            )
            target_link_options(c-game-client PRIVATE
                "-Wl,-sectcreate,__TEXT,__info_plist,${CGAME_PLIST_PATH}"
            )
            message(STATUS "Terminal hiding: LSUIElement plist embedded for macOS client")

            if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
                add_custom_command(TARGET c-game-client POST_BUILD
                    COMMAND codesign -s - --force $<TARGET_FILE:c-game-client> 2>/dev/null || true
                    COMMENT "Codesigning macOS client for LSUIElement"
                )
            endif()

        else()
            message(STATUS "Terminal hiding: constructor-based daemon mode enabled for client")
        endif()
    endif()
endif()

# ========================================
# Platform-specific linking
# ========================================

if(CGAME_PLATFORM_WINDOWS)
    target_link_libraries(c-game-server ws2_32)
    if(NOT CGAME_PLATFORM_IOS AND NOT CGAME_PLATFORM_ANDROID)
        target_link_libraries(c-game-client ${RAYLIB_LIB} ws2_32 winmm)
    endif()

elseif(CGAME_PLATFORM_MACOS)
    if(NOT CGAME_PLATFORM_IOS)
        target_link_libraries(c-game-client
            ${RAYLIB_LIB}
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
            "-framework OpenGL"
        )
    endif()

elseif(CGAME_PLATFORM_IOS)
    target_link_libraries(c-game-server pthread)
    target_link_libraries(c-game-server m)

elseif(CGAME_PLATFORM_ANDROID)
    target_link_libraries(c-game-server pthread)
    target_link_libraries(c-game-server m)

elseif(CGAME_PLATFORM_WEB)
    # Web handled above

elseif(CGAME_PLATFORM_CHROMEOS)
    target_link_libraries(c-game-server pthread m)
    if(NOT CGAME_PLATFORM_IOS AND NOT CGAME_PLATFORM_ANDROID)
        target_link_libraries(c-game-client ${RAYLIB_LIB} pthread m)
    endif()

elseif(CGAME_PLATFORM_DRM)
    target_link_libraries(c-game-server pthread m)
    target_link_libraries(c-game-client ${RAYLIB_LIB} pthread m)
    target_link_libraries(c-game-client drm EGL GLESv2)

elseif(CGAME_PLATFORM_HARMONYOS)
    target_link_libraries(c-game-server pthread m)
    target_link_libraries(c-game-client ${RAYLIB_LIB} pthread m)

elseif(CGAME_PLATFORM_FREEBSD OR CGAME_PLATFORM_OPENBSD OR CGAME_PLATFORM_NETBSD OR CGAME_PLATFORM_DRAGONFLY)
    target_link_libraries(c-game-server pthread m)
    if(NOT CGAME_PLATFORM_IOS AND NOT CGAME_PLATFORM_ANDROID)
        target_link_libraries(c-game-client ${RAYLIB_LIB} pthread m)
    endif()

else()
    target_link_libraries(c-game-server pthread m)
    if(NOT CGAME_PLATFORM_IOS AND NOT CGAME_PLATFORM_ANDROID)
        target_link_libraries(c-game-client ${RAYLIB_LIB} pthread m)
    endif()
endif()

# ========================================
# Platform definitions
# ========================================

if(CGAME_PLATFORM_WINDOWS)
    target_compile_definitions(c-game-server PRIVATE CGAME_WINDOWS=1)
    if(NOT CGAME_PLATFORM_IOS AND NOT CGAME_PLATFORM_ANDROID)
        target_compile_definitions(c-game-client PRIVATE CGAME_WINDOWS=1)
    endif()
elseif(CGAME_PLATFORM_ANDROID)
    target_compile_definitions(c-game-server PRIVATE CGAME_ANDROID=1)
elseif(CGAME_PLATFORM_IOS)
    target_compile_definitions(c-game-server PRIVATE CGAME_IOS=1)
elseif(CGAME_PLATFORM_MACOS)
    if(NOT CGAME_PLATFORM_IOS)
        target_compile_definitions(c-game-client PRIVATE CGAME_MACOS=1)
    endif()
elseif(CGAME_PLATFORM_WEB)
    target_compile_definitions(c-game-server PRIVATE CGAME_WEB=1)
elseif(CGAME_PLATFORM_FREEBSD)
    target_compile_definitions(c-game-server PRIVATE CGAME_FREEBSD=1)
    if(NOT CGAME_PLATFORM_IOS AND NOT CGAME_PLATFORM_ANDROID)
        target_compile_definitions(c-game-client PRIVATE CGAME_FREEBSD=1)
    endif()
elseif(CGAME_PLATFORM_OPENBSD)
    target_compile_definitions(c-game-server PRIVATE CGAME_OPENBSD=1)
    if(NOT CGAME_PLATFORM_IOS AND NOT CGAME_PLATFORM_ANDROID)
        target_compile_definitions(c-game-client PRIVATE CGAME_OPENBSD=1)
    endif()
elseif(CGAME_PLATFORM_NETBSD)
    target_compile_definitions(c-game-server PRIVATE CGAME_NETBSD=1)
elseif(CGAME_PLATFORM_DRAGONFLY)
    target_compile_definitions(c-game-server PRIVATE CGAME_DRAGONFLY=1)
elseif(CGAME_PLATFORM_HARMONYOS)
    target_compile_definitions(c-game-server PRIVATE CGAME_HARMONYOS=1)
    target_compile_definitions(c-game-client PRIVATE CGAME_HARMONYOS=1)
elseif(CGAME_PLATFORM_CHROMEOS)
    target_compile_definitions(c-game-server PRIVATE CGAME_CHROMEOS=1 CGAME_LINUX=1)
    if(NOT CGAME_PLATFORM_IOS AND NOT CGAME_PLATFORM_ANDROID)
        target_compile_definitions(c-game-client PRIVATE CGAME_CHROMEOS=1 CGAME_LINUX=1)
    endif()
else()
    target_compile_definitions(c-game-server PRIVATE CGAME_LINUX=1)
    if(NOT CGAME_PLATFORM_IOS AND NOT CGAME_PLATFORM_ANDROID)
        target_compile_definitions(c-game-client PRIVATE CGAME_LINUX=1)
    endif()
endif()

# ========================================
# Architecture definitions
# ========================================

foreach(target_name c-game-server)
    if(CGAME_ARCH_X86)
        target_compile_definitions(${target_name} PRIVATE CGAME_ARCH_X86=1)
    elseif(CGAME_ARCH_X64)
        target_compile_definitions(${target_name} PRIVATE CGAME_ARCH_X64=1)
    elseif(CGAME_ARCH_ARM32)
        target_compile_definitions(${target_name} PRIVATE CGAME_ARCH_ARM32=1)
        if(CGAME_ARCH_ARMV6)
            target_compile_definitions(${target_name} PRIVATE CGAME_ARCH_ARMV6=1)
        elseif(CGAME_ARCH_ARMV7)
            target_compile_definitions(${target_name} PRIVATE CGAME_ARCH_ARMV7=1)
        endif()
    elseif(CGAME_ARCH_ARM64)
        target_compile_definitions(${target_name} PRIVATE CGAME_ARCH_ARM64=1)
        if(CGAME_ARCH_ARM64E)
            target_compile_definitions(${target_name} PRIVATE CGAME_ARCH_ARM64E=1)
        endif()
    elseif(CGAME_ARCH_RISCV64)
        target_compile_definitions(${target_name} PRIVATE CGAME_ARCH_RISCV64=1)
    elseif(CGAME_ARCH_RISCV32)
        target_compile_definitions(${target_name} PRIVATE CGAME_ARCH_RISCV32=1)
    elseif(CGAME_ARCH_PPC64)
        target_compile_definitions(${target_name} PRIVATE CGAME_ARCH_PPC64=1)
        if(CGAME_ARCH_PPC64LE)
            target_compile_definitions(${target_name} PRIVATE CGAME_ARCH_PPC64LE=1)
        endif()
    elseif(CGAME_ARCH_PPC32)
        target_compile_definitions(${target_name} PRIVATE CGAME_ARCH_PPC32=1)
    elseif(CGAME_ARCH_S390X)
        target_compile_definitions(${target_name} PRIVATE CGAME_ARCH_S390X=1)
    elseif(CGAME_ARCH_MIPS64)
        target_compile_definitions(${target_name} PRIVATE CGAME_ARCH_MIPS64=1)
        if(CGAME_ARCH_MIPS64EL)
            target_compile_definitions(${target_name} PRIVATE CGAME_ARCH_MIPS64EL=1)
        endif()
    elseif(CGAME_ARCH_MIPS32)
        target_compile_definitions(${target_name} PRIVATE CGAME_ARCH_MIPS32=1)
        if(CGAME_ARCH_MIPS32EL)
            target_compile_definitions(${target_name} PRIVATE CGAME_ARCH_MIPS32EL=1)
        endif()
    elseif(CGAME_ARCH_SPARC64)
        target_compile_definitions(${target_name} PRIVATE CGAME_ARCH_SPARC64=1)
    elseif(CGAME_ARCH_WASM)
        target_compile_definitions(${target_name} PRIVATE CGAME_ARCH_WASM=1)
    endif()
endforeach()

if(NOT CGAME_PLATFORM_IOS AND NOT CGAME_PLATFORM_ANDROID)
    if(TARGET c-game-client)
        if(CGAME_ARCH_X86)
            target_compile_definitions(c-game-client PRIVATE CGAME_ARCH_X86=1)
        elseif(CGAME_ARCH_X64)
            target_compile_definitions(c-game-client PRIVATE CGAME_ARCH_X64=1)
        elseif(CGAME_ARCH_ARM32)
            target_compile_definitions(c-game-client PRIVATE CGAME_ARCH_ARM32=1)
            if(CGAME_ARCH_ARMV6)
                target_compile_definitions(c-game-client PRIVATE CGAME_ARCH_ARMV6=1)
            elseif(CGAME_ARCH_ARMV7)
                target_compile_definitions(c-game-client PRIVATE CGAME_ARCH_ARMV7=1)
            endif()
        elseif(CGAME_ARCH_ARM64)
            target_compile_definitions(c-game-client PRIVATE CGAME_ARCH_ARM64=1)
            if(CGAME_ARCH_ARM64E)
                target_compile_definitions(c-game-client PRIVATE CGAME_ARCH_ARM64E=1)
            endif()
        elseif(CGAME_ARCH_RISCV64)
            target_compile_definitions(c-game-client PRIVATE CGAME_ARCH_RISCV64=1)
        elseif(CGAME_ARCH_RISCV32)
            target_compile_definitions(c-game-client PRIVATE CGAME_ARCH_RISCV32=1)
        elseif(CGAME_ARCH_PPC64)
            target_compile_definitions(c-game-client PRIVATE CGAME_ARCH_PPC64=1)
            if(CGAME_ARCH_PPC64LE)
                target_compile_definitions(c-game-client PRIVATE CGAME_ARCH_PPC64LE=1)
            endif()
        elseif(CGAME_ARCH_PPC32)
            target_compile_definitions(c-game-client PRIVATE CGAME_ARCH_PPC32=1)
        elseif(CGAME_ARCH_S390X)
            target_compile_definitions(c-game-client PRIVATE CGAME_ARCH_S390X=1)
        elseif(CGAME_ARCH_MIPS64)
            target_compile_definitions(c-game-client PRIVATE CGAME_ARCH_MIPS64=1)
            if(CGAME_ARCH_MIPS64EL)
                target_compile_definitions(c-game-client PRIVATE CGAME_ARCH_MIPS64EL=1)
            endif()
        elseif(CGAME_ARCH_MIPS32)
            target_compile_definitions(c-game-client PRIVATE CGAME_ARCH_MIPS32=1)
            if(CGAME_ARCH_MIPS32EL)
                target_compile_definitions(c-game-client PRIVATE CGAME_ARCH_MIPS32EL=1)
            endif()
        elseif(CGAME_ARCH_SPARC64)
            target_compile_definitions(c-game-client PRIVATE CGAME_ARCH_SPARC64=1)
        elseif(CGAME_ARCH_WASM)
            target_compile_definitions(c-game-client PRIVATE CGAME_ARCH_WASM=1)
        endif()
    endif()
endif()

# ========================================
# Architecture-specific output naming
# ========================================

set(CGAME_OUTPUT_SUFFIX "")
if(CGAME_PLATFORM_WINDOWS)
    if(CGAME_ARCH_X86)
        set(CGAME_OUTPUT_SUFFIX "-x86")
    elseif(CGAME_ARCH_X64)
        set(CGAME_OUTPUT_SUFFIX "-x64")
    elseif(CGAME_ARCH_ARM32)
        set(CGAME_OUTPUT_SUFFIX "-arm32")
    elseif(CGAME_ARCH_ARM64)
        set(CGAME_OUTPUT_SUFFIX "-arm64")
    endif()
elseif(CGAME_PLATFORM_LINUX OR CGAME_PLATFORM_CHROMEOS)
    if(CGAME_ARCH_X86)
        set(CGAME_OUTPUT_SUFFIX "-x86")
    elseif(CGAME_ARCH_X64)
        set(CGAME_OUTPUT_SUFFIX "-x64")
    elseif(CGAME_ARCH_ARM32)
        set(CGAME_OUTPUT_SUFFIX "-arm32")
    elseif(CGAME_ARCH_ARM64)
        set(CGAME_OUTPUT_SUFFIX "-arm64")
    elseif(CGAME_ARCH_RISCV64)
        set(CGAME_OUTPUT_SUFFIX "-riscv64")
    elseif(CGAME_ARCH_RISCV32)
        set(CGAME_OUTPUT_SUFFIX "-riscv32")
    elseif(CGAME_ARCH_PPC64)
        set(CGAME_OUTPUT_SUFFIX "-ppc64")
    elseif(CGAME_ARCH_PPC32)
        set(CGAME_OUTPUT_SUFFIX "-ppc32")
    elseif(CGAME_ARCH_S390X)
        set(CGAME_OUTPUT_SUFFIX "-s390x")
    elseif(CGAME_ARCH_MIPS64)
        set(CGAME_OUTPUT_SUFFIX "-mips64")
    elseif(CGAME_ARCH_MIPS32)
        set(CGAME_OUTPUT_SUFFIX "-mips32")
    elseif(CGAME_ARCH_SPARC64)
        set(CGAME_OUTPUT_SUFFIX "-sparc64")
    endif()
elseif(CGAME_PLATFORM_MACOS)
    if(CGAME_ARCH_X64)
        set(CGAME_OUTPUT_SUFFIX "-x64")
    elseif(CGAME_ARCH_ARM64)
        set(CGAME_OUTPUT_SUFFIX "-arm64")
    endif()
elseif(CGAME_PLATFORM_FREEBSD OR CGAME_PLATFORM_OPENBSD OR CGAME_PLATFORM_NETBSD OR CGAME_PLATFORM_DRAGONFLY)
    if(CGAME_ARCH_X86)
        set(CGAME_OUTPUT_SUFFIX "-x86")
    elseif(CGAME_ARCH_X64)
        set(CGAME_OUTPUT_SUFFIX "-x64")
    elseif(CGAME_ARCH_ARM64)
        set(CGAME_OUTPUT_SUFFIX "-arm64")
    endif()
elseif(CGAME_PLATFORM_ANDROID)
    if(CGAME_ARCH_ARM64)
        set(CGAME_OUTPUT_SUFFIX "-arm64")
    elseif(CGAME_ARCH_ARM32)
        set(CGAME_OUTPUT_SUFFIX "-arm32")
    elseif(CGAME_ARCH_X86)
        set(CGAME_OUTPUT_SUFFIX "-x86")
    elseif(CGAME_ARCH_X64)
        set(CGAME_OUTPUT_SUFFIX "-x64")
    endif()
elseif(CGAME_PLATFORM_IOS)
    if(CGAME_ARCH_ARM64)
        set(CGAME_OUTPUT_SUFFIX "-arm64")
    elseif(CGAME_ARCH_X64)
        set(CGAME_OUTPUT_SUFFIX "-x64-sim")
    endif()
elseif(CGAME_PLATFORM_HARMONYOS)
    if(CGAME_ARCH_ARM64)
        set(CGAME_OUTPUT_SUFFIX "-arm64")
    endif()
elseif(CGAME_PLATFORM_WEB)
    set(CGAME_OUTPUT_SUFFIX "-wasm")
endif()

# ========================================
# Install rules
# ========================================

set_target_properties(c-game-server PROPERTIES
    OUTPUT_NAME "c-game-server${CGAME_OUTPUT_SUFFIX}"
)

install(TARGETS c-game-server DESTINATION bin)
if(NOT CGAME_PLATFORM_IOS AND NOT CGAME_PLATFORM_ANDROID AND NOT CGAME_PLATFORM_WEB)
    set_target_properties(c-game-client PROPERTIES
        OUTPUT_NAME "c-game-client${CGAME_OUTPUT_SUFFIX}"
    )
    install(TARGETS c-game-client DESTINATION bin)
endif()