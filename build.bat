@echo off

REM ========================================
REM Parse arguments
REM ========================================
set "ACTION=%~1"
set "ARG2=%~2"

if /i "%ACTION%"=="help" goto :show_help
if /i "%ACTION%"=="-h" goto :show_help
if /i "%ACTION%"=="--help" goto :show_help
if /i "%ACTION%"=="clean" goto :clean_action

setlocal EnableDelayedExpansion

REM ========================================
REM Build mode header
REM ========================================
echo ========================================
echo   CGame Racer Build Script (Windows)
echo   Usage: build.bat [arch]
echo     arch: x64 (default), x86, arm64
echo ========================================
echo.

REM Build mode: ACTION is the arch
set "ARCH=%ACTION%"

if "%ARCH%"=="" goto :native_build
if /i "%ARCH%"=="x64" goto :preset_build
if /i "%ARCH%"=="x86" goto :preset_build
if /i "%ARCH%"=="arm64" goto :preset_build

echo [ERROR] Unsupported architecture "%ARCH%"
echo         Supported: x64, x86, arm64
echo.
goto :eof

REM ========================================
REM Show help
REM ========================================
:show_help
echo.
echo   CGame Racer Build Script (Windows)
echo   Usage:
echo     build.bat [arch]                  Build (default)
echo     build.bat clean [arch]            Clean specific arch build
echo     build.bat clean all               Clean ALL build directories
echo     build.bat help                    Show this help
echo.
echo   arch: x64 (default), x86, arm64
echo.
echo   Examples:
echo     build.bat              Build native architecture
echo     build.bat x64          Build for x64
echo     build.bat clean        Clean x64 build (default)
echo     build.bat clean x86    Clean x86 build
echo     build.bat clean all    Clean all build directories
echo.
goto :eof

REM ========================================
REM Clean action
REM ========================================
:clean_action
set "CLEAN_ARCH=%ARG2%"

if /i "%CLEAN_ARCH%"=="all" goto :clean_all
if /i "%CLEAN_ARCH%"=="*" goto :clean_all

if not defined CLEAN_ARCH echo [CLEAN] No architecture specified, cleaning default: x64& set "CLEAN_ARCH=x64"

set "CLEAN_DIR="
if /i "%CLEAN_ARCH%"=="x64" set "CLEAN_DIR=build"
if /i "%CLEAN_ARCH%"=="x86" set "CLEAN_DIR=build-win32"
if /i "%CLEAN_ARCH%"=="arm64" set "CLEAN_DIR=build-win-arm64"

if not defined CLEAN_DIR echo [ERROR] Unknown architecture "%CLEAN_ARCH%"& echo         Supported: x64, x86, arm64& goto :eof

echo [CLEAN] Cleaning %CLEAN_ARCH% -^> %CLEAN_DIR%
if not exist "%CLEAN_DIR%" echo [CLEAN] Skipping (not found): %CLEAN_DIR%& goto :eof
echo [CLEAN] Removing %CLEAN_DIR%...
rmdir /s /q "%CLEAN_DIR%"
echo [CLEAN] Done: %CLEAN_DIR%
goto :eof

:clean_all
echo ========================================
echo   Cleaning ALL build artifacts
echo ========================================
echo.

call :clean_one build
call :clean_one build-win32
call :clean_one build-win-arm64
call :clean_one build-linux-x86
call :clean_one build-linux-arm32
call :clean_one build-linux-arm64
call :clean_one build-linux-riscv64
call :clean_one build-linux-riscv32
call :clean_one build-linux-ppc64
call :clean_one build-linux-ppc64le
call :clean_one build-linux-ppc32
call :clean_one build-linux-s390x
call :clean_one build-linux-mips64
call :clean_one build-linux-mips64le
call :clean_one build-linux-mips32
call :clean_one build-linux-mips32le
call :clean_one build-linux-sparc64
call :clean_one build-drm
call :clean_one build-chromeos
call :clean_one build-chromeos-x64
call :clean_one build-macos-x64
call :clean_one build-macos-arm64
call :clean_one build-macos-universal
call :clean_one build-freebsd
call :clean_one build-openbsd
call :clean_one build-netbsd
call :clean_one build-dragonfly
call :clean_one build-android-arm64
call :clean_one build-android-armeabi-v7a
call :clean_one build-android-x86
call :clean_one build-android-x86_64
call :clean_one build-ios
call :clean_one build-ios-theos
call :clean_one build-ios-sim
call :clean_one build-ios-sim-theos
call :clean_one build-web
call :clean_one build-harmonyos-arm64
call :clean_one build-mingw-x64
call :clean_one build-mingw-x86
echo.
echo [CLEAN] All build artifacts cleaned.
goto :eof

:clean_one
if not exist "%~1" echo [CLEAN] Skipping (not found): %~1& goto :eof
echo [CLEAN] Removing %~1...
rmdir /s /q "%~1"
echo [CLEAN] Done: %~1
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
echo   Server: %BUILD_DIR%\c-game-server.exe
echo   Client: %BUILD_DIR%\c-game-client.exe
echo.
echo   To run:
echo     Start server: %BUILD_DIR%\c-game-server.exe
echo     Start client: %BUILD_DIR%\c-game-client.exe
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
echo   Server: build\c-game-server.exe
echo   Client: build\c-game-client.exe
echo.
echo   To run:
echo     Start server: build\c-game-server.exe
echo     Start client: build\c-game-client.exe
echo.

pause
endlocal