#Requires -Version 5.1
<#
.SYNOPSIS
    Formats the Vicinae source tree on Windows: clang-format (C++), qmlformat
    (QML) and biome (TypeScript). PowerShell equivalent of `make format` for
    contributors who do not have GNU make / a POSIX shell on PATH.

.PARAMETER Check
    Verify C++ formatting instead of rewriting files (clang-format --dry-run
    -Werror). Exits non-zero on drift. Mirrors `make check-format`.

.EXAMPLE
    pwsh scripts/format.ps1
    Format everything in place.

.EXAMPLE
    pwsh scripts/format.ps1 -Check
    Fail if any C++ file is not formatted (for local pre-push checks).
#>
[CmdletBinding()]
param(
    [switch]$Check
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$srcDir   = Join-Path $repoRoot 'src'
$tsDir    = Join-Path $srcDir 'typescript'

# Vendored deps and build output live under src/; never reformat those.
$excludePattern = '\\(node_modules|build|build-release|build-debug|\.git)\\'

function Resolve-Tool {
    param([string]$Name, [string[]]$Fallbacks = @())
    $cmd = Get-Command $Name -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }
    foreach ($f in $Fallbacks) { if ($f -and (Test-Path $f)) { return $f } }
    return $null
}

function Get-Sources {
    param([string[]]$Patterns)
    Get-ChildItem -Path $srcDir -Recurse -File -Include $Patterns |
        Where-Object { $_.FullName -notmatch $excludePattern }
}

# clang-format / qmlformat take many files per call; batch to stay well under
# the Windows command-line length limit while keeping invocations few.
function Invoke-Batched {
    param([string]$Tool, [string[]]$ToolArgs, [System.IO.FileInfo[]]$Files, [int]$BatchSize = 40)
    $exit = 0
    for ($i = 0; $i -lt $Files.Count; $i += $BatchSize) {
        $end   = [Math]::Min($i + $BatchSize - 1, $Files.Count - 1)
        $batch = $Files[$i..$end] | ForEach-Object { $_.FullName }
        & $Tool @ToolArgs @batch
        if ($LASTEXITCODE -ne 0) { $exit = $LASTEXITCODE }
    }
    return $exit
}

# The repo's .clang-format-ignore is only honored by clang-format >= 18, and
# older releases disagree on rules (the npm `clang-format` package ships v15,
# which reformats generated/vendored files that CI leaves alone). Prefer a
# modern one: $CLANG_FORMAT override, then the LLVM bundled with VS 2022.
function Get-ClangFormatVersion {
    param([string]$Path)
    $out = (& $Path --version 2>$null) -join "`n"
    if ($out -match 'version\s+(\d+)\.') { return [int]$Matches[1] }
    return 0
}

$clangFallbacks = @(
    $env:CLANG_FORMAT,
    'C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\Llvm\bin\clang-format.exe',
    'C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\Llvm\x64\bin\clang-format.exe',
    'C:\Program Files\LLVM\bin\clang-format.exe'
)
$clangFormat = Resolve-Tool 'clang-format' $clangFallbacks
if ($env:CLANG_FORMAT) { $clangFormat = $env:CLANG_FORMAT }
# If PATH resolved to a stale version but a modern one is installed, prefer it.
if ($clangFormat -and (Get-ClangFormatVersion $clangFormat) -lt 18) {
    $modern = $clangFallbacks | Where-Object { $_ -and (Test-Path $_) } |
        Where-Object { (Get-ClangFormatVersion $_) -ge 18 } | Select-Object -First 1
    if ($modern) { $clangFormat = $modern }
}
if ($clangFormat -and (Get-ClangFormatVersion $clangFormat) -lt 18) {
    Write-Warning "clang-format $((Get-ClangFormatVersion $clangFormat)) is < 18; it ignores .clang-format-ignore and may churn generated files. Set `$env:CLANG_FORMAT to a newer one."
}

$qmlFallbacks = @(Get-ChildItem 'C:\Qt\*\*\bin\qmlformat.exe' -ErrorAction SilentlyContinue |
    Select-Object -ExpandProperty FullName)

$qmlFormat = Resolve-Tool 'qmlformat' $qmlFallbacks
$biome     = Resolve-Tool 'biome'

$failed = $false

# --- C++ (clang-format) ---
$cppFiles = @(Get-Sources @('*.cpp', '*.hpp'))
if (-not $clangFormat) {
    Write-Warning 'clang-format not found on PATH; skipping C++'
    if ($Check) { $failed = $true }
} elseif ($Check) {
    Write-Host "check clang-format: $($cppFiles.Count) files" -ForegroundColor Cyan
    if ((Invoke-Batched $clangFormat @('--dry-run', '-Werror') $cppFiles) -ne 0) { $failed = $true }
} else {
    Write-Host "clang-format: $($cppFiles.Count) files" -ForegroundColor Cyan
    if ((Invoke-Batched $clangFormat @('-i') $cppFiles) -ne 0) { $failed = $true }
}

if ($Check) {
    if ($failed) { Write-Error 'Formatting check failed.'; exit 1 }
    Write-Host 'Formatting OK.' -ForegroundColor Green
    exit 0
}

# --- QML (qmlformat) ---
$qmlFiles = @(Get-Sources @('*.qml'))
if (-not $qmlFormat) {
    Write-Warning 'qmlformat not found (PATH or C:\Qt\*\*\bin); skipping QML'
} else {
    Write-Host "qmlformat: $($qmlFiles.Count) files" -ForegroundColor Cyan
    if ((Invoke-Batched $qmlFormat @('-i') $qmlFiles) -ne 0) { $failed = $true }
}

# --- TypeScript (biome) ---
if (-not $biome) {
    Write-Warning 'biome not found on PATH; skipping TypeScript'
} else {
    Write-Host 'biome format --write' -ForegroundColor Cyan
    Push-Location $tsDir
    try {
        & $biome format --write .
        if ($LASTEXITCODE -ne 0) { $failed = $true }
    } finally { Pop-Location }
}

if ($failed) { Write-Error 'One or more formatters reported errors.'; exit 1 }
Write-Host 'Done.' -ForegroundColor Green
