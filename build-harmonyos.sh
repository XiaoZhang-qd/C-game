#!/usr/bin/env bash
set -e

echo "========================================"
echo "  PXPT Racer HarmonyOS Build"
echo "========================================"
echo

# HarmonyOS uses the OHOS Native Toolkit (similar to NDK)
if [ -z "${HOS_SDK_HOME}" ]; then
    if [ -d "${HOME}/HarmonyOS-NEXT-DB1" ]; then
        export HOS_SDK_HOME="${HOME}/HarmonyOS-NEXT-DB1"
    elif [ -d "${HOME}/ohos-sdk" ]; then
        export HOS_SDK_HOME="${HOME}/ohos-sdk"
    elif [ -d "/opt/harmonyos-sdk" ]; then
        export HOS_SDK_HOME="/opt/harmonyos-sdk"
    fi
fi

if [ -z "${HOS_SDK_HOME}" ] || [ ! -d "${HOS_SDK_HOME}" ]; then
    echo "[ERROR] HarmonyOS SDK not found."
    echo "        Set HOS_SDK_HOME environment variable."
    echo "        Download from: https://developer.harmonyos.com/cn/docs/develop/sdk"
    echo
    echo "        For OpenHarmony (alternative):"
    echo "          https://gitee.com/openharmony"
    exit 1
fi

echo "[INFO] HarmonyOS SDK: ${HOS_SDK_HOME}"

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "${SCRIPT_DIR}"

HOS_ARCH="${1:-arm64-v8a}"
HOS_API="${2:-12}"

echo "[INFO] Building for HarmonyOS: arch=${HOS_ARCH}, API=${HOS_API}"

# HarmonyOS uses CMake toolchain similar to Android NDK
TOOLCHAIN_FILE="${HOS_SDK_HOME}/native/llvm/build/cmake/ohos.toolchain.cmake"

if [ ! -f "${TOOLCHAIN_FILE}" ]; then
    echo "[WARN] HarmonyOS toolchain not found at ${TOOLCHAIN_FILE}"
    echo "[INFO] Attempting to locate toolchain..."
    TOOLCHAIN_FILE=$(find "${HOS_SDK_HOME}" -name "ohos.toolchain.cmake" 2>/dev/null | head -1)
    if [ -z "${TOOLCHAIN_FILE}" ]; then
        echo "[ERROR] Cannot find ohos.toolchain.cmake"
        exit 1
    fi
fi

echo "[INFO] Toolchain: ${TOOLCHAIN_FILE}"

mkdir -p build-harmonyos-${HOS_ARCH}
cd build-harmonyos-${HOS_ARCH}

echo "[INFO] Configuring for HarmonyOS..."
cmake -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN_FILE}" \
    -DOHOS_ARCH="${HOS_ARCH}" \
    -DOHOS_PLATFORM=ohos-${HOS_API} \
    -DCMAKE_BUILD_TYPE=Release \
    ..

echo
echo "[INFO] Building..."
cmake --build .

cd ..

echo
echo "========================================"
echo "  HarmonyOS build complete!"
echo "========================================"
echo
echo "  Output: build-harmonyos-${HOS_ARCH}/"
echo
echo "  To create a HAP package:"
echo "    1. Create an HAP project in DevEco Studio"
echo "    2. Copy .so files to entry/src/main/libs/${HOS_ARCH}/"
echo "    3. Create or update module.json5"
echo "    4. Build HAP via DevEco Studio or hvigorw"
echo
