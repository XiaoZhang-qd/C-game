# ========================================
# PXPT Racer - Build All Platforms (PowerShell)
# Works on Windows, Linux, and macOS
# ========================================

$ErrorActionPreference = "Continue"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  PXPT Racer - Build All Platforms" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $scriptDir

$passed = 0
$failed = 0
$skipped = 0

function Build-Platform {
    param(
        [string]$Name,
        [string]$Script,
        [string]$RequiresCmd
    )

    Write-Host ""
    Write-Host "----------------------------------------" -ForegroundColor Gray
    Write-Host "  Building for $Name..." -ForegroundColor White
    Write-Host "----------------------------------------" -ForegroundColor Gray

    if ($RequiresCmd) {
        $cmd = Get-Command $RequiresCmd -ErrorAction SilentlyContinue
        if (-not $cmd) {
            Write-Host "[SKIP] $RequiresCmd not found, skipping $Name build." -ForegroundColor Yellow
            $script:skipped++
            return
        }
    }

    if (Test-Path $Script) {
        & $Script
        if ($LASTEXITCODE -eq 0) {
            Write-Host "[OK] $Name build succeeded." -ForegroundColor Green
            $script:passed++
        } else {
            Write-Host "[FAIL] $Name build failed." -ForegroundColor Red
            $script:failed++
        }
    } else {
        Write-Host "[SKIP] Build script $Script not found." -ForegroundColor Yellow
        $script:skipped++
    }
}

# Native desktop build
Build-Platform "Native Desktop" "./build.ps1" "cmake"

# WebAssembly
$emcc = Get-Command emcc -ErrorAction SilentlyContinue
if ($emcc) {
    Build-Platform "WebAssembly" "./build-wasm.ps1" "emcc"
} else {
    Write-Host "[SKIP] emcc not found, skipping WebAssembly build." -ForegroundColor Yellow
    $skipped++
}

# Android
if ($env:ANDROID_NDK_HOME -or (Test-Path "$env:LOCALAPPDATA\Android\Sdk\ndk")) {
    Build-Platform "Android (arm64-v8a)" "./build-android.ps1" "cmake"
} else {
    Write-Host "[SKIP] ANDROID_NDK_HOME not set, skipping Android build." -ForegroundColor Yellow
    $skipped++
}

# iOS
if ($IsMacOS -or $env:OS -eq "Darwin") {
    Build-Platform "iOS" "./build-ios.sh" "xcodebuild"
} else {
    Write-Host "[SKIP] iOS build requires macOS, skipping." -ForegroundColor Yellow
    $skipped++
}

# HarmonyOS
if ($env:HOS_SDK_HOME -or (Test-Path "$HOME/HarmonyOS-NEXT-DB1")) {
    Build-Platform "HarmonyOS" "./build-harmonyos.sh" "cmake"
} else {
    Write-Host "[SKIP] HOS_SDK_HOME not set, skipping HarmonyOS build." -ForegroundColor Yellow
    $skipped++
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Build Summary" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Passed:  $passed" -ForegroundColor Green
Write-Host "  Failed:  $failed" -ForegroundColor $(if ($failed -gt 0) { "Red" } else { "Gray" })
Write-Host "  Skipped: $skipped" -ForegroundColor Yellow
Write-Host ""

if ($failed -gt 0) {
    Write-Host "  Some builds failed. Check logs above." -ForegroundColor Red
    exit 1
}

Write-Host "  All selected platforms built successfully!" -ForegroundColor Green
