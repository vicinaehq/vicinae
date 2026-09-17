# Usage: scripts/mkinstaller.ps1 [-BuildDir build-release] [-OutDir <BuildDir>] [-Version x.y.z]
# Packages an existing build: build the tree first, the script never compiles
# (a rebuild here would replace binaries that were signed after the build).
param(
    [string]$BuildDir = "build-release",
    [string]$OutDir = "",
    [string]$Arch = "x64",
    [string]$Version = ""
)
$ErrorActionPreference = "Stop"
$root = Split-Path $PSScriptRoot

$BuildDir = Join-Path $root $BuildDir
if (-not (Test-Path (Join-Path $BuildDir "CMakeCache.txt"))) {
    throw "no CMake build at $BuildDir (configure and build first)"
}

# prefer the real compiler over whatever shim is on PATH
$iscc = $null
foreach ($p in "$env:LOCALAPPDATA\Programs\Inno Setup 6\ISCC.exe",
               "${env:ProgramFiles(x86)}\Inno Setup 6\ISCC.exe") {
    if (Test-Path $p) { $iscc = $p; break }
}
if (-not $iscc) { $iscc = (Get-Command iscc -ErrorAction SilentlyContinue).Source }
if (-not $iscc) { throw "ISCC.exe not found (winget install JRSoftware.InnoSetup)" }

if (-not $Version) {
    try { $Version = git -C $root describe --tags --abbrev=0 2>$null } catch { $Version = $null }
}
if ($Version) { $Version = $Version -replace '^v', '' } else { $Version = "0.0.0" }
if ($Version -notmatch '^\d+(\.\d+){0,3}$') { throw "version '$Version' is not numeric x.y.z" }

if (-not (Test-Path (Join-Path $BuildDir "bin\vicinae-server.exe"))) {
    throw "no vicinae-server.exe in $BuildDir\bin (build first)"
}

$stage = Join-Path $BuildDir "stage"
if (Test-Path $stage) { Remove-Item -Recurse -Force $stage }
cmake --install $BuildDir --prefix $stage
if ($LASTEXITCODE -ne 0) { throw "cmake --install failed" }
foreach ($f in "bin\vicinae-server.exe", "bin\qt.conf", "plugins\platforms\qwindows.dll") {
    if (-not (Test-Path (Join-Path $stage $f))) {
        throw "staged tree is missing $f - $BuildDir has stale install rules, reconfigure it"
    }
}

$isccArgs = @("/DStageDir=$stage", "/DAppVersion=$Version", "/DArch=$Arch")

if ($OutDir) { $OutDir = Join-Path $root $OutDir } else { $OutDir = $BuildDir }
Write-Host "iscc: $iscc $isccArgs /O$OutDir"
& $iscc @isccArgs "/O$OutDir" (Join-Path $root "extra\windows\vicinae.iss")
if ($LASTEXITCODE -ne 0) { throw "iscc failed" }

Get-ChildItem $OutDir -Filter "vicinae-$Arch-setup.exe" | ForEach-Object {
    Write-Host ("installer: {0} ({1:N1} MB)" -f $_.FullName, ($_.Length / 1MB))
}
