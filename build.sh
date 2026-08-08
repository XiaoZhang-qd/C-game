#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "${SCRIPT_DIR}"

# ========================================
# Parse arguments
# ========================================
ACTION="${1:-build}"
ARCH="${2:-}"

show_help() {
    cat << EOF
========================================
  PXPT Racer Build Script (Linux/macOS)
  Usage:
    ./build.sh [arch]                  Build (default action)
    ./build.sh clean [arch]            Clean build artifacts
    ./build.sh clean all               Clean ALL build artifacts
    ./build.sh help                    Show this help

  arch: x64 (default), x86, arm64, arm32, riscv64, riscv32,
        ppc64, ppc64le, ppc32, s390x, mips64, mips64le,
        mips32, mips32le, sparc64, universal (macOS)
EOF
}

# ========================================
# Clean action
# ========================================
if [ "${ACTION}" = "clean" ] || [ "${ACTION}" = "clean" ]; then
    CLEAN_ARCH="${ARCH:-}"

    all_build_dirs=(
        "build"
        "build-win32"
        "build-win-arm64"
        "build-linux-x86"
        "build-linux-arm32"
        "build-linux-arm64"
        "build-linux-riscv64"
        "build-linux-riscv32"
        "build-linux-ppc64"
        "build-linux-ppc64le"
        "build-linux-ppc32"
        "build-linux-s390x"
        "build-linux-mips64"
        "build-linux-mips64le"
        "build-linux-mips32"
        "build-linux-mips32le"
        "build-linux-sparc64"
        "build-drm"
        "build-chromeos"
        "build-chromeos-x64"
        "build-macos-x64"
        "build-macos-arm64"
        "build-macos-universal"
        "build-freebsd"
        "build-openbsd"
        "build-netbsd"
        "build-dragonfly"
        "build-android-arm64"
        "build-android-armeabi-v7a"
        "build-android-x86"
        "build-android-x86_64"
        "build-ios"
        "build-ios-theos"
        "build-ios-sim"
        "build-ios-sim-theos"
        "build-web"
        "build-harmonyos-arm64"
        "build-mingw-x64"
        "build-mingw-x86"
    )

    preset_to_build_dir() {
        case "$1" in
            linux-x64|macos-x64)     echo "build" ;;
            linux-x86)               echo "build-linux-x86" ;;
            linux-arm64)             echo "build-linux-arm64" ;;
            linux-arm32)             echo "build-linux-arm32" ;;
            linux-riscv64)           echo "build-linux-riscv64" ;;
            linux-riscv32)           echo "build-linux-riscv32" ;;
            linux-ppc64)             echo "build-linux-ppc64" ;;
            linux-ppc64le)           echo "build-linux-ppc64le" ;;
            linux-ppc32)             echo "build-linux-ppc32" ;;
            linux-s390x)             echo "build-linux-s390x" ;;
            linux-mips64)            echo "build-linux-mips64" ;;
            linux-mips64le)          echo "build-linux-mips64le" ;;
            linux-mips32)            echo "build-linux-mips32" ;;
            linux-mips32le)          echo "build-linux-mips32le" ;;
            linux-sparc64)           echo "build-linux-sparc64" ;;
            linux-drm)               echo "build-drm" ;;
            linux-chromeos)          echo "build-chromeos" ;;
            macos-arm64)             echo "build-macos-arm64" ;;
            macos-universal)         echo "build-macos-universal" ;;
            freebsd-x64)             echo "build-freebsd" ;;
            openbsd-x64)             echo "build-openbsd" ;;
            netbsd-x64)              echo "build-netbsd" ;;
            dragonfly-x64)           echo "build-dragonfly" ;;
            android-arm64)           echo "build-android-arm64" ;;
            android-armv7)           echo "build-android-armeabi-v7a" ;;
            android-x86)             echo "build-android-x86" ;;
            android-x86_64)          echo "build-android-x86_64" ;;
            ios-arm64)               echo "build-ios" ;;
            ios-arm64-theos)         echo "build-ios-theos" ;;
            ios-arm64-simulator)     echo "build-ios-sim" ;;
            ios-arm64-sim-theos)     echo "build-ios-sim-theos" ;;
            web-emscripten)          echo "build-web" ;;
            harmonyos-arm64)         echo "build-harmonyos-arm64" ;;
            chromeos-x64)            echo "build-chromeos-x64" ;;
            windows-x64)             echo "build" ;;
            windows-x86)             echo "build-win32" ;;
            windows-arm64)           echo "build-win-arm64" ;;
            *)                       echo "" ;;
        esac
    }

    do_clean_dir() {
        local dir="$1"
        if [ -z "${dir}" ]; then
            dir="build"
        fi
        if [ -d "${dir}" ]; then
            echo "[CLEAN] Removing ${dir}..."
            rm -rf "${dir}"
            echo "[CLEAN] Done: ${dir}"
        else
            echo "[CLEAN] Skipping (not found): ${dir}"
        fi
    }

    if [ "${CLEAN_ARCH}" = "all" ] || [ "${CLEAN_ARCH}" = "*" ]; then
        echo "========================================"
        echo "  Cleaning ALL build artifacts"
        echo "========================================"
        echo
        for dir in "${all_build_dirs[@]}"; do
            do_clean_dir "${dir}"
        done
        echo
        echo "[CLEAN] All build artifacts cleaned."
        exit 0
    elif [ -z "${CLEAN_ARCH}" ]; then
        HOST_OS="$(uname -s)"
        case "${HOST_OS}" in
            Linux)  CLEAN_ARCH="x64" ;;
            Darwin) CLEAN_ARCH="x64" ;;
            *)      CLEAN_ARCH="x64" ;;
        esac
        echo "[CLEAN] No architecture specified, cleaning default: ${CLEAN_ARCH}"
    fi

    detect_os_for_clean() {
        case "$(uname -s)" in
            Linux)    echo "Linux" ;;
            Darwin)   echo "macOS" ;;
            FreeBSD)  echo "FreeBSD" ;;
            OpenBSD)  echo "OpenBSD" ;;
            NetBSD)   echo "NetBSD" ;;
            DragonFly) echo "DragonFly" ;;
            *)        echo "Unknown" ;;
        esac
    }

    HOST_OS="$(detect_os_for_clean)"
    PRESET=""

    case "${HOST_OS}" in
        Linux)
            case "${CLEAN_ARCH}" in
                x64)      PRESET="linux-x64" ;;
                x86)      PRESET="linux-x86" ;;
                arm64)    PRESET="linux-arm64" ;;
                arm32)    PRESET="linux-arm32" ;;
                riscv64)  PRESET="linux-riscv64" ;;
                riscv32)  PRESET="linux-riscv32" ;;
                ppc64)    PRESET="linux-ppc64" ;;
                ppc64le)  PRESET="linux-ppc64le" ;;
                ppc32)    PRESET="linux-ppc32" ;;
                s390x)    PRESET="linux-s390x" ;;
                mips64)   PRESET="linux-mips64" ;;
                mips64le) PRESET="linux-mips64le" ;;
                mips32)   PRESET="linux-mips32" ;;
                mips32le) PRESET="linux-mips32le" ;;
                sparc64)  PRESET="linux-sparc64" ;;
                *)        echo "[ERROR] Unknown arch '${CLEAN_ARCH}' on Linux"; exit 1 ;;
            esac
            ;;
        macOS)
            case "${CLEAN_ARCH}" in
                x64)       PRESET="macos-x64" ;;
                arm64)     PRESET="macos-arm64" ;;
                universal) PRESET="macos-universal" ;;
                *)         echo "[ERROR] Unknown arch '${CLEAN_ARCH}' on macOS"; exit 1 ;;
            esac
            ;;
        FreeBSD|OpenBSD|NetBSD|DragonFly)
            case "${CLEAN_ARCH}" in
                x64)   PRESET="${HOST_OS,,}-x64" ;;
                *)     echo "[ERROR] Unknown arch '${CLEAN_ARCH}' on ${HOST_OS}"; exit 1 ;;
            esac
            ;;
        *)
            echo "[ERROR] Unsupported OS: ${HOST_OS}"
            exit 1
            ;;
    esac

    BUILD_DIR="$(preset_to_build_dir "${PRESET}")"
    do_clean_dir "${BUILD_DIR}"
    exit 0
