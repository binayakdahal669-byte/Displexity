@echo off
echo ========================================
echo    Displexity Installer Environment Setup
echo ========================================
echo.

echo This script will help you set up the environment for building
echo professional Windows installers for Displexity.
echo.

REM Check if we're in MSYS2 environment
if defined MSYSTEM (
    echo Detected MSYS2 environment: %MSYSTEM%
    echo.
    
    echo [1/3] Installing NSIS via MSYS2...
    echo Running: pacman -S --needed mingw-w64-x86_64-nsis
    pacman -S --needed mingw-w64-x86_64-nsis
    
    if errorlevel 1 (
        echo WARNING: NSIS installation may have failed
        echo You can try installing manually with:
        echo   pacman -S mingw-w64-x86_64-nsis
        echo.
    ) else (
        echo   ✓ NSIS installed successfully
    )
    
    echo [2/3] Checking NSIS installation...
    where makensis >nul 2>&1
    if errorlevel 1 (
        echo   ! makensis not found in PATH
        echo   Adding MSYS2 mingw64 bin to PATH...
        set "PATH=%PATH%;C:\msys64\mingw64\bin"
        where makensis >nul 2>&1
        if errorlevel 1 (
            echo   ! Still not found. Please check your MSYS2 installation.
        ) else (
            echo   ✓ makensis found after PATH update
        )
    ) else (
        echo   ✓ makensis found in PATH
    )
    
) else (
    echo Not in MSYS2 environment.
    echo.
    echo To install NSIS, you have several options:
    echo.
    echo Option 1 - MSYS2 (Recommended):
    echo   1. Install MSYS2 from https://www.msys2.org/
    echo   2. Open MSYS2 terminal
    echo   3. Run: pacman -S mingw-w64-x86_64-nsis
    echo   4. Add C:\msys64\mingw64\bin to your PATH
    echo.
    echo Option 2 - Direct Download:
    echo   1. Download NSIS from https://nsis.sourceforge.io/
    echo   2. Install to default location
    echo   3. Add NSIS installation directory to PATH
    echo.
    echo Option 3 - Chocolatey:
    echo   1. Install Chocolatey if not already installed
    echo   2. Run: choco install nsis
    echo.
    
    REM Check if NSIS is already installed
    where makensis >nul 2>&1
    if errorlevel 1 (
        echo Current status: NSIS not found in PATH
    ) else (
        echo Current status: NSIS is already installed and available
        makensis /VERSION
    )
)

echo.
echo [3/3] Setting up installer build environment...

REM Create necessary directories
if not exist installer_build mkdir installer_build
echo   ✓ Created installer_build directory

REM Copy installer files to build directory
copy displexity.nsi installer_build\ >nul 2>&1
copy EnvVarUpdate.nsh installer_build\ >nul 2>&1
copy LICENSE.txt installer_build\ >nul 2>&1
copy build_installer.bat installer_build\ >nul 2>&1

echo   ✓ Copied installer scripts to build directory

REM Copy application files if they exist
if exist bin (
    xcopy bin installer_build\bin\ /s /e /y /q >nul 2>&1
    echo   ✓ Copied bin directory
) else (
    echo   ! bin directory not found
)

if exist release (
    xcopy release installer_build\release\ /s /e /y /q >nul 2>&1
    echo   ✓ Copied release directory
) else (
    echo   ! release directory not found
)

if exist examples (
    xcopy examples installer_build\examples\ /s /e /y /q >nul 2>&1
    echo   ✓ Copied examples directory
) else (
    echo   ! examples directory not found
)

if exist docs (
    xcopy docs installer_build\docs\ /s /e /y /q >nul 2>&1
    echo   ✓ Copied docs directory
) else (
    echo   ! docs directory not found
)

REM Create build script in installer_build directory
echo @echo off > installer_build\build.bat
echo cd /d "%%~dp0" >> installer_build\build.bat
echo echo Building Displexity installer... >> installer_build\build.bat
echo makensis displexity.nsi >> installer_build\build.bat
echo if errorlevel 1 ( >> installer_build\build.bat
echo     echo Build failed! >> installer_build\build.bat
echo     pause >> installer_build\build.bat
echo     exit /b 1 >> installer_build\build.bat
echo ^) >> installer_build\build.bat
echo echo Build successful! >> installer_build\build.bat
echo echo Created: displexity-installer.exe >> installer_build\build.bat
echo pause >> installer_build\build.bat

echo   ✓ Created build.bat in installer_build directory

echo.
echo ========================================
echo    Environment Setup Complete!
echo ========================================
echo.
echo Next steps:
echo 1. Navigate to installer_build directory
echo 2. Run build.bat to create the installer
echo.
echo Files created:
echo   installer_build\displexity.nsi      (NSIS script)
echo   installer_build\EnvVarUpdate.nsh    (PATH helper)
echo   installer_build\LICENSE.txt         (License file)
echo   installer_build\build.bat           (Build script)
echo   installer_build\bin\                (Application files)
echo   installer_build\release\            (Release files)
echo.

REM Test NSIS one more time
echo Testing NSIS installation...
where makensis >nul 2>&1
if errorlevel 1 (
    echo ❌ NSIS not found - please install NSIS first
    echo.
    echo Quick install with MSYS2:
    echo   pacman -S mingw-w64-x86_64-nsis
    echo.
) else (
    echo ✅ NSIS is ready to use
    makensis /VERSION
    echo.
    echo Ready to build installer!
    echo Run: cd installer_build ^&^& build.bat
)

echo.
pause