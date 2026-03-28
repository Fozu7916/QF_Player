param(
    [string]$Url = "",
    [ValidateSet("x86_64", "x86_64-v3", "i686", "aarch64")]
    [string]$Arch = "x86_64",
    [string]$OutDir = "mpv"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-MpvDevAssetUrl {
    param([string]$Arch)
    $api = "https://api.github.com/repos/shinchiro/mpv-winbuild-cmake/releases/latest"
    $headers = @{
        "Accept"               = "application/vnd.github+json"
        "X-GitHub-Api-Version" = "2022-11-28"
        "User-Agent"           = "QF_Player-fetch-mpv"
    }
    $release = Invoke-RestMethod -Uri $api -Headers $headers
    $pattern = switch ($Arch) {
        "x86_64" { '^mpv-dev-x86_64-\d.+\.7z$' }
        "x86_64-v3" { '^mpv-dev-x86_64-v3-.+\.7z$' }
        "i686" { '^mpv-dev-i686-.+\.7z$' }
        "aarch64" { '^mpv-dev-aarch64-.+\.7z$' }
        default { throw "Unknown architecture: $Arch" }
    }
    foreach ($asset in $release.assets) {
        if ($asset.name -match $pattern) {
            return [string]$asset.browser_download_url
        }
    }
    throw "No mpv-dev .7z asset for architecture '$Arch' in latest mpv-winbuild-cmake release."
}

function Get-SevenZipExe {
    $cmd = Get-Command 7z -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }
    foreach ($candidate in @(
        "${env:ProgramFiles}\7-Zip\7z.exe"
        "${env:ProgramFiles(x86)}\7-Zip\7z.exe"
    )) {
        if (Test-Path -LiteralPath $candidate) { return $candidate }
    }
    throw "7z not found. Install 7-Zip (https://www.7-zip.org/) or add it to PATH."
}

if (-not $Url) {
    Write-Host "Resolving latest mpv-dev ($Arch) from GitHub ..."
    $Url = Get-MpvDevAssetUrl -Arch $Arch
}

if (-not (Test-Path $OutDir)) { New-Item -ItemType Directory -Force -Path $OutDir | Out-Null }

$tmp = $null
try {
    $tmp = New-Item -ItemType Directory -Force -Path (Join-Path $env:TEMP ("mpv_pkg_" + [guid]::NewGuid()))
    $archivePath = Join-Path $tmp "mpv.7z"

    Write-Host "Downloading mpv package ..."
    Write-Host "  $Url"
    Invoke-WebRequest -Uri $Url -OutFile $archivePath -UseBasicParsing

    $sevenZip = Get-SevenZipExe
    $extractDir = Join-Path $tmp "extracted"
    New-Item -ItemType Directory -Force -Path $extractDir | Out-Null
    & $sevenZip x $archivePath "-o$extractDir" -y | Out-Null
    if ($LASTEXITCODE -ne 0) { throw "7z extraction failed (exit $LASTEXITCODE)." }

    $dll = Get-ChildItem -Path $extractDir -Recurse -Filter libmpv-2.dll | Select-Object -First 1
    if (-not $dll) { $dll = Get-ChildItem -Path $extractDir -Recurse -Filter libmpv.dll | Select-Object -First 1 }

    $implib = Get-ChildItem -Path $extractDir -Recurse -Filter libmpv.dll.a | Select-Object -First 1

    if (-not $dll -or -not $implib) {
        throw "Could not find libmpv DLL or import library (libmpv.dll.a) in the archive."
    }

    Copy-Item $dll.FullName -Destination (Join-Path $OutDir "libmpv-2.dll") -Force
    Copy-Item $implib.FullName -Destination (Join-Path $OutDir "libmpv.dll.a") -Force

    Write-Host "mpv runtime copied to $(Resolve-Path $OutDir)"
}
finally {
    if ($tmp -and (Test-Path -LiteralPath $tmp.FullName)) {
        Remove-Item -LiteralPath $tmp.FullName -Recurse -Force -ErrorAction SilentlyContinue
    }
}

