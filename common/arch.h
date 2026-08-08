#ifndef CGAME_ARCH_H
#define CGAME_ARCH_H

/*
 * Architecture detection for CGame Racer
 *
 * Supported architectures:
 *   CGAME_ARCH_X86       - x86 32-bit (IA32)
 *   CGAME_ARCH_X64       - x86_64 / AMD64 / x64
 *   CGAME_ARCH_ARM32     - ARM 32-bit (armv7l / armhf)
 *   CGAME_ARCH_ARM64     - ARM 64-bit (aarch64 / arm64)
 *   CGAME_ARCH_RISCV64   - RISC-V 64-bit
 *   CGAME_ARCH_MIPS64    - MIPS 64-bit
 *   CGAME_ARCH_WASM      - WebAssembly
 *   CGAME_ARCH_ARM64_SIM - ARM64 iOS simulator
 *   CGAME_ARCH_X86_64_SIM - x86_64 iOS simulator
 */

/* x86 / x86_64 */
#if defined(_M_IX86) || defined(__i386__) || defined(__i386) || defined(_X86_)
    #define CGAME_ARCH_X86 1
    #define CGAME_ARCH_NAME "x86"
    #define CGAME_ARCH_BITS 32
#endif

#if defined(_M_AMD64) || defined(_M_X64) || defined(__amd64__) || defined(__x86_64__) || defined(__x86_64)
    #define CGAME_ARCH_X64 1
    #define CGAME_ARCH_NAME "x64"
    #define CGAME_ARCH_BITS 64
#endif

/* ARM 32-bit */
#if defined(_M_ARM) || defined(_M_ARMT) || defined(__arm__) || defined(__arm) || defined(__ARM_ARCH_7__)
    #define CGAME_ARCH_ARM32 1
    #define CGAME_ARCH_NAME "arm32"
    #define CGAME_ARCH_BITS 32
#endif

/* ARM 64-bit */
#if defined(_M_ARM64) || defined(__aarch64__) || defined(__arm64__) || defined(__arm64)
    #define CGAME_ARCH_ARM64 1
    #define CGAME_ARCH_NAME "arm64"
    #define CGAME_ARCH_BITS 64
#endif

/* RISC-V 64-bit */
#if defined(__riscv) && defined(__riscv_xlen) && __riscv_xlen == 64
    #define CGAME_ARCH_RISCV64 1
    #define CGAME_ARCH_NAME "riscv64"
    #define CGAME_ARCH_BITS 64
#endif

/* MIPS 64-bit */
#if defined(__mips64) || defined(__mips64__)
    #define CGAME_ARCH_MIPS64 1
    #define CGAME_ARCH_NAME "mips64"
    #define CGAME_ARCH_BITS 64
#endif

/* WebAssembly */
#if defined(__wasm__) || defined(__EMSCRIPTEN__)
    #define CGAME_ARCH_WASM 1
    #define CGAME_ARCH_NAME "wasm32"
    #define CGAME_ARCH_BITS 32
#endif

/* ARM64 iOS simulator */
#if defined(__APPLE__) && defined(TARGET_OS_IPHONE) && defined(__arm64__)
    #define CGAME_ARCH_ARM64_SIM 1
    #define CGAME_ARCH_NAME "arm64-simulator"
    #define CGAME_ARCH_BITS 64
#endif

/* x86_64 iOS simulator */
#if defined(__APPLE__) && defined(TARGET_OS_IPHONE) && defined(__x86_64__)
    #define CGAME_ARCH_X86_64_SIM 1
    #define CGAME_ARCH_NAME "x86_64-simulator"
    #define CGAME_ARCH_BITS 64
#endif

/* Fallback */
#if !defined(CGAME_ARCH_X86) && \
    !defined(CGAME_ARCH_X64) && \
    !defined(CGAME_ARCH_ARM32) && \
    !defined(CGAME_ARCH_ARM64) && \
    !defined(CGAME_ARCH_RISCV64) && \
    !defined(CGAME_ARCH_MIPS64) && \
    !defined(CGAME_ARCH_WASM) && \
    !defined(CGAME_ARCH_ARM64_SIM) && \
    !defined(CGAME_ARCH_X86_64_SIM)
    #define CGAME_ARCH_UNKNOWN 1
    #define CGAME_ARCH_NAME "unknown"
#endif

/* Architecture group helpers */
#if defined(CGAME_ARCH_X86) || defined(CGAME_ARCH_X64)
    #define CGAME_IS_X86_FAMILY 1
#endif

#if defined(CGAME_ARCH_ARM32) || defined(CGAME_ARCH_ARM64) || defined(CGAME_ARCH_ARM64_SIM)
    #define CGAME_IS_ARM_FAMILY 1
#endif

#if defined(CGAME_ARCH_X86) || defined(CGAME_ARCH_ARM32)
    #define CGAME_IS_32BIT 1
#endif

#if defined(CGAME_ARCH_X64) || defined(CGAME_ARCH_ARM64) || defined(CGAME_ARCH_RISCV64) || defined(CGAME_ARCH_MIPS64)
    #define CGAME_IS_64BIT 1
#endif

#endif
