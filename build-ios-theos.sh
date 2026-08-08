#!/usr/bin/env bash
set -e

echo "========================================"
echo "  PXPT Racer iOS Build (Theos SDKs)"
echo "  Cross-platform iOS compilation + IPA packaging"
echo "========================================"
echo

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "${SCRIPT_DIR}"

IOS_ARCH="${1:-arm64}"
IOS_MIN_VER="${2:-13.0}"
BUNDLE_ID="${3:-com.pxpt.racer}"
BUNDLE_NAME="${4:-PXPT Racer}"

# ========================================
# Setup Theos SDKs
# ========================================

THEOS_SDKS="${THEOS:-$HOME/theos/sdks}"

if [ ! -d "${THEOS_SDKS}" ]; then
    echo "[ERROR] Theos SDKs not found at ${THEOS_SDKS}"
    echo
    echo "  Setup steps:"
    echo "    git clone https://github.com/theos/sdks.git ~/theos/sdks"
    echo "    # Or set THEOS env var:"
    echo "    export THEOS=/path/to/theos"
    echo
    echo "  Note: The Theos SDKs repository contains patched iOS SDKs"
    echo "  with private framework .tbd files removed from official SDKs"
    echo "  starting in Xcode 7.3 / iOS 9.3."
    exit 1
fi

# Find the appropriate SDK
SDK_NAME="iPhoneOS${IOS_MIN_VER}"
SDK_PATH="${THEOS_SDKS}/${SDK_NAME}"

if [ ! -d "${SDK_PATH}" ]; then
    AVAILABLE_SDKS=$(ls -d ${THEOS_SDKS}/iPhoneOS* 2>/dev/null | sort -V || true)
    if [ -z "${AVAILABLE_SDKS}" ]; then
        echo "[ERROR] No iPhoneOS SDK found in ${THEOS_SDKS}"
        echo "        Available:"
        ls -1 ${THEOS_SDKS}/ 2>/dev/null || echo "  (none)"
        exit 1
    fi
    SDK_PATH=$(echo "${AVAILABLE_SDKS}" | tail -1)
    SDK_NAME=$(basename "${SDK_PATH}")
    echo "[WARN] SDK ${SDK_NAME} not found. Using latest: ${SDK_NAME}"
fi

echo "[INFO] Using SDK: ${SDK_NAME}"

# Extract SDK version
SDKVER="${SDK_NAME#iPhoneOS}"
SDKVER="${SDKVER%%.*}"
IOS_VERSION="${SDK_NAME#iPhone}"

# ========================================
# Find cross-compiler
# ========================================

CC=""
CXX=""
LDFLAGS=""

if command -v aarch64-apple-ios${SDKVER}-clang &> /dev/null; then
    CC="aarch64-apple-ios${SDKVER}-clang"
    CXX="aarch64-apple-ios${SDKVER}-clang++"
    echo "[INFO] Using Theos cross-compiler: ${CC}"
elif command -v arm64-apple-darwin${SDKVER}-clang &> /dev/null; then
    CC="arm64-apple-darwin${SDKVER}-clang"
    CXX="arm64-apple-darwin${SDKVER}-clang++"
    echo "[INFO] Using cross-compiler: ${CC}"
elif command -v clang &> /dev/null; then
    CC="clang"
    CXX="clang++"
    echo "[INFO] Using clang with iOS target triple"
else
    echo "[ERROR] No suitable cross-compiler found."
    exit 1
fi

# ========================================
# SDK paths
# ========================================

SDK_INCLUDE="${SDK_PATH}/usr/include"
SDK_LIB="${SDK_PATH}/usr/lib"
FRAMEWORKS="${SDK_PATH}/System/Library/Frameworks"
CLANG_SDK_LIB="${SDK_PATH}/usr/lib/clang/${SDKVER}/lib"

# ========================================
# Build all source files
# ========================================

BUILD_DIR="${SCRIPT_DIR}/build-ios-theos"
rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

echo
echo "[INFO] Compiling sources for iOS (${IOS_ARCH})..."

