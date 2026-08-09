#ifndef CGAME_PLATFORM_H
#define CGAME_PLATFORM_H

/*
 * Comprehensive platform + architecture detection for CGame Racer
 *
 * Supported platforms (see CGAME_IS_DESKTOP, CGAME_IS_MOBILE, CGAME_IS_WEB):
 *   CGAME_WINDOWS     - Windows (x86/x64/ARM64)
 *   CGAME_LINUX       - Linux (x86/x64/ARM32/ARM64/RISC-V/PowerPC/s390x/MIPS)
 *   CGAME_MACOS       - macOS (x64/Apple Silicon/ARM64e)
 *   CGAME_IOS         - iOS (ARM64/ARM64e)
 *   CGAME_ANDROID     - Android (ARM/ARM64/x86/x64)
 *   CGAME_FREEBSD     - FreeBSD
 *   CGAME_OPENBSD     - OpenBSD
 *   CGAME_NETBSD      - NetBSD
 *   CGAME_DRAGONFLY    - DragonFly BSD
 *   CGAME_HARMONYOS    - HarmonyOS / OpenHarmony
 *   CGAME_CHROMEOS     - ChromeOS
 *   CGAME_WEB          - WebAssembly / Emscripten
 *   CGAME_DRM          - Linux DRM/KMS (headless/embedded)
 *
 * Supported architectures:
 *   CGAME_ARCH_X86     - 32-bit x86 (i386)
 *   CGAME_ARCH_X64     - 64-bit x86-64 / AMD64
 *   CGAME_ARCH_ARM32   - 32-bit ARM (armv6l / armv7l)
 *   CGAME_ARCH_ARM64   - 64-bit ARM (aarch64 / arm64 / arm64e)
 *   CGAME_ARCH_RISCV32 - 32-bit RISC-V
 *   CGAME_ARCH_RISCV64 - 64-bit RISC-V
 *   CGAME_ARCH_PPC32   - 32-bit PowerPC
 *   CGAME_ARCH_PPC64   - 64-bit PowerPC (BE/LE)
 *   CGAME_ARCH_S390X   - IBM System z 64-bit
 *   CGAME_ARCH_MIPS32  - 32-bit MIPS
 *   CGAME_ARCH_MIPS64  - 64-bit MIPS
 *   CGAME_ARCH_SPARC64 - 64-bit SPARC
 */

/* ========== Platform Detection ========== */

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__WINDOWS__)
    #define CGAME_WINDOWS 1
#endif

#if defined(__ANDROID__)
    #define CGAME_ANDROID 1
#endif

#if defined(__APPLE__) && defined(TARGET_OS_IPHONE)
    #define CGAME_IOS 1
#endif

#if defined(__APPLE__) && !defined(TARGET_OS_IPHONE)
    #define CGAME_MACOS 1
#endif

#if defined(__linux__) && !defined(__ANDROID__)
    #define CGAME_LINUX 1
#endif

#if defined(__linux__) && defined(__EMSCRIPTEN__)
    #define CGAME_CHROMEOS 1
#endif

#if defined(__FreeBSD__)
    #define CGAME_FREEBSD 1
#endif

#if defined(__OpenBSD__)
    #define CGAME_OPENBSD 1
#endif

#if defined(__NetBSD__)
    #define CGAME_NETBSD 1
#endif

#if defined(__DragonFly__)
    #define CGAME_DRAGONFLY 1
#endif

#if defined(__OHOS__) || defined(HARMONYOS)
    #define CGAME_HARMONYOS 1
#endif

#if defined(__EMSCRIPTEN__)
    #define CGAME_WEB 1
#endif

#if defined(__linux__) && defined(CGAME_USE_DRM)
    #define CGAME_DRM 1
#endif

#if !defined(CGAME_WINDOWS) && \
    !defined(CGAME_ANDROID) && \
    !defined(CGAME_IOS) && \
    !defined(CGAME_MACOS) && \
    !defined(CGAME_LINUX) && \
    !defined(CGAME_FREEBSD) && \
    !defined(CGAME_OPENBSD) && \
    !defined(CGAME_NETBSD) && \
    !defined(CGAME_DRAGONFLY) && \
    !defined(CGAME_HARMONYOS) && \
    !defined(CGAME_WEB)
    #define CGAME_POSIX 1
#endif

/* ========== Architecture Detection ========== */

#if defined(__i386__) || defined(_M_IX86) || defined(__i386)
    #define CGAME_ARCH_X86 1
    #define CGAME_ARCH_NAME "x86"
    #define CGAME_ARCH_BITS 32
#elif defined(__x86_64__) || defined(_M_X64) || defined(__amd64__)
    #define CGAME_ARCH_X64 1
    #define CGAME_ARCH_NAME "x64"
    #define CGAME_ARCH_BITS 64
#elif defined(__arm__) || defined(_M_ARM) || defined(__armv6l__) || defined(__armv7l__)
    #define CGAME_ARCH_ARM32 1
    #define CGAME_ARCH_NAME "arm32"
    #define CGAME_ARCH_BITS 32
    #if defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)
        #define CGAME_ARCH_ARMV6 1
    #elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
        #define CGAME_ARCH_ARMV7 1
    #endif
