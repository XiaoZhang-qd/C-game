# ========================================
# CGame Racer Build Script (PowerShell)
# Works on Windows, Linux, and macOS
# ========================================

param(
    [string]$Arch = "",
    [switch]$Clean = $false,
    [switch]$CleanAll = $false,
    [switch]$Help = $false
)

$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

# ========================================
# Help
# ========================================
if ($Help) {
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host "  CGame Racer Build Script" -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Usage:" -ForegroundColor White
    Write-Host "  ./build.ps1 [-Arch ARCH]              Build (default)" -ForegroundColor Gray
    Write-Host "  ./build.ps1 -Clean [-Arch ARCH]       Clean specific arch build" -ForegroundColor Gray
    Write-Host "  ./build.ps1 -Clean -CleanAll          Clean ALL build directories" -ForegroundColor Gray
    Write-Host "  ./build.ps1 -Help                     Show this help" -ForegroundColor Gray
    Write-Host ""
    Write-Host "Available architectures:" -ForegroundColor White
    Write-Host "  x86      - 32-bit Intel/AMD" -ForegroundColor Gray
    Write-Host "  x64      - 64-bit Intel/AMD" -ForegroundColor Gray
    Write-Host "  arm32    - 32-bit ARM" -ForegroundColor Gray
    Write-Host "  arm64    - 64-bit ARM" -ForegroundColor Gray
    Write-Host "  riscv64  - 64-bit RISC-V" -ForegroundColor Gray
    Write-Host "  mips64   - 64-bit MIPS" -ForegroundColor Gray
    Write-Host ""
    Write-Host "Examples:" -ForegroundColor White
    Write-Host "  ./build.ps1 -Arch x64          Build x64" -ForegroundColor Gray
    Write-Host "  ./build.ps1 -Clean -Arch x86   Clean x86 build" -ForegroundColor Gray
    Write-Host "  ./build.ps1 -Clean -CleanAll   Clean ALL builds" -ForegroundColor Gray
    exit 0
}

# ========================================
# Clean action
# ========================================
if ($Clean -or $CleanAll) {
    Set-Location $scriptDir

    $allBuildDirs = @(
        "build",
        "build-win32",
        "build-win-arm64",
        "build-linux-x86",
        "build-linux-arm32",
        "build-linux-arm64",
        "build-linux-riscv64",
        "build-linux-riscv32",
        "build-linux-ppc64",
        "build-linux-ppc64le",
        "build-linux-ppc32",
        "build-linux-s390x",
        "build-linux-mips64",
        "build-linux-mips64le",
        "build-linux-mips32",
        "build-linux-mips32le",
        "build-linux-sparc64",
        "build-drm",
        "build-chromeos",
        "build-chromeos-x64",
        "build-macos-x64",
        "build-macos-arm64",
        "build-macos-universal",
        "build-freebsd",
        "build-openbsd",
        "build-netbsd",
        "build-dragonfly",
        "build-android-arm64",
        "build-android-armeabi-v7a",
        "build-android-x86",
        "build-android-x86_64",
        "build-ios",
        "build-ios-theos",
        "build-ios-sim",
        "build-ios-sim-theos",
        "build-web",
        "build-harmonyos-arm64",
        "build-mingw-x64",
        "build-mingw-x86"
    )

    $archToDir = @{
        "x64"     = "build"
        "x86"     = "build-win32"
        "arm64"   = "build-win-arm64"
        "arm32"   = "build-linux-arm32"
        "riscv64" = "build-linux-riscv64"
        "mips64"  = "build-linux-mips64"
    }

    function Remove-BuildDir($dir) {
        $fullPath = Join-Path $scriptDir $dir
        if (Test-Path $fullPath) {
            Write-Host "[CLEAN] Removing $dir..." -ForegroundColor Yellow
            Remove-Item -Recurse -Force $fullPath
            Write-Host "[CLEAN] Done: $dir" -ForegroundColor Green
        } else {
            Write-Host "[CLEAN] Skipping (not found): $dir" -ForegroundColor DarkGray
        }
    }

    if ($CleanAll) {
        Write-Host "========================================" -ForegroundColor Cyan
        Write-Host "  Cleaning ALL build artifacts" -ForegroundColor Cyan
        Write-Host "========================================" -ForegroundColor Cyan
        Write-Host ""
        foreach ($dir in $allBuildDirs) {
            Remove-BuildDir $dir
        }
        Write-Host ""
        Write-Host "[CLEAN] All build artifacts cleaned." -ForegroundColor Green
        exit 0
    }

    # Clean specific arch
    $cleanArch = if ($Arch) { $Arch } else { "x64" }

    if (-not $archToDir.ContainsKey($cleanArch)) {
        Write-Host "[ERROR] Unknown architecture: $cleanArch" -ForegroundColor Red
        Write-Host "        Valid: $($archToDir.Keys -join ', ')" -ForegroundColor Red
        exit 1
    }

    $dir = $archToDir[$cleanArch]
    Write-Host "[CLEAN] Cleaning $cleanArch -> $dir" -ForegroundColor Cyan
    Remove-BuildDir $dir
    exit 0
}

