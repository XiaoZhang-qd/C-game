#!/usr/bin/env bash
set -e

echo "========================================"
echo "  CGame Racer WebAssembly Build"
echo "========================================"
echo

# Check for Emscripten
if ! command -v emcc &> /dev/null; then
    echo "[ERROR] Emscripten (emcc) not found."
    echo "        Install from: https://emscripten.org/docs/getting_started/downloads.html"
    echo
    echo "        Quick setup:"
    echo "          git clone https://github.com/emscripten-core/emsdk.git"
    echo "          cd emsdk && ./emsdk install latest && ./emsdk activate latest"
    echo "          source ./emsdk_env.sh"
    exit 1
fi

echo "[INFO] emcc found: $(emcc --version | head -1)"

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "${SCRIPT_DIR}"

# Build raylib for Web if not already done
RAYLIB_WASM_LIB="raylib/src/libraylib.a"
if [ ! -f "${RAYLIB_WASM_LIB}" ]; then
    echo "[INFO] Building raylib for WebAssembly..."
    RAYLIB_DIR="${SCRIPT_DIR}/raylib"
    (cd "${RAYLIB_DIR}" && make -j4 PLATFORM=PLATFORM_WEB)
    if [ ! -f "${RAYLIB_WASM_LIB}" ]; then
        echo "[WARN] raylib web build may have failed, attempting generic build..."
        (cd "${RAYLIB_DIR}" && make -j4)
    fi
fi

mkdir -p build-web
cd build-web

echo "[INFO] Configuring for WebAssembly with Emscripten..."
emcmake cmake -G "Unix Makefiles" ..

echo
echo "[INFO] Building WebAssembly client..."
emmake make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)

cd ..

echo
echo "========================================"
echo "  WebAssembly build complete!"
echo "========================================"
echo
echo "  Output: build-web/c-game-client-web.html"
echo
echo "  To serve locally:"
echo "    python3 -m http.server 8000 -d build-web"
echo "  Then open: http://localhost:8000/c-game-client-web.html"
echo
