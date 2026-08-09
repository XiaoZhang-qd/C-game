# ========================================
# CGame Racer WebAssembly Build (PowerShell)
# Works on Windows, Linux, and macOS
# ========================================

$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  CGame Racer WebAssembly Build" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$emcc = Get-Command emcc -ErrorAction SilentlyContinue
if (-not $emcc) {
    Write-Host "[ERROR] Emscripten (emcc) not found." -ForegroundColor Red
    Write-Host "        Install from: https://emscripten.org/docs/getting_started/downloads.html"
    Write-Host ""
    Write-Host "        Quick setup:"
    Write-Host "          git clone https://github.com/emscripten-core/emsdk.git"
    Write-Host "          cd emsdk"
    Write-Host "          emsdk install latest"
    Write-Host "          emsdk activate latest"
    Write-Host "          # On Windows: emsdk_env.bat"
    Write-Host "          # On Linux/macOS: source ./emsdk_env.sh"
    exit 1
}

Write-Host "[INFO] emcc found." -ForegroundColor Green

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $scriptDir

# Build raylib for Web if needed
if (-not (Test-Path "raylib/src/libraylib.a")) {
    Write-Host "[INFO] Building raylib for WebAssembly..." -ForegroundColor Cyan
    Set-Location "raylib"
    make -j4 PLATFORM=PLATFORM_WEB
    Set-Location $scriptDir
}

if (-not (Test-Path "build-web")) {
    New-Item -ItemType Directory -Path "build-web" | Out-Null
}
Set-Location "build-web"

Write-Host "[INFO] Configuring for WebAssembly..." -ForegroundColor Cyan
emcmake cmake -G "Unix Makefiles" ..
if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERROR] Configuration failed." -ForegroundColor Red
    Set-Location $scriptDir
    exit 1
}

Write-Host ""
Write-Host "[INFO] Building..." -ForegroundColor Cyan
emmake make -j$([Environment]::ProcessorCount)
if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERROR] Build failed." -ForegroundColor Red
    Set-Location $scriptDir
    exit 1
}

Set-Location $scriptDir

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  WebAssembly build complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "  Output: build-web/c-game-client-web.html"
Write-Host ""
Write-Host "  To serve locally:"
Write-Host "    python3 -m http.server 8000 -d build-web"
Write-Host "  Then open: http://localhost:8000/c-game-client-web.html"
Write-Host ""