# ========================================
# Build action (default)
# ========================================

$validArchs = @("x86", "x64", "arm32", "arm64", "riscv64", "mips64")

if (-not $Arch) {
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host "  CGame Racer Build Script" -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "[INFO] No architecture specified. Building native architecture." -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Usage: ./build.ps1 [-Arch ARCH] [Options]" -ForegroundColor White
    Write-Host ""
    Write-Host "Options:" -ForegroundColor White
    Write-Host "  -Arch ARCH         Build for specific architecture" -ForegroundColor Gray
    Write-Host "  -Clean             Clean build artifacts" -ForegroundColor Gray
    Write-Host "  -Clean -CleanAll   Clean ALL build artifacts" -ForegroundColor Gray
    Write-Host "  -Help              Show this help" -ForegroundColor Gray
    Write-Host ""
    Write-Host "Available architectures:" -ForegroundColor White
    Write-Host "  x86      - 32-bit Intel/AMD" -ForegroundColor Gray
    Write-Host "  x64      - 64-bit Intel/AMD (default)" -ForegroundColor Gray
    Write-Host "  arm32    - 32-bit ARM" -ForegroundColor Gray
    Write-Host "  arm64    - 64-bit ARM" -ForegroundColor Gray
    Write-Host "  riscv64  - 64-bit RISC-V" -ForegroundColor Gray
    Write-Host "  mips64   - 64-bit MIPS" -ForegroundColor Gray
    Write-Host ""
    Write-Host "Examples:" -ForegroundColor White
    Write-Host "  ./build.ps1 -Arch x64          Build x64" -ForegroundColor Gray
    Write-Host "  ./build.ps1 -Clean -Arch x86   Clean x86 build" -ForegroundColor Gray
    Write-Host "  ./build.ps1 -Clean -CleanAll   Clean ALL builds" -ForegroundColor Gray
    Write-Host ""
    $Arch = "x64"
}

if ($validArchs -notcontains $Arch) {
    Write-Host "[ERROR] Invalid architecture: $Arch" -ForegroundColor Red
    Write-Host "        Valid options: $($validArchs -join ', ')" -ForegroundColor Red
    exit 1
}

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  CGame Racer Build Script" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "[INFO] Target architecture: $Arch" -ForegroundColor Green
Write-Host ""

# ========================================
# Check dependencies
# ========================================

$cmake = Get-Command cmake -ErrorAction SilentlyContinue
if (-not $cmake) {
    Write-Host "[ERROR] cmake not found." -ForegroundColor Red
    Write-Host "        Install from: https://cmake.org/download/"
    Write-Host "        Or via package manager:"
    Write-Host "          Windows:  winget install CMake.CMake"
    Write-Host "          Ubuntu:   sudo apt install cmake"
    Write-Host "          Fedora:   sudo dnf install cmake"
    Write-Host "          Arch:     sudo pacman -S cmake"
    Write-Host "          macOS:    brew install cmake"
    exit 1
}
Write-Host "[INFO] cmake found: $($cmake.Source)" -ForegroundColor Green

# Determine generator
$generator = $null
$buildCmd = $null

$ninja = Get-Command ninja -ErrorAction SilentlyContinue
if ($ninja) {
    $generator = "Ninja"
    $buildCmd = "cmake --build ."
} else {
    $make = Get-Command make -ErrorAction SilentlyContinue
    if ($make) {
        $generator = "Unix Makefiles"
        $cores = [Environment]::ProcessorCount
        $buildCmd = "make -j$cores"
    }
}

if (-not $generator) {
    $mingwMake = Get-Command mingw32-make -ErrorAction SilentlyContinue
    if ($mingwMake) {
        $generator = "MinGW Makefiles"
        $buildCmd = "cmake --build ."
    }
}

if (-not $generator) {
    Write-Host "[WARN] No ninja/make found. Trying default CMake generator..." -ForegroundColor Yellow
} else {
    Write-Host "[INFO] Using generator: $generator" -ForegroundColor Green
}

# Check for C compiler
$cc = $null
$cl = Get-Command cl -ErrorAction SilentlyContinue
if ($cl) { $cc = "MSVC (cl)" }
if (-not $cc) {
    $gcc = Get-Command gcc -ErrorAction SilentlyContinue
    if ($gcc) { $cc = "GCC" }
}
if (-not $cc) {
    $clang = Get-Command clang -ErrorAction SilentlyContinue
    if ($clang) { $cc = "Clang" }
}
if (-not $cc) {
    $ccCmd = Get-Command cc -ErrorAction SilentlyContinue
    if ($ccCmd) { $cc = "cc" }
}

