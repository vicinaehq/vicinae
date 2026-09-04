# Usage: scripts/mkinstaller.ps1 [-BuildDir build-release] [-OutDir <BuildDir>] [-Version x.y.z]
#        [-SignCommand 'signtool sign /fd SHA256 /tr <url> /td SHA256 ... $f']
# -SignCommand is run on every staged exe and, through ISCC, on the installer and uninstaller.
param(
    [string]$BuildDir = "build-release",
    [string]$OutDir = "",
    [string]$Arch = "x64",
    [string]$Version = "",
    [string]$SignCommand = ""
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

if (-not $Version) {
    try { $Version = git -C $root describe --tags --abbrev=0 2>$null } catch { $Version = $null }
}
if ($Version) { $Version = $Version -replace '^v', '' } else { $Version = "0.0.0" }
if ($Version -notmatch '^\d+(\.\d+){0,3}$') { throw "version '$Version' is not numeric x.y.z" }

cmake --build $BuildDir
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

$isccArgs = @("/DStageDir=$stage", "/DAppVersion=$Version", "/DArch=$Arch")
if ($SignCommand) {
    foreach ($exe in Get-ChildItem (Join-Path $stage "bin") -Filter "*.exe") {
        Invoke-Expression ($SignCommand -replace '\$f', "`"$($exe.FullName)`"")
        if ($LASTEXITCODE -ne 0) { throw "signing $($exe.Name) failed" }
    }
    $isccArgs += @("/DSign", "/Ssign=$SignCommand")
}

if ($OutDir) { $OutDir = Join-Path $root $OutDir } else { $OutDir = $BuildDir }
& $iscc @isccArgs "/O$OutDir" (Join-Path $root "extra\windows\vicinae.iss")
if ($LASTEXITCODE -ne 0) { throw "iscc failed" }

Get-ChildItem $OutDir -Filter "vicinae-$Arch-setup.exe" | ForEach-Object {
    Write-Host ("installer: {0} ({1:N1} MB)" -f $_.FullName, ($_.Length / 1MB))
}
