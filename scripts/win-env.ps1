<#
  Enter the MSVC x64 build environment (vcvars64) in the CURRENT PowerShell
  session. Visual Studio is located via vswhere, so this works for any edition
  or version without a hardcoded path.

  Powershell will almost certainly block the script from executing.
  Bypass with: Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
#>

if ($env:VCINSTALLDIR) {
  Write-Host "[win-env] MSVC environment already active - skipping."
  return
}

$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
if (-not (Test-Path $vswhere)) {
  Write-Error "[win-env] vswhere not found. Install Visual Studio 2017+ with the C++ workload."
  return
}

$vsPath = & $vswhere -latest -products * `
  -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
  -property installationPath
if (-not $vsPath) {
  Write-Error "[win-env] No Visual Studio install with the C++ x64 toolset was found."
  return
}

$vcvars = Join-Path $vsPath 'VC\Auxiliary\Build\vcvars64.bat'
if (-not (Test-Path $vcvars)) {
  Write-Error "[win-env] vcvars64.bat not found at $vcvars"
  return
}

# A .bat can't mutate its PowerShell parent, so run vcvars in a child cmd, dump
# the resulting environment with `set`, and import each variable back here.
cmd /c "`"$vcvars`" >nul 2>&1 && set" | ForEach-Object {
  if ($_ -match '^([^=]+)=(.*)$') {
    Set-Item -Path "Env:$($matches[1])" -Value $matches[2]
  }
}

Write-Host "[win-env] MSVC x64 environment ready ($env:VSCMD_ARG_TGT_ARCH)."
