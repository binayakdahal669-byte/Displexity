@echo off
echo.
echo ========================================
echo  Building Displexity Installers
echo ========================================
echo.

REM Check if NSIS is installed
where makensis >nul 2>&1
if errorlevel 1 (
    echo ERROR: NSIS (makensis) not found in PATH
    echo.
    echo To install NSIS:
    echo 1. Download: https://nsis.sourceforge.io/Download
    echo 2. Install and add to PATH
    echo 3. Or install via MSYS2: pacman -S mingw-w64-x86_64-nsis
    echo.
    pause
    exit /b 1
)

echo [✓] NSIS found
echo.

REM Build 64-bit installer
echo Building 64-bit installer...
makensis displexity-x64.nsi
if errorlevel 1 (
    echo ERROR: 64-bit build failed
    pause
    exit /b 1
)
echo [✓] 64-bit installer created: displexity-x64-installer.exe
echo.

REM Build 32-bit installer
echo Building 32-bit installer...
makensis displexity-x86.nsi
if errorlevel 1 (
    echo ERROR: 32-bit build failed
    pause
    exit /b 1
)
echo [✓] 32-bit installer created: displexity-x86-installer.exe
echo.

REM Verify installers
if exist displexity-x64-installer.exe (
    for %%A in (displexity-x64-installer.exe) do (
        echo [✓] 64-bit: %%~zA bytes
    )
) else (
    echo [✗] 64-bit installer not found
)

if exist displexity-x86-installer.exe (
    for %%A in (displexity-x86-installer.exe) do (
        echo [✓] 32-bit: %%~zA bytes
    )
) else (
    echo [✗] 32-bit installer not found
)

echo.
echo ========================================
echo  Installation Complete!
echo ========================================
echo.
echo Installers created:
echo   displexity-x64-installer.exe (64-bit)
echo   displexity-x86-installer.exe (32-bit)
echo.
echo Next steps:
echo   1. Test the installers
echo   2. Upload to GitHub Releases
echo   3. Update documentation
echo.
pause
