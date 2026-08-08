#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "${SCRIPT_DIR}"

# ========================================
# Parse arguments
# ========================================
ACTION="${1:-build}"

if [ "${ACTION}" = "clean" ]; then
    echo "========================================"
    echo "  CGame Racer - Clean All Build Artifacts"
    echo "========================================"
    echo

    CLEAN_TARGET="${2:-all}"

    if [ "${CLEAN_TARGET}" = "all" ] || [ "${CLEAN_TARGET}" = "*" ]; then
        echo "[CLEAN] Cleaning ALL build directories..."
        bash "${SCRIPT_DIR}/build.sh" clean all
        if [ -f "${SCRIPT_DIR}/build.bat" ]; then
            echo "[CLEAN] (Windows users can use: build.bat clean all)"
        fi
        echo "[CLEAN] Done."
        exit 0
    else
        echo "[CLEAN] Cleaning ${CLEAN_TARGET}..."
        bash "${SCRIPT_DIR}/build.sh" clean "${CLEAN_TARGET}"
        exit 0
    fi
fi

if [ "${ACTION}" = "help" ] || [ "${ACTION}" = "-h" ] || [ "${ACTION}" = "--help" ]; then
    echo "========================================"
    echo "  CGame Racer - Build All Platforms"
    echo "========================================"
    echo
    echo "Usage:"
    echo "  ./build-all.sh                  Build all platforms"
    echo "  ./build-all.sh clean [arch]     Clean build artifacts (or 'all')"
    echo "  ./build-all.sh help             Show this help"
    echo
    echo "Examples:"
    echo "  ./build-all.sh                  Build all detected platforms"
    echo "  ./build-all.sh clean all        Clean ALL build artifacts"
    echo "  ./build-all.sh clean x64        Clean x64 build artifacts"
    exit 0
fi

echo "========================================"
echo "  CGame Racer - Build All Platforms"
echo "========================================"
echo

PASSED=0
FAILED=0
SKIPPED=0

build_platform() {
    local name="$1"
    local script="$2"
    local require_cmd="$3"

    echo
    echo "----------------------------------------"
    echo "  Building for ${name}..."
    echo "----------------------------------------"

    if [ -n "${require_cmd}" ]; then
        if ! command -v "${require_cmd}" &> /dev/null; then
            echo "[SKIP] ${require_cmd} not found, skipping ${name} build."
            SKIPPED=$((SKIPPED + 1))
            return 0
        fi
    fi

    if [ -f "${script}" ]; then
        if bash "${script}" 2>&1; then
            echo "[OK] ${name} build succeeded."
            PASSED=$((PASSED + 1))
        else
            echo "[FAIL] ${name} build failed."
            FAILED=$((FAILED + 1))
        fi
    else
        echo "[SKIP] Build script ${script} not found."
        SKIPPED=$((SKIPPED + 1))
    fi
}

# Always try native platform
NATIVE_PLATFORM="Native Desktop"
echo "[INFO] Detected OS: $(uname -s)"

# Desktop (native)
build_platform "${NATIVE_PLATFORM}" "./build.sh" "cmake"

# WebAssembly (requires emscripten)
build_platform "WebAssembly" "./build-wasm.sh" "emcc"

# Android (requires NDK)
if [ -n "${ANDROID_NDK_HOME}" ] || [ -d "${HOME}/Android/Sdk/ndk" ]; then
    build_platform "Android (arm64-v8a)" "./build-android.sh" "cmake"
else
    echo "[SKIP] ANDROID_NDK_HOME not set, skipping Android build."
    SKIPPED=$((SKIPPED + 1))
fi

# iOS (macOS only)
if [ "$(uname)" = "Darwin" ]; then
    build_platform "iOS" "./build-ios.sh" "xcodebuild"
else
    echo "[SKIP] iOS build requires macOS, skipping."
    SKIPPED=$((SKIPPED + 1))
fi

# HarmonyOS
if [ -n "${HOS_SDK_HOME}" ] || [ -d "${HOME}/HarmonyOS-NEXT-DB1" ]; then
    build_platform "HarmonyOS" "./build-harmonyos.sh" "cmake"
else
    echo "[SKIP] HOS_SDK_HOME not set, skipping HarmonyOS build."
    SKIPPED=$((SKIPPED + 1))
fi

echo
echo "========================================"
echo "  Build Summary"
echo "========================================"
echo "  Passed:  ${PASSED}"
echo "  Failed:  ${FAILED}"
echo "  Skipped: ${SKIPPED}"
echo

if [ ${FAILED} -gt 0 ]; then
    exit 1
fi

echo "  All selected platforms built successfully!"
