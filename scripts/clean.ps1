#Requires -Version 5.1
<#
.SYNOPSIS
    Removes build output and generated artifacts. PowerShell equivalent of
    `make clean`, covering both Windows preset build dirs (build, build-release).
#>
[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot

$targets = @(
    'build'
    'build-release'
    'src/typescript/api/node_modules'
    'src/typescript/api/dist'
    'src/typescript/api/src/proto'
    'src/typescript/extension-manager/dist'
    'src/typescript/extension-manager/node_modules'
    'src/typescript/extension-manager/src/proto'
    'scripts/.tmp'
)

foreach ($t in $targets) {
    $path = Join-Path $repoRoot $t
    if (Test-Path $path) {
        Write-Host "rm $t" -ForegroundColor Cyan
        # -Force clears read-only bits (Qt resource copies, downloaded node.exe).
        Remove-Item -LiteralPath $path -Recurse -Force
    }
}

# src/lib/*/build
Get-ChildItem (Join-Path $repoRoot 'src/lib') -Directory -ErrorAction SilentlyContinue |
    ForEach-Object { Join-Path $_.FullName 'build' } |
    Where-Object { Test-Path $_ } |
    ForEach-Object {
        Write-Host "rm src/lib/$(Split-Path (Split-Path $_ -Parent) -Leaf)/build" -ForegroundColor Cyan
        Remove-Item -LiteralPath $_ -Recurse -Force
    }

Write-Host 'Clean.' -ForegroundColor Green
