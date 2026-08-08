@echo off
setlocal EnableDelayedExpansion

echo ========================================
echo   CGame Racer WebAssembly Build (Windows)
echo ========================================
echo.

where emcc >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Emscripten ^(emcc^) not found.
    echo         Install from: https://emscripten.org/docs/getting_started/downloads.html
    echo.
    echo         Quick setup:
    echo           git clone https://github.com/emscripten-core/emsdk.git
    echo           cd emsdk
    echo           emsdk install latest
    echo           emsdk activate latest
    echo           emsdk_env.bat
    pause
    exit /b 1
)

echo [INFO] emcc found.

if not exist build-web mkdir build-web
cd build-web

echo [INFO] Configuring for WebAssembly...
emcmake cmake -G "MinGW Makefiles" ..
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Configuration failed.
    cd ..
    pause
    exit /b 1
)

echo.
echo [INFO] Building...
emmake mingw32-make -j%NUMBER_OF_PROCESSORS%
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Build failed.
    cd ..
    pause
    exit /b 1
)

cd ..

echo.
echo ========================================
echo   WebAssembly build complete!
echo ========================================
echo.
echo   Output: build-web\c-game-client-web.html
echo.
echo   To serve locally:
echo     python -m http.server 8000 -d build-web
echo   Then open: http://localhost:8000/c-game-client-web.html
echo.

pause
endlocal
