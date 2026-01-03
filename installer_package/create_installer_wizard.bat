@echo off
setlocal enabledelayedexpansion

echo ========================================
echo    Displexity Language Installer
echo ========================================
echo.
echo This installer will:
echo - Install Displexity compiler and tools
echo - Add Displexity to your system PATH
echo - Install examples and documentation
echo - Set up the development environment
echo.

REM Check for administrator privileges
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo WARNING: Running without administrator privileges.
    echo Some features may not work properly.
    echo.
    pause
)

REM Get installation directory
set "DEFAULT_DIR=C:\Displexity"
set /p "INSTALL_DIR=Installation directory [%DEFAULT_DIR%]: "
if "%INSTALL_DIR%"=="" set "INSTALL_DIR=%DEFAULT_DIR%"

echo.
echo Installing to: %INSTALL_DIR%
echo.

REM Create installation directory
if not exist "%INSTALL_DIR%" (
    echo Creating installation directory...
    mkdir "%INSTALL_DIR%" 2>nul
    if errorlevel 1 (
        echo ERROR: Cannot create directory %INSTALL_DIR%
        echo Please run as administrator or choose a different directory.
        pause
        exit /b 1
    )
)

REM Copy files
echo [1/5] Installing compiler and tools...
if exist release\bin (
    xcopy release\bin "%INSTALL_DIR%\bin\" /s /e /y /q >nul
    if errorlevel 1 (
        echo ERROR: Failed to copy compiler files
        pause
        exit /b 1
    )
    echo   - Compiler installed
) else if exist bin (
    xcopy bin "%INSTALL_DIR%\bin\" /s /e /y /q >nul
    echo   - Compiler installed
) else (
    echo ERROR: Compiler files not found
    pause
    exit /b 1
)

echo [2/5] Installing examples...
if exist release\examples (
    xcopy release\examples "%INSTALL_DIR%\examples\" /s /e /y /q >nul
    echo   - Examples installed
) else if exist examples (
    xcopy examples "%INSTALL_DIR%\examples\" /s /e /y /q >nul
    echo   - Examples installed
) else (
    echo   - No examples found (skipping)
)

echo [3/5] Installing libraries...
if exist release\lib (
    xcopy release\lib "%INSTALL_DIR%\lib\" /s /e /y /q >nul
    echo   - Libraries installed
) else if exist lib (
    xcopy lib "%INSTALL_DIR%\lib\" /s /e /y /q >nul
    echo   - Libraries installed
) else (
    mkdir "%INSTALL_DIR%\lib" 2>nul
    echo   - Library directory created
)

echo [4/5] Installing documentation...
if exist release\docs (
    xcopy release\docs "%INSTALL_DIR%\docs\" /s /e /y /q >nul
    echo   - Documentation installed
) else if exist docs (
    xcopy docs "%INSTALL_DIR%\docs\" /s /e /y /q >nul
    echo   - Documentation installed
) else (
    echo   - No documentation found (skipping)
)

echo [5/5] Installing IDE configuration...
if exist release\ide (
    xcopy release\ide "%INSTALL_DIR%\ide\" /s /e /y /q >nul
    echo   - IDE configuration installed
) else if exist gui_ide (
    xcopy gui_ide "%INSTALL_DIR%\ide\" /s /e /y /q >nul
    echo   - IDE configuration installed
) else (
    echo   - No IDE configuration found (skipping)
)

REM Add to PATH
echo.
echo Adding Displexity to system PATH...
set "NEW_PATH=%INSTALL_DIR%\bin"

REM Check if already in PATH
echo %PATH% | findstr /i "%NEW_PATH%" >nul
if errorlevel 1 (
    REM Not in PATH, add it
    setx PATH "%PATH%;%NEW_PATH%" >nul 2>&1
    if errorlevel 1 (
        echo WARNING: Could not add to system PATH automatically.
        echo Please manually add this to your PATH: %NEW_PATH%
    ) else (
        echo   - Added to system PATH successfully
    )
) else (
    echo   - Already in system PATH
)

REM Create desktop shortcuts (optional)
echo.
set /p "CREATE_SHORTCUTS=Create desktop shortcuts? [Y/n]: "
if /i not "%CREATE_SHORTCUTS%"=="n" (
    if exist "%USERPROFILE%\Desktop" (
        echo Creating desktop shortcuts...
        
        REM Create compiler shortcut
        echo @echo off > "%USERPROFILE%\Desktop\Displexity Compiler.bat"
        echo cd /d "%INSTALL_DIR%\bin" >> "%USERPROFILE%\Desktop\Displexity Compiler.bat"
        echo cmd /k "echo Displexity Compiler Ready. Type 'disp --help' for usage." >> "%USERPROFILE%\Desktop\Displexity Compiler.bat"
        
        REM Create examples shortcut
        echo @echo off > "%USERPROFILE%\Desktop\Displexity Examples.bat"
        echo cd /d "%INSTALL_DIR%\examples" >> "%USERPROFILE%\Desktop\Displexity Examples.bat"
        echo start . >> "%USERPROFILE%\Desktop\Displexity Examples.bat"
        
        echo   - Desktop shortcuts created
    )
)

REM Test installation
echo.
echo Testing installation...
"%INSTALL_DIR%\bin\disp.exe" --version >nul 2>&1
if errorlevel 1 (
    echo WARNING: Installation test failed
    echo The compiler may not work properly
) else (
    echo   - Installation test passed
)

REM Create uninstaller
echo.
echo Creating uninstaller...
echo @echo off > "%INSTALL_DIR%\uninstall.bat"
echo echo Uninstalling Displexity... >> "%INSTALL_DIR%\uninstall.bat"
echo rmdir /s /q "%INSTALL_DIR%" >> "%INSTALL_DIR%\uninstall.bat"
echo echo Displexity has been uninstalled. >> "%INSTALL_DIR%\uninstall.bat"
echo pause >> "%INSTALL_DIR%\uninstall.bat"

echo.
echo ========================================
echo    Installation Complete!
echo ========================================
echo.
echo Displexity has been installed to: %INSTALL_DIR%
echo.
echo Quick Start:
echo   1. Open a new Command Prompt
echo   2. Type: disp --help
echo   3. Try: disp "%INSTALL_DIR%\examples\hello.dis"
echo.
echo Documentation: %INSTALL_DIR%\docs\README.md
echo Examples: %INSTALL_DIR%\examples\
echo.
echo To uninstall: Run %INSTALL_DIR%\uninstall.bat
echo.
echo Thank you for installing Displexity!
echo.
pause