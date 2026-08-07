# Build host + launcher and assemble a D7-safe Windows player zip.
# Run from repo root:  powershell -File scripts/package_release.ps1
param(
    [string]$Version = $env:GEN3RECOMP_VERSION,
    [switch]$SkipBuild
)

$ErrorActionPreference = "Stop"
$root = Resolve-Path (Join-Path $PSScriptRoot "..")
Set-Location $root

if (-not $Version -or $Version.Trim() -eq "") {
    try {
        $Version = (git describe --tags --always 2>$null).Trim()
    } catch {
        $Version = "0.1.0"
    }
    if (-not $Version) { $Version = "0.1.0" }
}

$platform = "windows-x64"
$stage = Join-Path $root "dist/release/gen3recomp-$Version-$platform"
if (Test-Path $stage) { Remove-Item -Recurse -Force $stage }
New-Item -ItemType Directory -Force -Path (Join-Path $stage "bin") | Out-Null
New-Item -ItemType Directory -Force -Path (Join-Path $stage "scripts") | Out-Null
New-Item -ItemType Directory -Force -Path (Join-Path $stage "docs") | Out-Null
New-Item -ItemType Directory -Force -Path (Join-Path $stage "roms") | Out-Null

if (-not $SkipBuild) {
    Write-Host "==> CMake host"
    cmake -S . -B build
    # VS generator: one --target per invocation (multi-target misses _gbarecomp/*.vcxproj).
    cmake --build build --config Release --target gen3recomp
    cmake --build build --config Release --target gba_recompile

    Write-Host "==> Tauri launcher"
    Push-Location launcher
    try {
        if (Test-Path package-lock.json) { npm ci } else { npm install }
        npm run tauri build
    } finally {
        Pop-Location
    }
}

function Find-FirstFile([string[]]$candidates) {
    foreach ($c in $candidates) {
        if (Test-Path $c) { return (Resolve-Path $c).Path }
    }
    return $null
}

$hostBin = Find-FirstFile @(
    $env:GEN3RECOMP_HOST_BIN,
    "build/Release/gen3recomp.exe",
    "build/gen3recomp.exe",
    "build/Debug/gen3recomp.exe"
)
$launcherBin = Find-FirstFile @(
    $env:GEN3RECOMP_LAUNCHER_BIN,
    "launcher/src-tauri/target/release/gen3recomp-launcher.exe",
    "launcher/src-tauri/target/release/bundle/msi/*.exe"
)
$recompileBin = Find-FirstFile @(
    "build/Release/gba_recompile.exe",
    "build/_gbarecomp/Release/gba_recompile.exe",
    "build/gba_recompile.exe"
)

if (-not $hostBin) { throw "gen3recomp.exe not found; build first or set GEN3RECOMP_HOST_BIN" }
if (-not $launcherBin) { throw "gen3recomp-launcher.exe not found after tauri build" }

Copy-Item $hostBin (Join-Path $stage "bin/gen3recomp.exe")
Copy-Item $launcherBin (Join-Path $stage "bin/gen3recomp-launcher.exe")
if ($recompileBin) {
    Copy-Item $recompileBin (Join-Path $stage "bin/gba_recompile.exe")
}

foreach ($script in @("build_cart_artifact.sh", "verify_release_layout.sh", "run_launcher.sh")) {
    $src = Join-Path $root "scripts/$script"
    if (Test-Path $src) {
        Copy-Item $src (Join-Path $stage "scripts/$script")
    }
}

@"
Place catalogued USA Ruby, Sapphire, or Emerald .gba dumps in this folder.
Place gba_bios.bin in the package root (next to gen3recomp-player.bat).

This project does not distribute Nintendo ROMs, BIOS images, or cover art.
"@ | Set-Content -Encoding utf8 (Join-Path $stage "roms/README.txt")

@"
@echo off
setlocal
set HERE=%~dp0
set PATH=%HERE%bin;%PATH%
if not defined GEN3RECOMP_HOST set GEN3RECOMP_HOST=%HERE%bin\gen3recomp.exe
if not defined GEN3RECOMP_LAUNCHER set GEN3RECOMP_LAUNCHER=%HERE%bin\gen3recomp-launcher.exe
cd /d "%HERE%"
"%GEN3RECOMP_LAUNCHER%" %*
"@ | Set-Content -Encoding ascii (Join-Path $stage "gen3recomp-player.bat")

if (Test-Path "LICENSE") { Copy-Item "LICENSE" $stage }
if (Test-Path "README.md") { Copy-Item "README.md" (Join-Path $stage "docs/") }
if (Test-Path "docs/manual-boot.md") { Copy-Item "docs/manual-boot.md" (Join-Path $stage "docs/") }
if (Test-Path "docs/player-guide.md") { Copy-Item "docs/player-guide.md" (Join-Path $stage "docs/") }

$outDir = Join-Path $root "dist/release"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$zipPath = Join-Path $outDir "gen3recomp-$Version-$platform.zip"
if (Test-Path $zipPath) { Remove-Item -Force $zipPath }
Compress-Archive -Path $stage -DestinationPath $zipPath

# D7 layout check via bash when available (Git Bash / WSL)
$verify = Join-Path $root "scripts/verify_release_layout.sh"
if (Get-Command bash -ErrorAction SilentlyContinue) {
    & bash $verify $stage
    if ($LASTEXITCODE -ne 0) { throw "verify_release_layout failed" }
} else {
    Write-Host "warning: bash not found; skipping verify_release_layout.sh"
    if (-not (Test-Path (Join-Path $stage "bin/gen3recomp.exe"))) { throw "missing host" }
    if (-not (Test-Path (Join-Path $stage "bin/gen3recomp-launcher.exe"))) { throw "missing launcher" }
    if (-not (Test-Path (Join-Path $stage "roms"))) { throw "missing roms/" }
}

Write-Host "==> Release folder: $stage"
Write-Host "==> Zip: $zipPath"
