#!/usr/bin/env bash
set -e

echo "========================================"
echo "  PXPT Racer Chrome OS Build"
echo "========================================"
echo
echo "  Chrome OS supports two build approaches:"
echo "    1. Native (via Crostini/Linux) — uses build.sh"
echo "    2. Web (via browser) — uses build-wasm.sh"
echo

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "${SCRIPT_DIR}"

MODE="${1:-all}"

case "${MODE}" in
    native)
        echo "[INFO] Building native version for Crostini/Linux..."
        bash ./build.sh
        ;;
    web)
        echo "[INFO] Building WebAssembly version for Chrome browser..."
        bash ./build-wasm.sh
        ;;
    all)
        echo "[INFO] Building both native and web versions..."
        echo
        echo "=== Native Build ==="
        bash ./build.sh
        echo
        echo "=== WebAssembly Build ==="
        bash ./build-wasm.sh
        ;;
    *)
        echo "Usage: $0 [native|web|all]"
        echo "  native - Build native Linux binary (runs in Crostini)"
        echo "  web    - Build WebAssembly (runs in Chrome browser)"
        echo "  all    - Build both (default)"
        exit 1
        ;;
esac

echo
echo "========================================"
echo "  Chrome OS build complete!"
echo "========================================"
echo
echo "  For native (Crostini):"
echo "    ./build/pxpt-server"
echo "    ./build/pxpt-client"
echo
echo "  For web (Chrome browser):"
echo "    python3 -m http.server 8000 -d build-web"
echo "    Open: http://localhost:8000/pxpt-client-web.html"
echo