fi

# ========================================
# Help action
# ========================================
if [ "${ACTION}" = "help" ] || [ "${ACTION}" = "-h" ] || [ "${ACTION}" = "--help" ]; then
    show_help
    exit 0
fi

# ========================================
# Build action (default)
# ========================================
ARCH="${ACTION}"
if [ -n "${ARCH}" ] && [ "${ARCH}" != "build" ]; then
    :
fi
if [ "${ARCH}" = "build" ]; then
    ARCH="${2:-}"
fi

echo "========================================"
echo "  PXPT Racer Build Script (Linux/macOS)"
echo "  Usage: ./build.sh [arch]"
echo "    arch: x64 (default), x86, arm64, arm32, riscv64, riscv32,"
echo "          ppc64, ppc64le, ppc32, s390x, mips64, mips64le,"
echo "          mips32, mips32le, sparc64, universal (macOS)"
echo "========================================"
echo

# ========================================
# Architecture -> CMake preset mapping
# ========================================
detect_os() {
    case "$(uname -s)" in
        Linux)  echo "Linux" ;;
        Darwin) echo "macOS" ;;
        FreeBSD) echo "FreeBSD" ;;
        OpenBSD) echo "OpenBSD" ;;
        NetBSD) echo "NetBSD" ;;
        DragonFly) echo "DragonFly" ;;
        *)      echo "Unknown" ;;
    esac
}

