#!/usr/bin/env bash
set -e

echo "========================================"
echo "  PXPT Racer Build Script (Linux/macOS)"
echo "  Usage: ./build.sh [arch]"
echo "    arch: x64 (default), x86, arm64, arm32, riscv64, riscv32,"
echo "          ppc64, ppc64le, ppc32, s390x, mips64, mips64le,"
echo "          mips32, mips32le, sparc64, universal (macOS)"
echo "========================================"
echo

# ========================================
# Parse architecture argument
# ========================================
ARCH="${1:-}"

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "${SCRIPT_DIR}"

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