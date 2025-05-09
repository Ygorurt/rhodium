# Rhodium Blockchain Installation Guide

## System Requirements
- Windows 10/11 64-bit
- 8GB RAM (16GB recommended for mining)
- 50GB free disk space
- Internet connection for initial setup

## Installation Steps

1. **Download the installer**:
   ```powershell
   Invoke-WebRequest -Uri "https://rhodium.org/download/install.ps1" -OutFile "install.ps1"
   ```

2. **Run the installer** (as Administrator):
   ```powershell
   Set-ExecutionPolicy Bypass -Scope Process -Force
   .\install.ps1
   ```

3. **Wait for installation to complete** (may take 10-30 minutes)

4. **Run the application**:
   - Desktop shortcut: `Rhodium Blockchain`
   - Or from command line: `rhodium_gui.exe`

## First-Time Setup
After installation, run the configuration wizard:
```powershell
.\configure.ps1
```

## Troubleshooting
If you encounter issues:
1. Check logs in `%ProgramData%\Rhodium\logs`
2. Verify service is running:
   ```powershell
   Get-Service -Name "RhodiumNode"
   ```
3. Restart the service:
   ```powershell
   Restart-Service -Name "RhodiumNode"
   ```