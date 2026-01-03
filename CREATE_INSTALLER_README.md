# Displexity Professional Installer Creation Guide

## 🎯 Overview

This guide will help you create a professional Windows installer for Displexity using NSIS (Nullsoft Scriptable Install System).

## 📋 Prerequisites

### Option 1: MSYS2 (Recommended)
```bash
# Install MSYS2 from https://www.msys2.org/
# Then in MSYS2 terminal:
pacman -S mingw-w64-x86_64-nsis
```

### Option 2: Direct Download
1. Download NSIS from https://nsis.sourceforge.io/
2. Install to default location (usually `C:\Program Files (x86)\NSIS`)
3. Add NSIS directory to your PATH environment variable

### Option 3: Chocolatey
```cmd
choco install nsis
```

## 🚀 Quick Start

### Step 1: Setup Environment
```cmd
# Run the setup script
setup_installer_environment.bat
```

### Step 2: Build Installer
```cmd
# Navigate to build directory
cd installer_build

# Build the installer
build.bat
```

### Step 3: Test Installer
```cmd
# Run the created installer
displexity-installer.exe
```

## 📁 Files Included

### Core Installer Files
- `displexity.nsi` - Main NSIS installer script
- `EnvVarUpdate.nsh` - PATH environment variable helper
- `LICENSE.txt` - Software license
- `build_installer.bat` - Build script
- `setup_installer_environment.bat` - Environment setup

### Application Files (Auto-detected)
- `bin/disp.exe` - Displexity compiler
- `release/lib/displexity/*.disll` - Binary libraries
- `release/include/displexity/*.dish` - Header files
- `examples/*.dis` - Example programs
- `docs/*.md` - Documentation

## 🎛️ Installer Features

### Professional Interface
- Welcome screen with branding
- License agreement page
- Component selection (Core, Libraries, Examples, Docs, Tools)
- Installation directory selection
- Progress display
- Completion screen with quick start options

### System Integration
- **PATH Configuration**: Automatically adds Displexity to system PATH
- **File Associations**: 
  - `.dis` files → Displexity compiler
  - `.dish` files → Header files
  - `.disll` files → Library files
  - `.disp` files → Package files (with run command)
- **Start Menu**: Creates program group with shortcuts
- **Desktop Shortcut**: Quick access to compiler
- **Registry Integration**: Proper Windows integration
- **Uninstaller**: Complete removal with registry cleanup

### Installation Types
1. **Full Installation** - Everything included
2. **Minimal Installation** - Core components only
3. **Developer Installation** - Full + development tools

## 🔧 Customization

### Modifying the Installer
Edit `displexity.nsi` to customize:
- Installation components
- File associations
- Registry entries
- Shortcuts and menu items
- Branding and appearance

### Adding Components
```nsis
Section "New Component" SEC_NEW
  SectionIn 1 3  ; Include in Full and Developer installations
  SetOutPath "$INSTDIR\newcomponent"
  File /r "source\newcomponent\*.*"
SectionEnd
```

### Custom Branding
- Replace `exe.ico` with your custom icon
- Add `installer_header.bmp` (150x57 pixels)
- Add `installer_welcome.bmp` (164x314 pixels)

## 🧪 Testing

### Test Installation
1. Run installer on clean system
2. Verify all components installed correctly
3. Test PATH configuration: `disp --help`
4. Test file associations: double-click `.dis` file
5. Test shortcuts and menu items

### Test Uninstallation
1. Run uninstaller from Control Panel or Start Menu
2. Verify all files and registry entries removed
3. Verify PATH cleaned up
4. Verify file associations removed

## 📦 Distribution

### Single File Distribution
The installer creates a single `displexity-installer.exe` file that contains:
- All application files
- All libraries and examples
- Complete installation logic
- Uninstaller
- System integration

### File Size Optimization
- NSIS uses LZMA compression by default
- Typical compression ratio: 60-80%
- Final installer size depends on included components

## 🔍 Troubleshooting

### Common Issues

**NSIS not found**
```
Solution: Install NSIS and add to PATH
Check: makensis /VERSION
```

**Build fails with file not found**
```
Solution: Ensure all referenced files exist
Check: File paths in displexity.nsi
```

**Installer fails to run**
```
Solution: Check Windows compatibility
Try: Run as administrator
```

**PATH not updated**
```
Solution: Restart command prompt/system
Check: echo %PATH%
```

### Debug Mode
Add to `displexity.nsi` for debugging:
```nsis
!define DEBUG
!ifdef DEBUG
  SetCompress off
  SetDatablockOptimize off
!endif
```

## 📋 Checklist

### Before Building
- [ ] NSIS installed and in PATH
- [ ] All application files present
- [ ] License file exists
- [ ] Icons and branding files ready
- [ ] Test on development machine

### Before Distribution
- [ ] Test installer on clean system
- [ ] Verify all components install correctly
- [ ] Test uninstaller
- [ ] Check file associations
- [ ] Verify PATH configuration
- [ ] Test shortcuts and menu items
- [ ] Scan for viruses/malware
- [ ] Code sign installer (optional)

## 🎉 Success!

Once built, you'll have a professional Windows installer that:
- Provides a polished installation experience
- Integrates properly with Windows
- Includes complete uninstallation
- Handles system configuration automatically
- Can be distributed as a single file

The installer demonstrates Displexity's professional quality and makes it easy for users to get started with the language.

## 📞 Support

If you encounter issues:
1. Check this README for common solutions
2. Verify NSIS installation
3. Check file paths and permissions
4. Test on different Windows versions
5. Review NSIS documentation at https://nsis.sourceforge.io/

Happy building! 🚀