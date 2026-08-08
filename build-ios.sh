#!/usr/bin/env bash
set -e

echo "========================================"
echo "  CGame Racer iOS Build"
echo "========================================"
echo

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "${SCRIPT_DIR}"

IOS_ARCH="${1:-arm64}"
IOS_MIN_VER="${2:-13.0}"

# ========================================
# Determine build method
# ========================================
IS_MACOS=0
if [[ "$(uname)" == "Darwin" ]]; then
    IS_MACOS=1
fi

if [[ "${IS_MACOS}" -eq 1 ]]; then
    echo "[INFO] macOS detected. Using native Xcode build."
    echo "[INFO] Building for iOS: arch=${IOS_ARCH}, min version=${IOS_MIN_VER}"

    if ! command -v xcodebuild &> /dev/null; then
        echo "[ERROR] xcodebuild not found. Install Xcode from the App Store."
        echo "        Then run: xcode-select --install"
        exit 1
    fi

    if ! command -v cmake &> /dev/null; then
        echo "[ERROR] cmake not found. Install via: brew install cmake"
        exit 1
    fi

    if [ "${IOS_ARCH}" = "arm64-simulator" ] || [ "${IOS_ARCH}" = "x86_64" ]; then
        IOS_PLATFORM="SIMULATOR"
        IOS_ARCH_NAME="${IOS_ARCH}"
    else
        IOS_PLATFORM="OS"
        IOS_ARCH_NAME="${IOS_ARCH}"
    fi

    mkdir -p build-ios
    cd build-ios

    echo "[INFO] Configuring for iOS (${IOS_PLATFORM})..."
    cmake -G Xcode \
        -DCMAKE_SYSTEM_NAME=iOS \
        -DCMAKE_SYSTEM_PROCESSOR=${IOS_ARCH_NAME} \
        -DCMAKE_OSX_SYSROOT=iphone${IOS_PLATFORM} \
        -DCMAKE_OSX_ARCHITECTURES=${IOS_ARCH_NAME} \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=${IOS_MIN_VER} \
        ..

    echo
    echo "[INFO] Building..."
    cmake --build . --config Release

    cd ..

    echo
    echo "========================================"
    echo "  iOS build complete! (Xcode)"
    echo "========================================"
    echo
    echo "  Output: build-ios/"
    echo
    echo "  To deploy to device:"
    echo "    1. Open build-ios/CGAME_Racer.xcodeproj in Xcode"
    echo "    2. Select your target device"
    echo "    3. Press Run"
    echo
    echo "  Command line deploy:"
    echo "    xcodebuild -project build-ios/CGAME_Racer.xcodeproj \\"
    echo "      -scheme c-game-client \\"
    echo "      -destination 'generic/platform=iOS' \\"
    echo "      build"
    echo