if (-not $cc) {
    Write-Host "[ERROR] No C compiler found." -ForegroundColor Red
    Write-Host "        Install MSVC (Visual Studio), MinGW-w64, GCC, or Clang."
    Write-Host "          Windows:  winget install MinGW-w64"
    Write-Host "          Ubuntu:   sudo apt install gcc build-essential"
    Write-Host "          Fedora:   sudo dnf install gcc"
    Write-Host "          Arch:     sudo pacman -S gcc"
    Write-Host "          macOS:    xcode-select --install"
    exit 1
}
Write-Host "[INFO] C compiler: $cc" -ForegroundColor Green

# ========================================
# Select toolchain file based on architecture
# ========================================

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$toolchainFile = $null

$isWindows = $false
$isLinux = $false
$isMacOS = $false
if ($env:OS -eq "Windows_NT") {
    $isWindows = $true
} elseif ($IsLinux) {
    $isLinux = $true
} elseif ($IsMacOS) {
    $isMacOS = $true
} else {
    # Fallback for PowerShell 5 on Windows
    if ($env:OS -eq "Windows_NT") { $isWindows = $true }
    elseif ([Environment]::OSVersion.Platform -eq "Unix") {
        # Check if Linux or macOS
        try {
            $uname = & uname -s 2>$null
            if ($uname -eq "Darwin") { $isMacOS = $true }
            else { $isLinux = $true }
        } catch {
            $isLinux = $true
        }
    }
}

if ($isWindows) {
    switch ($Arch) {
        "x86"    { $toolchainFile = "cmake/toolchains/windows-x86.cmake" }
        "x64"    { $toolchainFile = "cmake/toolchains/windows-x64.cmake" }
        "arm32"  { $toolchainFile = "cmake/toolchains/windows-arm32.cmake"; if (-not (Test-Path $toolchainFile)) { $toolchainFile = $null } }
        "arm64"  { $toolchainFile = "cmake/toolchains/windows-arm64.cmake" }
    }
} elseif ($isLinux) {
    switch ($Arch) {
        "x86"    { $toolchainFile = "cmake/toolchains/linux-x86.cmake" }
        "x64"    { $toolchainFile = "cmake/toolchains/linux-x64.cmake" }
        "arm32"  { $toolchainFile = "cmake/toolchains/linux-arm32.cmake" }
        "arm64"  { $toolchainFile = "cmake/toolchains/linux-arm64.cmake" }
        "riscv64" { $toolchainFile = "cmake/toolchains/linux-riscv64.cmake" }
        "mips64"  { $toolchainFile = "cmake/toolchains/linux-mips64.cmake" }
    }
} elseif ($isMacOS) {
    switch ($Arch) {
        "x64"    { $toolchainFile = "cmake/toolchains/macos-x64.cmake" }
        "arm64"  { $toolchainFile = "cmake/toolchains/macos-arm64.cmake" }
        default  { $toolchainFile = $null }
    }
}

if ($toolchainFile) {
    $fullToolchainPath = Join-Path $scriptDir $toolchainFile
    if (Test-Path $fullToolchainPath) {
        Write-Host "[INFO] Using toolchain: $toolchainFile" -ForegroundColor Green
    } else {
        Write-Host "[INFO] Toolchain file not found, using native compiler" -ForegroundColor Yellow
        $toolchainFile = $null
    }
} else {
    Write-Host "[INFO] Using native compiler (no cross-compilation)" -ForegroundColor Yellow
}

# ========================================
# Build
# ========================================

Set-Location $scriptDir

if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}
Set-Location "build"

$cmakeArgs = @()
if ($toolchainFile) {
    $cmakeArgs += "-DCMAKE_TOOLCHAIN_FILE=$scriptDir/$toolchainFile"
}

if ($generator) {
    Write-Host "[INFO] Configuring with $generator..." -ForegroundColor Cyan
    $cmakeArgs = @("-G", $generator) + $cmakeArgs
} else {
    Write-Host "[INFO] Configuring with default generator..." -ForegroundColor Cyan
}

& cmake @cmakeArgs ..
if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERROR] Configuration failed." -ForegroundColor Red
    Set-Location $scriptDir
    exit 1
}

Write-Host ""
Write-Host "[INFO] Building..." -ForegroundColor Cyan
if ($generator -eq "Ninja") {
    cmake --build .
} elseif ($generator -eq "Unix Makefiles") {
    $cores = [Environment]::ProcessorCount
    make -j$cores
} else {
    cmake --build .
}
if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERROR] Build failed." -ForegroundColor Red
    Set-Location $scriptDir
    exit 1
}

Set-Location $scriptDir

$exeExt = ""
if ($isWindows) {
    $exeExt = ".exe"
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  Build successful!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "  Architecture: $Arch"
Write-Host "  Server: build/c-game-server$exeExt"
Write-Host "  Client: build/c-game-client$exeExt"
Write-Host ""
Write-Host "  To run:"
Write-Host "    Start server: ./build/c-game-server$exeExt"
Write-Host "    Start client: ./build/c-game-client$exeExt"
Write-Host ""
