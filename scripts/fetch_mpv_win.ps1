param(
    [Parameter(Mandatory=$true)][string]$Url,
    [string]$OutDir = "mpv"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if (-not (Test-Path $OutDir)) { New-Item -ItemType Directory -Force -Path $OutDir | Out-Null }

$tmp = New-Item -ItemType Directory -Force -Path (Join-Path $env:TEMP ("mpv_pkg_" + [guid]::NewGuid()))
$zipPath = Join-Path $tmp "mpv.7z"

Write-Host "Downloading mpv package from $Url ..."
Invoke-WebRequest -Uri $Url -OutFile $zipPath

# Require 7z
$sevenZip = Get-Command 7z -ErrorAction SilentlyContinue
if (-not $sevenZip) { throw "7z not found in PATH. Please install 7-Zip and ensure '7z' is available." }

& 7z x $zipPath -o"$tmp\extracted" -y | Out-Null

# Try locate DLL and import lib
$dll = Get-ChildItem -Path "$tmp\extracted" -Recurse -Filter libmpv-2.dll | Select-Object -First 1
if (-not $dll) { $dll = Get-ChildItem -Path "$tmp\extracted" -Recurse -Filter libmpv.dll | Select-Object -First 1 }

$implib = Get-ChildItem -Path "$tmp\extracted" -Recurse -Filter libmpv.dll.a | Select-Object -First 1

if (-not $dll -or -not $implib) { throw "Could not find libmpv DLL or import library in the downloaded package." }

Copy-Item $dll.FullName -Destination (Join-Path $OutDir "libmpv-2.dll") -Force
Copy-Item $implib.FullName -Destination (Join-Path $OutDir "libmpv.dll.a") -Force

Write-Host "mpv runtime copied to $OutDir"

Remove-Item -Recurse -Force $tmp

