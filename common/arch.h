#ifndef PXPT_ARCH_H
#define PXPT_ARCH_H

/*
 * Architecture detection for PXPT Racer
 *
 * Supported architectures:
 *   PXPT_ARCH_X86       - x86 32-bit (IA32)
 *   PXPT_ARCH_X64       - x86_64 / AMD64 / x64
 *   PXPT_ARCH_ARM32     - ARM 32-bit (armv7l / armhf)
 *   PXPT_ARCH_ARM64     - ARM 64-bit (aarch64 / arm64)
 *   PXPT_ARCH_RISCV64   - RISC-V 64-bit
 *   PXPT_ARCH_MIPS64    - MIPS 64-bit
 *   PXPT_ARCH_WASM      - WebAssembly
 *   PXPT_ARCH_ARM64_SIM - ARM64 iOS simulator
 *   PXPT_ARCH_X86_64_SIM - x86_64 iOS simulator
 */

/* x86 / x86_64 */
#if defined(_M_IX86) || defined(__i386__) || defined(__i386) || defined(_X86_)
    #define PXPT_ARCH_X86 1
    #define PXPT_ARCH_NAME "x86"
    #define PXPT_ARCH_BITS 32
#endif

#if defined(_M_AMD64) || defined(_M_X64) || defined(__amd64__) || defined(__x86_64__) || defined(__x86_64)
    #define PXPT_ARCH_X64 1
    #define PXPT_ARCH_NAME "x64"
    #define PXPT_ARCH_BITS 64
#endif

/* ARM 32-bit */
#if defined(_M_ARM) || defined(_M_ARMT) || defined(__arm__) || defined(__arm) || defined(__ARM_ARCH_7__)
    #define PXPT_ARCH_ARM32 1
    #define PXPT_ARCH_NAME "arm32"
    #define PXPT_ARCH_BITS 32
#endif

/* ARM 64-bit */
#if defined(_M_ARM64) || defined(__aarch64__) || defined(__arm64__) || defined(__arm64)
    #define PXPT_ARCH_ARM64 1
    #define PXPT_ARCH_NAME "arm64"
    #define PXPT_ARCH_BITS 64
#endif

/* RISC-V 64-bit */
#if defined(__riscv) && defined(__riscv_xlen) && __riscv_xlen == 64
    #define PXPT_ARCH_RISCV64 1
    #define PXPT_ARCH_NAME "riscv64"
    #define PXPT_ARCH_BITS 64
#endif

/* MIPS 64-bit */
#if defined(__mips64) || defined(__mips64__)
    #define PXPT_ARCH_MIPS64 1
    #define PXPT_ARCH_NAME "mips64"
    #define PXPT_ARCH_BITS 64
#endif

/* WebAssembly */
#if defined(__wasm__) || defined(__EMSCRIPTEN__)
    #define PXPT_ARCH_WASM 1
    #define PXPT_ARCH_NAME "wasm32"
    #define PXPT_ARCH_BITS 32
#endif

/* ARM64 iOS simulator */
#if defined(__APPLE__) && defined(TARGET_OS_IPHONE) && defined(__arm64__)
    #define PXPT_ARCH_ARM64_SIM 1
    #define PXPT_ARCH_NAME "arm64-simulator"
    #define PXPT_ARCH_BITS 64
#endif

/* x86_64 iOS simulator */
#if defined(__APPLE__) && defined(TARGET_OS_IPHONE) && defined(__x86_64__)
    #define PXPT_ARCH_X86_64_SIM 1
    #define PXPT_ARCH_NAME "x86_64-simulator"
    #define PXPT_ARCH_BITS 64
#endif

/* Fallback */
#if !defined(PXPT_ARCH_X86) && \
    !defined(PXPT_ARCH_X64) && \
    !defined(PXPT_ARCH_ARM32) && \
    !defined(PXPT_ARCH_ARM64) && \
    !defined(PXPT_ARCH_RISCV64) && \
    !defined(PXPT_ARCH_MIPS64) && \
    !defined(PXPT_ARCH_WASM) && \
    !defined(PXPT_ARCH_ARM64_SIM) && \
    !defined(PXPT_ARCH_X86_64_SIM)
    #define PXPT_ARCH_UNKNOWN 1
    #define PXPT_ARCH_NAME "unknown"
#endif

/* Architecture group helpers */
#if defined(PXPT_ARCH_X86) || defined(PXPT_ARCH_X64)
    #define PXPT_IS_X86_FAMILY 1
#endif

#if defined(PXPT_ARCH_ARM32) || defined(PXPT_ARCH_ARM64) || defined(PXPT_ARCH_ARM64_SIM)
    #define PXPT_IS_ARM_FAMILY 1
#endif

#if defined(PXPT_ARCH_X86) || defined(PXPT_ARCH_ARM32)
    #define PXPT_IS_32BIT 1
#endif

#if defined(PXPT_ARCH_X64) || defined(PXPT_ARCH_ARM64) || defined(PXPT_ARCH_RISCV64) || defined(PXPT_ARCH_MIPS64)
    #define PXPT_IS_64BIT 1
#endif

#endif