HOST_OS="$(detect_os)"

resolve_preset() {
    local arch="$1"
    local os="$2"

    if [ -z "${arch}" ]; then
        echo ""
        return
    fi

    case "${os}" in
        Linux)
            case "${arch}" in
                x64)      echo "linux-x64" ;;
                x86)      echo "linux-x86" ;;
                arm64)    echo "linux-arm64" ;;
                arm32)    echo "linux-arm32" ;;
                riscv64)  echo "linux-riscv64" ;;
                riscv32)  echo "linux-riscv32" ;;
                ppc64)    echo "linux-ppc64" ;;
                ppc64le)  echo "linux-ppc64le" ;;
                ppc32)    echo "linux-ppc32" ;;
                s390x)    echo "linux-s390x" ;;
                mips64)   echo "linux-mips64" ;;
                mips64le) echo "linux-mips64le" ;;
                mips32)   echo "linux-mips32" ;;
                mips32le) echo "linux-mips32le" ;;
                sparc64)  echo "linux-sparc64" ;;
                *)        echo "INVALID" ;;
            esac
            ;;
        macOS)
            case "${arch}" in
                x64)       echo "macos-x64" ;;
                arm64)     echo "macos-x64" ;;
                universal) echo "macos-universal" ;;
                *)         echo "INVALID" ;;
            esac
            ;;
        FreeBSD|OpenBSD|NetBSD|DragonFly)
            case "${arch}" in
                x64)   echo "${os,,}-x64" ;;
                *)     echo "INVALID" ;;
            esac
            ;;
        *)
            echo "INVALID"
            ;;
    esac
}

preset_build_dir() {
    local preset="$1"
    case "${preset}" in
        linux-x64|macos-x64)    echo "build" ;;
        linux-x86)              echo "build-linux-x86" ;;
        linux-arm64)            echo "build-linux-arm64" ;;
        linux-arm32)            echo "build-linux-arm32" ;;
        linux-riscv64)          echo "build-linux-riscv64" ;;
        linux-riscv32)          echo "build-linux-riscv32" ;;
        linux-ppc64)            echo "build-linux-ppc64" ;;
        linux-ppc64le)          echo "build-linux-ppc64le" ;;
        linux-ppc32)            echo "build-linux-ppc32" ;;
        linux-s390x)            echo "build-linux-s390x" ;;
        linux-mips64)           echo "build-linux-mips64" ;;
        linux-mips64le)         echo "build-linux-mips64le" ;;
        linux-mips32)           echo "build-linux-mips32" ;;
        linux-mips32le)         echo "build-linux-mips32le" ;;
        linux-sparc64)          echo "build-linux-sparc64" ;;
        linux-drm)              echo "build-drm" ;;
        macos-universal)        echo "build-macos-universal" ;;
        freebsd-x64)            echo "build-freebsd" ;;
        openbsd-x64)            echo "build-openbsd" ;;
        netbsd-x64)             echo "build-netbsd" ;;
        dragonfly-x64)          echo "build-dragonfly" ;;
        *)                      echo "build" ;;
    esac
}

PRESET_NAME="$(resolve_preset "${ARCH}" "${HOST_OS}")"

if [ "${PRESET_NAME}" = "INVALID" ]; then
    echo "[ERROR] Unsupported architecture '${ARCH}' on ${HOST_OS}."
    echo "        Supported architectures on ${HOST_OS}:"
    if [ "${HOST_OS}" = "Linux" ]; then
        echo "          x64, x86, arm64, arm32, riscv64, riscv32, ppc64, ppc64le, ppc32, s390x, mips64, mips64le, mips32, mips32le, sparc64"
    elif [ "${HOST_OS}" = "macOS" ]; then
        echo "          x64, arm64, universal"
    elif [ "${HOST_OS}" = "FreeBSD" ] || [ "${HOST_OS}" = "OpenBSD" ] || [ "${HOST_OS}" = "NetBSD" ] || [ "${HOST_OS}" = "DragonFly" ]; then
        echo "          x64"
    fi
    echo
    echo "        Or use CMake presets directly:"
    echo "          cmake --preset <preset-name>"
    exit 1
