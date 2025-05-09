# Post-installation configuration script
param(
    [string]$DataDir = "$env:ProgramData\Rhodium"
)

# Create data directory
if (-not (Test-Path $DataDir)) {
    New-Item -ItemType Directory -Path $DataDir | Out-Null
}

# Initialize blockchain
& "$env:ProgramFiles\RhodiumBlockchain\backend\build\Release\rhodium_node.exe" --init --datadir "$DataDir"

# Create service
New-Service -Name "RhodiumNode" `
            -BinaryPathName "`"$env:ProgramFiles\RhodiumBlockchain\backend\build\Release\rhodium_node.exe`" --datadir `"$DataDir`"" `
            -DisplayName "Rhodium Blockchain Node" `
            -StartupType Automatic `
            -Description "Rhodium Blockchain Node Service"

# Start service
Start-Service -Name "RhodiumNode"

Write-Host "Configuration completed. Rhodium Blockchain service is now running."