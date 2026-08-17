param (
    [string]$tarFilePath,
    [string]$destinationPath
)

$tarFilePath = $tarFilePath.Trim('"')
$destinationPath = $destinationPath.Trim('"')

if (-not (Test-Path $tarFilePath)) {
    Write-Host "Error: The specified tar file does not exist."
    exit 1
}

if (-not (Test-Path $destinationPath)) {
    try {
        New-Item -Path $destinationPath -ItemType Directory -Force
    } catch {
        Write-Host "Error: Failed to create destination directory."
        exit 1
    }
}

try {
    Write-Host "Extracting tar file..."
    tar -xzf $tarFilePath -C $destinationPath
    Write-Host "Extraction complete."
} catch {
    Write-Host "Error: Failed to extract tar file."
    exit 1
}
