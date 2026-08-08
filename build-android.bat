@echo off
setlocal EnableDelayedExpansion

echo ========================================
echo   CGame Racer Android Build (Windows)
echo ========================================
echo.

if "%ANDROID_NDK_HOME%"=="" (
    if exist "%LOCALAPPDATA%\Android\Sdk\ndk" (
        set "ANDROID_NDK_HOME=%LOCALAPPDATA%\Android\Sdk\ndk"
    )
)

if "%ANDROID_NDK_HOME%"=="" (
    echo [ERROR] Android NDK not found.
    echo         Set ANDROID_NDK_HOME environment variable.
    echo         Download from: https://developer.android.com/ndk/downloads
    pause
    exit /b 1
)

echo [INFO] Android NDK: %ANDROID_NDK_HOME%

set "ANDROID_ABI=%~1"
if "%ANDROID_ABI%"=="" set "ANDROID_ABI=arm64-v8a"

set "ANDROID_API=%~2"
if "%ANDROID_API%"=="" set "ANDROID_API=24"

echo [INFO] Building for ABI: %ANDROID_ABI%, API level: %ANDROID_API%

set "TOOLCHAIN_FILE=%ANDROID_NDK_HOME%\build\cmake\android.toolchain.cmake"

if not exist "%TOOLCHAIN_FILE%" (
    echo [ERROR] Android toolchain file not found at %TOOLCHAIN_FILE%
    pause
    exit /b 1
)

if not exist build-android-%ANDROID_ABI% mkdir build-android-%ANDROID_ABI%
cd build-android-%ANDROID_ABI%

echo [INFO] Configuring for Android...
cmake -G Ninja ^
    -DCMAKE_TOOLCHAIN_FILE="%TOOLCHAIN_FILE%" ^
    -DANDROID_ABI="%ANDROID_ABI%" ^
    -DANDROID_PLATFORM=android-%ANDROID_API% ^
    -DANDROID_STL=c++_shared ^
    ..
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Configuration failed.
    cd ..
    pause
    exit /b 1
)

echo.
echo [INFO] Building...
cmake --build .
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Build failed.
    cd ..
    pause
    exit /b 1
)

cd ..

echo.
echo ========================================
echo   Android build complete!
echo ========================================
echo.
echo   Output: build-android-%ANDROID_ABI%\
echo.

pause
endlocal
