# Usage: scripts/mkinstaller.ps1 [-BuildDir build-release] [-OutDir <BuildDir>]
param(
    [string]$BuildDir = "build-release",
    [string]$OutDir = "",
    [string]$Arch = "x64"
)
$ErrorActionPreference = "Stop"
$root = Split-Path $PSScriptRoot

$BuildDir = Join-Path $root $BuildDir
if (-not (Test-Path (Join-Path $BuildDir "CMakeCache.txt"))) {
    throw "no CMake build at $BuildDir (configure and build first)"
}

$iscc = Get-Command iscc -ErrorAction SilentlyContinue
if ($iscc) { $iscc = $iscc.Source }
else {
    foreach ($p in "$env:LOCALAPPDATA\Programs\Inno Setup 6\ISCC.exe",
                   "${env:ProgramFiles(x86)}\Inno Setup 6\ISCC.exe") {
        if (Test-Path $p) { $iscc = $p; break }
    }
}
if (-not $iscc) { throw "ISCC.exe not found (winget install JRSoftware.InnoSetup)" }

try { $version = git -C $root describe --tags --abbrev=0 2>$null } catch { $version = $null }
if ($version) { $version = $version -replace '^v', '' } else { $version = "0.0.0" }

cmake --build $BuildDir --target vicinae-server vicinae
if ($LASTEXITCODE -ne 0) { throw "cmake --build failed" }

$stage = Join-Path $BuildDir "stage"
if (Test-Path $stage) { Remove-Item -Recurse -Force $stage }
cmake --install $BuildDir --prefix $stage
if ($LASTEXITCODE -ne 0) { throw "cmake --install failed" }
foreach ($f in "bin\vicinae-server.exe", "bin\qt.conf", "plugins\platforms\qwindows.dll") {
    if (-not (Test-Path (Join-Path $stage $f))) {
        throw "staged tree is missing $f - $BuildDir has stale install rules, reconfigure it"
    }
}

if ($OutDir) { $OutDir = Join-Path $root $OutDir } else { $OutDir = $BuildDir }
& $iscc "/DStageDir=$stage" "/DAppVersion=$version" "/DArch=$Arch" "/O$OutDir" `
    (Join-Path $root "extra\windows\vicinae.iss")
if ($LASTEXITCODE -ne 0) { throw "iscc failed" }

Get-ChildItem $OutDir -Filter "vicinae-$Arch-setup.exe" | ForEach-Object {
    Write-Host ("installer: {0} ({1:N1} MB)" -f $_.FullName, ($_.Length / 1MB))
}
