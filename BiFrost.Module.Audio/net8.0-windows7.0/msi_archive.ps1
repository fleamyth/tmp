param (
    [string]$BasePath,
    [string]$AssemblyName
)

$BasePath = $BasePath.TrimEnd('\').Trim('"')
$archiveFolder = Join-Path $BasePath 'msi_archive'
$archiveFile = Join-Path $archiveFolder 'archive.tar.gz'
$exeFile = Join-Path $BasePath "$AssemblyName.exe"
$msiArchiveScript = Join-Path $BasePath 'msi_archive.ps1'
$unzipArchiveScript = Join-Path $BasePath 'unzip_archive.ps1'
$parentDir = Split-Path $BasePath -Parent
$tempTarFile = Join-Path $parentDir 'temp_archive.tar.gz'

if (Test-Path $archiveFolder) {
    Remove-Item $archiveFolder -Recurse -Force
}

if (Test-Path $tempTarFile) {
    Remove-Item $tempTarFile -Force
}

& tar -czf $tempTarFile -C $BasePath .

$tempExtractDir = Join-Path $parentDir 'temp_extract'
New-Item -Path $tempExtractDir -ItemType Directory
& tar -xzf $tempTarFile -C $tempExtractDir

$entriesToRemoveNames = @(
    [System.IO.Path]::GetFileName($exeFile),
    [System.IO.Path]::GetFileName($msiArchiveScript),
    [System.IO.Path]::GetFileName($unzipArchiveScript)
)
foreach ($name in $entriesToRemoveNames) {
    $pathToRemove = Join-Path $tempExtractDir $name
    if (Test-Path $pathToRemove) {
        Write-Host "Deleting entry: $pathToRemove"
        Remove-Item $pathToRemove -Force
    }
}

& tar -czf $tempTarFile -C $tempExtractDir .

Remove-Item -Path $tempExtractDir -Recurse -Force

New-Item -Path $archiveFolder -ItemType Directory
Move-Item -Path $tempTarFile -Destination $archiveFile -Force
Copy-Item -Path $exeFile -Destination $archiveFolder
Copy-Item -Path $unzipArchiveScript -Destination $archiveFolder
