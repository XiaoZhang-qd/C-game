#ifndef PXPT_PLATFORM_H
#define PXPT_PLATFORM_H

/*
 * Comprehensive platform + architecture detection for PXPT Racer
 *
 * Supported platforms (see PXPT_IS_DESKTOP, PXPT_IS_MOBILE, PXPT_IS_WEB):
 *   PXPT_WINDOWS     - Windows (x86/x64/ARM64)
 *   PXPT_LINUX       - Linux (x86/x64/ARM32/ARM64/RISC-V/PowerPC/s390x/MIPS)
 *   PXPT_MACOS       - macOS (x64/Apple Silicon/ARM64e)
 *   PXPT_IOS         - iOS (ARM64/ARM64e)
 *   PXPT_ANDROID     - Android (ARM/ARM64/x86/x64)
 *   PXPT_FREEBSD     - FreeBSD
 *   PXPT_OPENBSD     - OpenBSD
 *   PXPT_NETBSD      - NetBSD
 *   PXPT_DRAGONFLY    - DragonFly BSD
 *   PXPT_HARMONYOS    - HarmonyOS / OpenHarmony
 *   PXPT_CHROMEOS     - ChromeOS
 *   PXPT_WEB          - WebAssembly / Emscripten
 *   PXPT_DRM          - Linux DRM/KMS (headless/embedded)
 *
 * Supported architectures:
 *   PXPT_ARCH_X86     - 32-bit x86 (i386)
 *   PXPT_ARCH_X64     - 64-bit x86-64 / AMD64
 *   PXPT_ARCH_ARM32   - 32-bit ARM (armv6l / armv7l)
 *   PXPT_ARCH_ARM64   - 64-bit ARM (aarch64 / arm64 / arm64e)
 *   PXPT_ARCH_RISCV32 - 32-bit RISC-V
 *   PXPT_ARCH_RISCV64 - 64-bit RISC-V
 *   PXPT_ARCH_PPC32   - 32-bit PowerPC
 *   PXPT_ARCH_PPC64   - 64-bit PowerPC (BE/LE)
 *   PXPT_ARCH_S390X   - IBM System z 64-bit
 *   PXPT_ARCH_MIPS32  - 32-bit MIPS
 *   PXPT_ARCH_MIPS64  - 64-bit MIPS
 *   PXPT_ARCH_SPARC64 - 64-bit SPARC
 */

/* ========== Platform Detection ========== */

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__WINDOWS__)
    #define PXPT_WINDOWS 1
#endif

#if defined(__ANDROID__)
    #define PXPT_ANDROID 1
#endif

#if defined(__APPLE__) && defined(TARGET_OS_IPHONE)
    #define PXPT_IOS 1
#endif

#if defined(__APPLE__) && !defined(TARGET_OS_IPHONE)
    #define PXPT_MACOS 1
#endif

#if defined(__linux__) && !defined(__ANDROID__)
    #define PXPT_LINUX 1
#endif

#if defined(__linux__) && defined(__EMSCRIPTEN__)
    #define PXPT_CHROMEOS 1
#endif

#if defined(__FreeBSD__)
    #define PXPT_FREEBSD 1
#endif

#if defined(__OpenBSD__)
    #define PXPT_OPENBSD 1
#endif

#if defined(__NetBSD__)
    #define PXPT_NETBSD 1
#endif

#if defined(__DragonFly__)
    #define PXPT_DRAGONFLY 1
#endif

#if defined(__OHOS__) || defined(HARMONYOS)
    #define PXPT_HARMONYOS 1
#endif

#if defined(__EMSCRIPTEN__)
    #define PXPT_WEB 1
#endif

#if defined(__linux__) && defined(PXPT_USE_DRM)
    #define PXPT_DRM 1
#endif

#if !defined(PXPT_WINDOWS) && \
    !defined(PXPT_ANDROID) && \
    !defined(PXPT_IOS) && \
    !defined(PXPT_MACOS) && \
    !defined(PXPT_LINUX) && \
    !defined(PXPT_FREEBSD) && \
    !defined(PXPT_OPENBSD) && \
    !defined(PXPT_NETBSD) && \
    !defined(PXPT_DRAGONFLY) && \
    !defined(PXPT_HARMONYOS) && \
    !defined(PXPT_WEB)
    #define PXPT_POSIX 1
#endif

/* ========== Architecture Detection ========== */

#if defined(__i386__) || defined(_M_IX86) || defined(__i386)
    #define PXPT_ARCH_X86 1
    #define PXPT_ARCH_NAME "x86"
    #define PXPT_ARCH_BITS 32
#elif defined(__x86_64__) || defined(_M_X64) || defined(__amd64__)
    #define PXPT_ARCH_X64 1
    #define PXPT_ARCH_NAME "x64"
    #define PXPT_ARCH_BITS 64
#elif defined(__arm__) || defined(_M_ARM) || defined(__armv6l__) || defined(__armv7l__)
    #define PXPT_ARCH_ARM32 1
    #define PXPT_ARCH_NAME "arm32"
    #define PXPT_ARCH_BITS 32
    #if defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)
        #define PXPT_ARCH_ARMV6 1
    #elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
        #define PXPT_ARCH_ARMV7 1
    #endif
