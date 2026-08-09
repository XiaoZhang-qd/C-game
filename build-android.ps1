# ========================================
# CGame Racer Android Build (PowerShell)
# Works on Windows, Linux, and macOS
# ========================================

$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  CGame Racer Android Build" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Locate Android NDK
$ndkHome = $env:ANDROID_NDK_HOME
if (-not $ndkHome) {
    if ($IsWindows -or $env:OS -eq "Windows_NT") {
        $ndkPath = Join-Path $env:LOCALAPPDATA "Android\Sdk\ndk"
        if (Test-Path $ndkPath) { $ndkHome = $ndkPath }
    } elseif ($IsLinux -or $IsMacOS) {
        $ndkPath = Join-Path $HOME "Android/Sdk/ndk"
        if (Test-Path $ndkPath) { $ndkHome = $ndkPath }
    }
}

if (-not $ndkHome -or -not (Test-Path $ndkHome)) {
    Write-Host "[ERROR] Android NDK not found." -ForegroundColor Red
    Write-Host "        Set ANDROID_NDK_HOME environment variable."
    Write-Host "        Download from: https://developer.android.com/ndk/downloads"
    exit 1
}

Write-Host "[INFO] Android NDK: $ndkHome" -ForegroundColor Green

$abi = if ($args.Count -gt 0) { $args[0] } else { "arm64-v8a" }
$api = if ($args.Count -gt 1) { $args[1] } else { "24" }

Write-Host "[INFO] Building for ABI: $abi, API level: $api" -ForegroundColor Cyan

$toolchainFile = Join-Path $ndkHome "build/cmake/android.toolchain.cmake"
if (-not (Test-Path $toolchainFile)) {
    Write-Host "[ERROR] Android toolchain file not found at $toolchainFile" -ForegroundColor Red
    exit 1
}

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $scriptDir

$buildDir = "build-android-$abi"
if (-not (Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}
Set-Location $buildDir

Write-Host "[INFO] Configuring for Android..." -ForegroundColor Cyan
cmake -G Ninja `
    -DCMAKE_TOOLCHAIN_FILE="$toolchainFile" `
    -DANDROID_ABI="$abi" `
    -DANDROID_PLATFORM="android-$api" `
    -DANDROID_STL=c++_shared `
    ..

if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERROR] Configuration failed." -ForegroundColor Red
    Set-Location $scriptDir
    exit 1
}

Write-Host ""
Write-Host "[INFO] Building..." -ForegroundColor Cyan
cmake --build .
if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERROR] Build failed." -ForegroundColor Red
    Set-Location $scriptDir
    exit 1
}

Set-Location $scriptDir

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  Android build complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "  Output: $buildDir/"
Write-Host ""
