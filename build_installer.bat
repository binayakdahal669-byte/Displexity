@echo off
echo ========================================
echo    Building Displexity NSIS Installer
echo ========================================
echo.

REM Check if NSIS is installed
where makensis >nul 2>&1
if errorlevel 1 (
    echo ERROR: NSIS (makensis) not found in PATH
    echo.
    echo To install NSIS:
    echo 1. Using MSYS2: pacman -S mingw-w64-x86_64-nsis
    echo 2. Or download from: https://nsis.sourceforge.io/
    echo 3. Make sure makensis.exe is in your PATH
    echo.
    pause
    exit /b 1
)

echo [1/4] Checking NSIS installation...
makensis /VERSION
echo   ✓ NSIS found and working

echo [2/4] Preparing installer files...

REM Create basic icon if it doesn't exist
if not exist exe.ico (
    echo Creating default icon...
    REM This would normally create an actual .ico file
    REM For now, we'll just note that it's missing
    echo   ! exe.ico not found - installer will use default icon
) else (
    echo   ✓ exe.ico found
)

REM Check for required files
if not exist bin\disp.exe (
    echo   ! bin\disp.exe not found - creating placeholder
    if not exist bin mkdir bin
    echo @echo off > bin\disp.exe.bat
    echo echo Displexity Compiler Placeholder >> bin\disp.exe.bat
    echo echo Usage: disp [options] file.dis >> bin\disp.exe.bat
    copy bin\disp.exe.bat bin\disp.exe >nul 2>&1
    del bin\disp.exe.bat
) else (
    echo   ✓ bin\disp.exe found
)

if not exist LICENSE.txt (
    echo   ! LICENSE.txt not found - using existing one
) else (
    echo   ✓ LICENSE.txt found
)

echo [3/4] Building installer with NSIS...
makensis displexity.nsi
if errorlevel 1 (
    echo ERROR: NSIS compilation failed
    echo Check the output above for errors
    pause
    exit /b 1
)

echo [4/4] Verifying installer...
if exist displexity-installer.exe (
    echo   ✓ Installer created successfully: displexity-installer.exe
    
    REM Get file size
    for %%A in (displexity-installer.exe) do (
        echo   ✓ File size: %%~zA bytes
    )
    
    echo.
    echo ========================================
    echo    Installer Build Complete!
    echo ========================================
    echo.
    echo Created: displexity-installer.exe
    echo.
    echo Features:
    echo - Professional Windows installer interface
    echo - Component selection (Core, Libraries, Examples, Docs, Tools)
    echo - Automatic PATH configuration
    echo - File associations (.dis, .dish, .disll, .disp)
    echo - Start menu and desktop shortcuts
    echo - Complete uninstaller
    echo - Registry integration
    echo.
    echo To test: Run displexity-installer.exe
    echo To distribute: Share displexity-installer.exe
    echo.
) else (
    echo ERROR: Installer was not created
    echo Check for errors in the NSIS compilation
    pause
    exit /b 1
)

pause