SOURCES=(
    "../common/socket.c"
    "../server/main.c"
    "../server/server.c"
    "../client/main.c"
    "../client/gui.c"
    "../client/game.c"
    "../client/network.c"
    "../client/raygui_impl.c"
)

INCLUDES=(
    "-I${SCRIPT_DIR}/common"
    "-I${SCRIPT_DIR}/client"
    "-I${SCRIPT_DIR}/server"
    "-I${SCRIPT_DIR}/raylib/src"
    "-I${SCRIPT_DIR}/raygui-5.0/src"
    "-I${SDK_INCLUDE}"
    "-I${SDK_INCLUDE}/c++"
    "-I${SDK_INCLUDE}/c++/backward"
    "-I${SDK_INCLUDE}/c++/v1"
)

TARGET_TRIPLE="arm64-apple-ios${SDKVER}"

COMMON_FLAGS=(
    "-arch arm64"
    "-isysroot ${SDK_PATH}"
    "-target ${TARGET_TRIPLE}"
    "-miphoneos-version-min=${IOS_VERSION}"
    "-fembed-bitcode"
    "-O2"
    "-DNDEBUG"
    "-DPXPT_IOS=1"
)

# Compile each source
for src in "${SOURCES[@]}"; do
    BASENAME=$(basename "${src}" .c)
    echo "  CC  ${BASENAME}.c"
    ${CC} "${COMMON_FLAGS[@]}" "${INCLUDES[@]}" \
        -c "${src}" -o "${BASENAME}.o" \
        2>&1 || { echo "[ERROR] Failed to compile ${src}"; exit 1; }
done

echo
echo "[INFO] Compiling raylib (if needed)..."
# Check if we need to build raylib for iOS
if [ -f "${SCRIPT_DIR}/raylib/src/libraylib.a" ]; then
    echo "  [INFO] Using pre-built raylib"
    RAYLIB_LIB="${SCRIPT_DIR}/raylib/src/libraylib.a"
else
    echo "  [INFO] Building raylib for iOS..."
    cd "${SCRIPT_DIR}/raylib"
    make -j4 PLATFORM=PLATFORM_IOS \
        CC="${CC}" \
        AR="ar" \
        CFLAGS="${COMMON_FLAGS[*]} ${INCLUDES[*]}" \
        2>&1 || { echo "[ERROR] Failed to build raylib for iOS"; exit 1; }
    cd "${BUILD_DIR}"
    RAYLIB_LIB="${SCRIPT_DIR}/raylib/src/libraylib.a"
fi

# ========================================
# Link the client binary
# ========================================

echo
echo "[INFO] Linking pxpt-client..."

CLIENT_OBJS=$(ls client_*.o 2>/dev/null || true)
GAME_OBJS=$(ls game_*.o 2>/dev/null || true)
NET_OBJS=$(ls network_*.o 2>/dev/null || true)
GUI_OBJS=$(ls gui_*.o 2>/dev/null || true)
RAYGUI_OBJS=$(ls raygui_*.o 2>/dev/null || true)
SOCKET_OBJS=$(ls socket_*.o 2>/dev/null || true)

# Collect all .o files except server ones
CLIENT_FILES=$(ls *.o | grep -v "^server_" || true)

if [ -z "${CLIENT_FILES}" ]; then
    echo "[ERROR] No compiled object files found for client."
    exit 1
fi

LINK_CMD="${CC} ${COMMON_FLAGS[*]}"
for obj in ${CLIENT_FILES}; do
    LINK_CMD="${LINK_CMD} ${obj}"
done
LINK_CMD="${LINK_CMD} ${RAYLIB_LIB}"
LINK_CMD="${LINK_CMD} -L${SDK_LIB} -L${CLANG_SDK_LIB} -F${FRAMEWORKS}"
LINK_CMD="${LINK_CMD} -framework Foundation -framework UIKit -framework CoreGraphics"
LINK_CMD="${LINK_CMD} -framework CoreMotion -framework OpenGLES -framework QuartzCore"
LINK_CMD="${LINK_CMD} -lpthread -lm -o pxpt-client"