#elif defined(__aarch64__) || defined(_M_ARM64) || defined(__arm64__)
    #define PXPT_ARCH_ARM64 1
    #define PXPT_ARCH_NAME "arm64"
    #define PXPT_ARCH_BITS 64
    #if defined(__arm64e__) || defined(_M_ARM64E)
        #define PXPT_ARCH_ARM64E 1
    #endif
#elif defined(__riscv) || defined(__riscv__)
    #if __SIZEOF_POINTER__ == 8
        #define PXPT_ARCH_RISCV64 1
        #define PXPT_ARCH_NAME "riscv64"
    #else
        #define PXPT_ARCH_RISCV32 1
        #define PXPT_ARCH_NAME "riscv32"
    #endif
    #define PXPT_ARCH_BITS __SIZEOF_POINTER__ * 8
#elif defined(__powerpc64__) || defined(__ppc64__) || defined(_M_PPC64)
    #define PXPT_ARCH_PPC64 1
    #define PXPT_ARCH_NAME "ppc64"
    #define PXPT_ARCH_BITS 64
    #if defined(__LITTLE_ENDIAN__) || defined(_LITTLE_ENDIAN)
        #define PXPT_ARCH_PPC64LE 1
    #endif
#elif defined(__powerpc__) || defined(__ppc__) || defined(_M_PPC)
    #define PXPT_ARCH_PPC32 1
    #define PXPT_ARCH_NAME "ppc32"
    #define PXPT_ARCH_BITS 32
#elif defined(__s390x__) || defined(__zarch__) || defined(_M_S390)
    #define PXPT_ARCH_S390X 1
    #define PXPT_ARCH_NAME "s390x"
    #define PXPT_ARCH_BITS 64
#elif defined(__mips64) || defined(__mips64__)
    #define PXPT_ARCH_MIPS64 1
    #define PXPT_ARCH_NAME "mips64"
    #define PXPT_ARCH_BITS 64
    #if defined(__MIPSEL__) || defined(__mipsel__)
        #define PXPT_ARCH_MIPS64EL 1
    #endif
#elif defined(__mips__) || defined(__mips32__)
    #define PXPT_ARCH_MIPS32 1
    #define PXPT_ARCH_NAME "mips32"
    #define PXPT_ARCH_BITS 32
    #if defined(__MIPSEL__) || defined(__mipsel__)
        #define PXPT_ARCH_MIPS32EL 1
    #endif
#elif defined(__sparc64__) || defined(__sparcv9)
    #define PXPT_ARCH_SPARC64 1
    #define PXPT_ARCH_NAME "sparc64"
    #define PXPT_ARCH_BITS 64
#elif defined(__EMSCRIPTEN__)
    #define PXPT_ARCH_WASM 1
    #define PXPT_ARCH_NAME "wasm"
    #define PXPT_ARCH_BITS 32
#else
    #define PXPT_ARCH_NAME "unknown"
    #define PXPT_ARCH_BITS 0
#endif

/* ========== Platform Group Helpers ========== */

#if defined(PXPT_WINDOWS)
    #define PXPT_PLATFORM_NAME "Windows"
    #define PXPT_IS_DESKTOP 1
#elif defined(PXPT_MACOS)
    #define PXPT_PLATFORM_NAME "macOS"
    #define PXPT_IS_DESKTOP 1
#elif defined(PXPT_LINUX)
    #define PXPT_PLATFORM_NAME "Linux"
    #define PXPT_IS_DESKTOP 1
#elif defined(PXPT_FREEBSD)
    #define PXPT_PLATFORM_NAME "FreeBSD"
    #define PXPT_IS_DESKTOP 1
#elif defined(PXPT_OPENBSD)
    #define PXPT_PLATFORM_NAME "OpenBSD"
    #define PXPT_IS_DESKTOP 1
#elif defined(PXPT_NETBSD)
    #define PXPT_PLATFORM_NAME "NetBSD"
    #define PXPT_IS_DESKTOP 1
#elif defined(PXPT_DRAGONFLY)
    #define PXPT_PLATFORM_NAME "DragonFly"
    #define PXPT_IS_DESKTOP 1
#elif defined(PXPT_ANDROID)
    #define PXPT_PLATFORM_NAME "Android"
    #define PXPT_IS_MOBILE 1
#elif defined(PXPT_IOS)
    #define PXPT_PLATFORM_NAME "iOS"
    #define PXPT_IS_MOBILE 1
#elif defined(PXPT_HARMONYOS)
    #define PXPT_PLATFORM_NAME "HarmonyOS"
    #define PXPT_IS_MOBILE 1
#elif defined(PXPT_WEB)
    #define PXPT_PLATFORM_NAME "Web"
    #define PXPT_IS_WEB 1
#elif defined(PXPT_POSIX)
    #define PXPT_PLATFORM_NAME "POSIX"
    #define PXPT_IS_DESKTOP 1
#else
    #define PXPT_PLATFORM_NAME "Unknown"
#endif

#endif
