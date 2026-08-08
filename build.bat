@echo off
setlocal EnableDelayedExpansion

echo ========================================
echo   PXPT Racer Build Script (Windows)
echo   Usage: build.bat [arch]
echo     arch: x64 (default), x86, arm64
echo ========================================
echo.

REM ========================================
REM Parse architecture argument
REM ========================================
set "ARCH=%~1"

if "%ARCH%"=="" goto :native_build
if /i "%ARCH%"=="x64" goto :preset_build
if /i "%ARCH%"=="x86" goto :preset_build
if /i "%ARCH%"=="arm64" goto :preset_build

echo [ERROR] Unsupported architecture "%ARCH%"
echo         Supported: x64, x86, arm64
echo.
goto :eof

REM ========================================
REM Preset-based build (cross or native arch)
REM ========================================
:preset_build
set "PRESET="
if /i "%ARCH%"=="x64" set "PRESET=windows-x64"
if /i "%ARCH%"=="x86" set "PRESET=windows-x86"
if /i "%ARCH%"=="arm64" set "PRESET=windows-arm64"

echo [INFO] Architecture: %ARCH% -^> preset: %PRESET%
echo.

where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [ERROR] cmake not found.
    echo         Install from: https://cmake.org/download/
    pause
    exit /b 1
)

echo [INFO] Configuring with preset: %PRESET%...
cmake --preset %PRESET%
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Configuration failed.
    pause
    exit /b 1
)

echo.
echo [INFO] Building...
cmake --build --preset %PRESET%
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Build failed.
    pause
    exit /b 1
)

REM Determine build directory
set "BUILD_DIR=build"
if /i "%ARCH%"=="x86" set "BUILD_DIR=build-win-x86"
if /i "%ARCH%"=="arm64" set "BUILD_DIR=build-win-arm64"

echo.
echo ========================================
echo   Build successful!
echo ========================================
echo.
echo   Server: %BUILD_DIR%\pxpt-server.exe
echo   Client: %BUILD_DIR%\pxpt-client.exe
echo.
echo   To run:
echo     Start server: %BUILD_DIR%\pxpt-server.exe
echo     Start client: %BUILD_DIR%\pxpt-client.exe
echo.
pause
endlocal
goto :eof

REM ========================================
REM Native build (auto-detect)
REM ========================================
:native_build

where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [ERROR] cmake not found.
    echo         Install from: https://cmake.org/download/
    pause
    exit /b 1
)

set "GENERATOR="

where ninja >nul 2>nul
if %ERRORLEVEL% equ 0 (
    set "GENERATOR=Ninja"
) else (
    where mingw32-make >nul 2>nul
    if %ERRORLEVEL% equ 0 (
        set "GENERATOR=MinGW Makefiles"
    ) else (
        where nmake >nul 2>nul
        if %ERRORLEVEL% equ 0 (
            set "GENERATOR=NMake Makefiles"
        ) else (
            where make >nul 2>nul
            if %ERRORLEVEL% equ 0 (
                set "GENERATOR=MinGW Makefiles"
            )
        )
    )
)

if "%GENERATOR%"=="" (
    echo [WARN] Neither ninja nor make found. Trying default CMake generator...
    set "GENERATOR="
) else (
    echo [INFO] Using generator: %GENERATOR%
)

set "CC_FOUND="
where cl >nul 2>nul
if %ERRORLEVEL% equ 0 (
    set "CC_FOUND=MSVC"
)
if "%CC_FOUND%"=="" (
    where gcc >nul 2>nul
    if %ERRORLEVEL% equ 0 (
        set "CC_FOUND=GCC"
    )
)
if "%CC_FOUND%"=="" (
    where cc >nul 2>nul
    if %ERRORLEVEL% equ 0 (
        set "CC_FOUND=CC"
    )
)

if "%CC_FOUND%"=="" (
    echo [ERROR] No C compiler found.
    echo         Install MSVC ^(Visual Studio^), MinGW-w64, or MSYS2.
    pause
    exit /b 1
)
echo [INFO] C compiler: %CC_FOUND%

if not exist build mkdir build
cd build

if "%GENERATOR%"=="" (
    echo [INFO] Configuring with default generator...
    cmake ..
) else (
    echo [INFO] Configuring with %GENERATOR%...
    cmake -G "%GENERATOR%" ..
)
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Configuration failed.
    cd ..
    pause
    exit /b 1
)

echo.
echo [INFO] Building...
cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Build failed.
    cd ..
    pause
    exit /b 1
)

cd ..

echo.
echo ========================================
echo   Build successful!
echo ========================================
echo.
echo   Server: build\pxpt-server.exe
echo   Client: build\pxpt-client.exe
echo.
echo   To run:
echo     Start server: build\pxpt-server.exe
echo     Start client: build\pxpt-client.exe
echo.

pause
endlocal