# Rhodium Blockchain Installation Script
param(
    [string]$InstallDir = "$env:ProgramFiles\RhodiumBlockchain"
)

Write-Host "Starting Rhodium Blockchain installation..."

# 1. Create installation directory
if (-not (Test-Path $InstallDir)) {
    New-Item -ItemType Directory -Path $InstallDir | Out-Null
}

# 2. Install system dependencies
Write-Host "Installing system dependencies..."
$dependencies = @(
    "git",
    "cmake",
    "openssl",
    "nodejs",
    "python",
    "visualstudio2022buildtools",
    "visualstudio2022-workload-vctools"
)

foreach ($dep in $dependencies) {
    choco install $dep -y --no-progress
}

# 3. Clone repository
Write-Host "Downloading Rhodium Blockchain..."
git clone https://github.com/rhodium/blockchain.git "$InstallDir"

# 4. Build backend
Write-Host "Building backend components..."
cd "$InstallDir\backend"
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release

# 5. Install frontend dependencies
Write-Host "Installing frontend dependencies..."
cd "$InstallDir\frontend"
npm install

# 6. Create shortcuts
Write-Host "Creating shortcuts..."
$shell = New-Object -ComObject WScript.Shell
$desktopShortcut = $shell.CreateShortcut("$env:USERPROFILE\Desktop\Rhodium Blockchain.lnk")
$desktopShortcut.TargetPath = "$InstallDir\backend\build\Release\rhodium_gui.exe"
$desktopShortcut.WorkingDirectory = "$InstallDir"
$desktopShortcut.Save()

# 7. Configure firewall
Write-Host "Configuring firewall..."
New-NetFirewallRule -DisplayName "Rhodium Blockchain" -Direction Inbound -LocalPort 8080,3000 -Protocol TCP -Action Allow

Write-Host "Installation completed successfully!"
Write-Host "Shortcut created on your desktop."
Write-Host "You can now run Rhodium Blockchain from the Start Menu or desktop shortcut."