#elif defined(__aarch64__) || defined(_M_ARM64) || defined(__arm64__)
    #define CGAME_ARCH_ARM64 1
    #define CGAME_ARCH_NAME "arm64"
    #define CGAME_ARCH_BITS 64
    #if defined(__arm64e__) || defined(_M_ARM64E)
        #define CGAME_ARCH_ARM64E 1
    #endif
#elif defined(__riscv) || defined(__riscv__)
    #if __SIZEOF_POINTER__ == 8
        #define CGAME_ARCH_RISCV64 1
        #define CGAME_ARCH_NAME "riscv64"
    #else
        #define CGAME_ARCH_RISCV32 1
        #define CGAME_ARCH_NAME "riscv32"
    #endif
    #define CGAME_ARCH_BITS __SIZEOF_POINTER__ * 8
#elif defined(__powerpc64__) || defined(__ppc64__) || defined(_M_PPC64)
    #define CGAME_ARCH_PPC64 1
    #define CGAME_ARCH_NAME "ppc64"
    #define CGAME_ARCH_BITS 64
    #if defined(__LITTLE_ENDIAN__) || defined(_LITTLE_ENDIAN)
        #define CGAME_ARCH_PPC64LE 1
    #endif
#elif defined(__powerpc__) || defined(__ppc__) || defined(_M_PPC)
    #define CGAME_ARCH_PPC32 1
    #define CGAME_ARCH_NAME "ppc32"
    #define CGAME_ARCH_BITS 32
#elif defined(__s390x__) || defined(__zarch__) || defined(_M_S390)
    #define CGAME_ARCH_S390X 1
    #define CGAME_ARCH_NAME "s390x"
    #define CGAME_ARCH_BITS 64
#elif defined(__mips64) || defined(__mips64__)
    #define CGAME_ARCH_MIPS64 1
    #define CGAME_ARCH_NAME "mips64"
    #define CGAME_ARCH_BITS 64
    #if defined(__MIPSEL__) || defined(__mipsel__)
        #define CGAME_ARCH_MIPS64EL 1
    #endif
#elif defined(__mips__) || defined(__mips32__)
    #define CGAME_ARCH_MIPS32 1
    #define CGAME_ARCH_NAME "mips32"
    #define CGAME_ARCH_BITS 32
    #if defined(__MIPSEL__) || defined(__mipsel__)
        #define CGAME_ARCH_MIPS32EL 1
    #endif
#elif defined(__sparc64__) || defined(__sparcv9)
    #define CGAME_ARCH_SPARC64 1
    #define CGAME_ARCH_NAME "sparc64"
    #define CGAME_ARCH_BITS 64
#elif defined(__EMSCRIPTEN__)
    #define CGAME_ARCH_WASM 1
    #define CGAME_ARCH_NAME "wasm"
    #define CGAME_ARCH_BITS 32
#else
    #define CGAME_ARCH_NAME "unknown"
    #define CGAME_ARCH_BITS 0
#endif

/* ========== Platform Group Helpers ========== */

#if defined(CGAME_WINDOWS)
    #define CGAME_PLATFORM_NAME "Windows"
    #define CGAME_IS_DESKTOP 1
#elif defined(CGAME_MACOS)
    #define CGAME_PLATFORM_NAME "macOS"
    #define CGAME_IS_DESKTOP 1
#elif defined(CGAME_LINUX)
    #define CGAME_PLATFORM_NAME "Linux"
    #define CGAME_IS_DESKTOP 1
#elif defined(CGAME_FREEBSD)
    #define CGAME_PLATFORM_NAME "FreeBSD"
    #define CGAME_IS_DESKTOP 1
#elif defined(CGAME_OPENBSD)
    #define CGAME_PLATFORM_NAME "OpenBSD"
    #define CGAME_IS_DESKTOP 1
#elif defined(CGAME_NETBSD)
    #define CGAME_PLATFORM_NAME "NetBSD"
    #define CGAME_IS_DESKTOP 1
#elif defined(CGAME_DRAGONFLY)
    #define CGAME_PLATFORM_NAME "DragonFly"
    #define CGAME_IS_DESKTOP 1
#elif defined(CGAME_ANDROID)
    #define CGAME_PLATFORM_NAME "Android"
    #define CGAME_IS_MOBILE 1
#elif defined(CGAME_IOS)
    #define CGAME_PLATFORM_NAME "iOS"
    #define CGAME_IS_MOBILE 1
#elif defined(CGAME_HARMONYOS)
    #define CGAME_PLATFORM_NAME "HarmonyOS"
    #define CGAME_IS_MOBILE 1
#elif defined(CGAME_WEB)
    #define CGAME_PLATFORM_NAME "Web"
    #define CGAME_IS_WEB 1
#elif defined(CGAME_POSIX)
    #define CGAME_PLATFORM_NAME "POSIX"
    #define CGAME_IS_DESKTOP 1
#else
    #define CGAME_PLATFORM_NAME "Unknown"
#endif

#endif
