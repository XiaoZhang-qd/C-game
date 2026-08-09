#!/usr/bin/env bash
set -e

echo "========================================"
echo "  CGame Racer Android Build"
echo "========================================"
echo

# Check for Android NDK
if [ -z "${ANDROID_NDK_HOME}" ]; then
    if [ -d "${HOME}/Android/Sdk/ndk" ]; then
        export ANDROID_NDK_HOME="${HOME}/Android/Sdk/ndk"
    elif [ -d "${HOME}/android-ndk" ]; then
        export ANDROID_NDK_HOME="${HOME}/android-ndk"
    elif [ -d "/opt/android-ndk" ]; then
        export ANDROID_NDK_HOME="/opt/android-ndk"
    fi
fi

if [ -z "${ANDROID_NDK_HOME}" ] || [ ! -d "${ANDROID_NDK_HOME}" ]; then
    echo "[ERROR] Android NDK not found."
    echo "        Set ANDROID_NDK_HOME environment variable."
    echo "        Download from: https://developer.android.com/ndk/downloads"
    exit 1
fi

echo "[INFO] Android NDK: ${ANDROID_NDK_HOME}"

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "${SCRIPT_DIR}"

ANDROID_ABI="${1:-arm64-v8a}"
ANDROID_API="${2:-24}"

echo "[INFO] Building for ABI: ${ANDROID_ABI}, API level: ${ANDROID_API}"

TOOLCHAIN_FILE="${ANDROID_NDK_HOME}/build/cmake/android.toolchain.cmake"

if [ ! -f "${TOOLCHAIN_FILE}" ]; then
    echo "[ERROR] Android toolchain file not found at ${TOOLCHAIN_FILE}"
    exit 1
fi

mkdir -p build-android-${ANDROID_ABI}
cd build-android-${ANDROID_ABI}

echo "[INFO] Configuring for Android..."
cmake -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN_FILE}" \
    -DANDROID_ABI="${ANDROID_ABI}" \
    -DANDROID_PLATFORM=android-${ANDROID_API} \
    -DANDROID_STL=c++_shared \
    ..

echo
echo "[INFO] Building..."
cmake --build .

cd ..

echo
echo "========================================"
echo "  Android build complete!"
echo "========================================"
echo
echo "  Output: build-android-${ANDROID_ABI}/"
echo
echo "  To use in Android Studio:"
echo "    1. Create new Android project or import existing"
echo "    2. Add CMakeLists.txt reference to jniLibs"
echo "    3. Copy .so files to app/src/main/jniLibs/${ANDROID_ABI}/"
echo