eval ${LINK_CMD} 2>&1 || { echo "[ERROR] Linking failed"; exit 1; }

echo "  [OK] pxpt-client linked successfully."

# ========================================
# Link the server binary (for reference/desktop)
# ========================================

echo
echo "[INFO] Linking pxpt-server (desktop, for reference)..."

SERVER_OBJS=$(ls server_*.o socket_*.o 2>/dev/null || true)
if [ -n "${SERVER_OBJS}" ]; then
    LINK_SERVER="${CC} ${COMMON_FLAGS[*]}"
    for obj in ${SERVER_OBJS}; do
        LINK_SERVER="${LINK_SERVER} ${obj}"
    done
    LINK_SERVER="${LINK_SERVER} -L${SDK_LIB} -L${CLANG_SDK_LIB} -F${FRAMEWORKS}"
    LINK_SERVER="${LINK_SERVER} -lsystem -lpthread -lm -o pxpt-server-ios"
    eval ${LINK_SERVER} 2>&1 || echo "  [WARN] Server linking failed (may not be needed on iOS)"
fi

# ========================================
# Create .app bundle
# ========================================

echo
echo "[INFO] Creating .app bundle..."

APP_DIR="Payload/PXPT Racer.app"
rm -rf "Payload"
mkdir -p "${APP_DIR}"

# Copy binary
cp pxpt-client "${APP_DIR}/PXPT Racer"

# Set executable permissions
chmod +x "${APP_DIR}/PXPT Racer"

# Create Info.plist
cat > "${APP_DIR}/Info.plist" << PLIST_EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN"
  "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key>
    <string>en</string>
    <key>CFBundleDisplayName</key>
    <string>${BUNDLE_NAME}</string>
    <key>CFBundleExecutable</key>
    <string>PXPT Racer</string>
    <key>CFBundleIdentifier</key>
    <string>${BUNDLE_ID}</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleName</key>
    <string>${BUNDLE_NAME}</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0.0</string>
    <key>CFBundleVersion</key>
    <string>1.0</string>
    <key>LSRequiresIPhoneOS</key>
    <true/>
    <key>UIDeviceFamily</key>
    <array>
        <integer>1</integer>
        <integer>2</integer>
    </array>
    <key>UILaunchScreen</key>
    <dict/>
    <key>UIRequiredDeviceCapabilities</key>
    <array>
        <string>armv7</string>
    </array>
    <key>UISupportedInterfaceOrientations</key>
    <array>
        <string>UIInterfaceOrientationPortrait</string>
        <string>UIInterfaceOrientationLandscapeLeft</string>
        <string>UIInterfaceOrientationLandscapeRight</string>
    </array>
</dict>
</plist>
PLIST_EOF

# Create PkgInfo
echo "APPL????" > "${APP_DIR}/PkgInfo"

# ========================================
# Create IPA
# ========================================

echo
echo "[INFO] Packaging IPA..."

IPA_NAME="pxpt-client-${IOS_ARCH}-ios${SDKVER}.ipa"
zip -r "${IPA_NAME}" "Payload/" 2>&1

IPA_SIZE=$(du -h "${IPA_NAME}" | cut -f1)

# ========================================
# Summary
# ========================================

cd "${SCRIPT_DIR}"

echo
echo "========================================"
echo "  iOS Build Complete! (Theos SDKs)"
echo "========================================"
echo
echo "  SDK:         ${SDK_NAME}"
echo "  Compiler:    ${CC}"
echo "  Architecture: ${IOS_ARCH}"
echo "  IPA:         build-ios-theos/${IPA_NAME} (${IPA_SIZE})"
echo
echo "  .app bundle: build-ios-theos/Payload/PXPT Racer.app/"
echo
echo "  To install on jailbroken device:"
echo "    1. Transfer .ipa to device via sftp/ssh"
echo "    2. Install with: dpkg -i ${IPA_NAME} (after unpacking)"
echo "       Or use sideloading tools like TrollStore/AltStore"
echo
echo "  To install on non-jailbroken device (via Xcode on Mac):"
echo "    Use the native Xcode build path (build-ios.sh on macOS)"
echo