fi

# ========================================
# Check dependencies
# ========================================

if ! command -v cmake &> /dev/null; then
    echo "[ERROR] cmake not found."
    echo "        Install via your package manager:"
    echo "          Ubuntu/Debian: sudo apt install cmake"
    echo "          Fedora:        sudo dnf install cmake"
    echo "          Arch:          sudo pacman -S cmake"
    echo "          macOS:         brew install cmake"
    exit 1
fi

if [ -n "${PRESET_NAME}" ]; then
    echo "[INFO] Using CMake preset: ${PRESET_NAME}"
    echo "[INFO] Target architecture: ${ARCH}"
    echo

    echo "[INFO] Configuring..."
    cmake --preset "${PRESET_NAME}"
    if [ $? -ne 0 ]; then
        echo "[ERROR] Configuration failed."
        exit 1
    fi

    echo
    echo "[INFO] Building..."
    cmake --build --preset "${PRESET_NAME}"
    if [ $? -ne 0 ]; then
        echo "[ERROR] Build failed."
        exit 1
    fi

    BUILD_DIR="$(preset_build_dir "${PRESET_NAME}")"
else
    # Native build - auto-detect generator
    GENERATOR=""
    BUILD_CMD=""

    if command -v ninja &> /dev/null; then
        GENERATOR="Ninja"
        BUILD_CMD="cmake --build ."
    elif command -v make &> /dev/null; then
        GENERATOR="Unix Makefiles"
        if command -v nproc &> /dev/null; then
            CORES=$(nproc)
        elif command -v sysctl &> /dev/null; then
            CORES=$(sysctl -n hw.ncpu 2>/dev/null || echo 2)
        else
            CORES=2
        fi
        BUILD_CMD="make -j${CORES}"
    else
        echo "[ERROR] Neither ninja nor make found."
        echo "        Install ninja or make via your package manager."
        exit 1
    fi

    echo "[INFO] Using generator: ${GENERATOR}"

    CC_FOUND=""
    if command -v cc &> /dev/null; then
        CC_FOUND="cc"
    elif command -v gcc &> /dev/null; then
        CC_FOUND="gcc"
    elif command -v clang &> /dev/null; then
        CC_FOUND="clang"
    fi

    if [ -z "${CC_FOUND}" ]; then
        echo "[ERROR] No C compiler found."
        echo "        Install gcc, clang, or a C99-compatible compiler."
        exit 1
    fi
    echo "[INFO] C compiler: ${CC_FOUND}"

    # Check raylib
    RAYLIB_LIB="raylib/src/libraylib.a"
    if [ ! -f "${RAYLIB_LIB}" ]; then
        echo "[WARN] ${RAYLIB_LIB} not found."
        echo "       Building raylib from source..."
        RAYLIB_DIR="${SCRIPT_DIR}/raylib"
        if [ -d "${RAYLIB_DIR}" ]; then
            (cd "${RAYLIB_DIR}" && make -j4)
            if [ ! -f "${RAYLIB_LIB}" ]; then
                echo "[ERROR] Failed to build raylib. Check raylib directory."
                exit 1
            fi
        else
            echo "[ERROR] raylib directory not found. Please include raylib sources."
            exit 1
        fi
    fi

    mkdir -p build
    cd build

    echo "[INFO] Configuring with ${GENERATOR}..."
    cmake -G "${GENERATOR}" ..
    if [ $? -ne 0 ]; then
        echo "[ERROR] Configuration failed."
        cd ..
        exit 1
    fi

    echo
    echo "[INFO] Building (${BUILD_CMD})..."
    ${BUILD_CMD}
    if [ $? -ne 0 ]; then
        echo "[ERROR] Build failed."
        cd ..
        exit 1
    fi

    cd ..
    BUILD_DIR="build"
fi

echo
echo "========================================"
echo "  Build successful!"
echo "========================================"
echo
echo "  Server: ${BUILD_DIR}/pxpt-server"
echo "  Client: ${BUILD_DIR}/pxpt-client"
echo
echo "  To run:"
echo "    Start server: ./${BUILD_DIR}/pxpt-server"
echo "    Start client: ./${BUILD_DIR}/pxpt-client"
echo