else
    echo "[INFO] Non-macOS platform detected."
    echo "[INFO] Using Theos SDKs for cross-compilation."
    echo "[INFO] Building for iOS: arch=${IOS_ARCH}, min version=${IOS_MIN_VER}"

    # Check for Theos SDKs
    THEOS_SDKS="${THEOS:-$HOME/theos/sdks}"

    if [ ! -d "${THEOS_SDKS}" ]; then
        echo "[ERROR] Theos SDKs not found at ${THEOS_SDKS}"
        echo
        echo "  To set up Theos SDKs for cross-platform iOS development:"
        echo
        echo "    1. Clone the patched SDKs:"
        echo "       git clone https://github.com/theos/sdks.git ~/theos/sdks"
        echo
        echo "    2. Or set THEOS environment variable to your Theos install:"
        echo "       export THEOS=/path/to/theos"
        echo
        echo "    3. The patched SDKs include private framework .tbd files"
        echo "       that were removed from official SDKs starting Xcode 7.3"
        echo
        echo "    4. Create patched SDKs using create_patched_sdk.sh"
        echo
        exit 1
    fi

    # Find the appropriate SDK
    SDK_NAME="iPhoneOS${IOS_MIN_VER}"
    SDK_PATH="${THEOS_SDKS}/${SDK_NAME}"

    # Try to find available SDKs
    if [ ! -d "${SDK_PATH}" ]; then
        AVAILABLE_SDKS=$(ls -d ${THEOS_SDKS}/iPhoneOS* 2>/dev/null | sort -V || true)
        if [ -z "${AVAILABLE_SDKS}" ]; then
            echo "[ERROR] No iPhoneOS SDK found in ${THEOS_SDKS}"
            echo "        Available SDKs in theos/sdks:"
            ls -1 ${THEOS_SDKS}/ 2>/dev/null || echo "  (none)"
            exit 1
        fi
        # Use the latest available SDK
        SDK_PATH=$(echo "${AVAILABLE_SDKS}" | tail -1)
        SDK_NAME=$(basename "${SDK_PATH}")
        echo "[WARN] SDK ${SDK_NAME} not found. Using latest: ${SDK_NAME}"
    fi

    echo "[INFO] Using SDK: ${SDK_NAME}"

    # Cross-compiler settings
    IOS_VERSION="${IOS_MIN_VER}"
    SDKVER="${SDK_NAME#iPhoneOS}"
    SDKVER="${SDKVER%%.*}"

    # Determine the cross compiler
    if command -v aarch64-apple-ios${SDKVER}-clang &> /dev/null; then
        CC="aarch64-apple-ios${SDKVER}-clang"
        CXX="aarch64-apple-ios${SDKVER}-clang++"
    elif command -v arm64-apple-darwin${SDKVER}-clang &> /dev/null; then
        CC="arm64-apple-darwin${SDKVER}-clang"
        CXX="arm64-apple-darwin${SDKVER}-clang++"
    elif command -v clang &> /dev/null; then
        CC="clang"
        CXX="clang++"
    else
        echo "[ERROR] No cross-compiler found for iOS."
        echo "        Install aarch64-apple-ios cross toolchain or use clang with appropriate flags."
        exit 1
    fi

    echo "[INFO] Cross compiler: ${CC}"

    mkdir -p build-ios
    cd build-ios

    echo "[INFO] Configuring for iOS (Theos SDKs, cross-compile)..."

    # CMake toolchain file for iOS cross-compilation
    cat > ios-toolchain.cmake << 'TOOLCHAIN_EOF'
set(CMAKE_SYSTEM_NAME Darwin)
set(CMAKE_SYSTEM_PROCESSOR arm64)
set(CMAKE_CROSSCOMPILING TRUE)

set(CMAKE_C_COMPILER ${CROSS_CC})
set(CMAKE_CXX_COMPILER ${CROSS_CXX})
set(CMAKE_AR ar CACHE FILEPATH "" FORCE)
set(CMAKE_RANLIB ranlib CACHE FILEPATH "" FORCE)
set(CMAKE_STRIP strip CACHE FILEPATH "" FORCE)
set(CMAKE_LINKER ${CROSS_LD})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
    TOOLCHAIN_EOF

    # Set up SDK paths
    SDK_INCLUDE="${SDK_PATH}/usr/include"
    SDK_LIB="${SDK_PATH}/usr/lib"
    FRAMEWORKS="${SDK_PATH}/System/Library/Frameworks"

    # Compile with SDK sysroot
    ${CC} \
        -isysroot "${SDK_PATH}" \
        -I"${SDK_INCLUDE}" \
        -I"${SDK_INCLUDE}/c++" \
        -I"${SDK_INCLUDE}/c++/backward" \
        -I"${SDK_INCLUDE}/c++/arm64-apple-ios${SDKVER}" \
        -L"${SDK_LIB}" \
        -L"${SDK_LIB}/clang/${SDKVER}/lib" \
        -F"${FRAMEWORKS}" \
        -target arm64-apple-ios${SDKVER} \
        -c ../common/socket.c -o socket.o

    echo
    echo "[INFO] Cross-compilation with Theos SDKs completed!"

    cd ..

    echo
    echo "========================================"
    echo "  iOS build complete! (Theos SDKs cross-compile)"
    echo "========================================"
    echo
    echo "  SDK used: ${SDK_NAME}"
    echo "  Cross compiler: ${CC}"
    echo
    echo "  Next steps to create IPA:"
    echo "    1. Compile all source files: bash build-ios-theos.sh"
    echo "    2. Create .app bundle with Info.plist"
    echo "    3. Copy built binary into .app/"
    echo "    4. Zip into .ipa: zip -r c-game-client.ipa Payload/"
    echo
    echo "  For a complete IPA workflow, see build-ios-theos.sh"
    echo
fi
