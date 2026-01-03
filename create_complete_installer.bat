@echo off
echo ========================================
echo    Creating Complete Displexity Installer
echo ========================================
echo.

REM Compile the installers
echo [1/4] Compiling GUI installer...
bin\disp.exe gui_installer.dis -o gui_installer.exe
if errorlevel 1 (
    echo ERROR: Failed to compile GUI installer
    pause
    exit /b 1
)
echo   ✓ GUI installer compiled

echo [2/4] Compiling text installer...
bin\disp.exe installer_simple.dis -o installer_simple.exe
if errorlevel 1 (
    echo ERROR: Failed to compile text installer
    pause
    exit /b 1
)
echo   ✓ Text installer compiled

echo [3/4] Creating installer package...
if not exist installer_package mkdir installer_package

REM Copy installers
copy gui_installer.exe installer_package\ >nul
copy installer_simple.exe installer_package\ >nul
copy create_installer_wizard.bat installer_package\ >nul

REM Copy essential files for installation
if exist release (
    xcopy release installer_package\release\ /s /e /y /q >nul
) else (
    echo Creating minimal release structure...
    mkdir installer_package\release\bin
    mkdir installer_package\release\lib\displexity
    mkdir installer_package\release\include\displexity
    mkdir installer_package\release\examples
    mkdir installer_package\release\docs
    
    REM Copy available files
    if exist bin\disp.exe copy bin\disp.exe installer_package\release\bin\ >nul
    if exist examples xcopy examples installer_package\release\examples\ /s /e /y /q >nul
    if exist docs xcopy docs installer_package\release\docs\ /s /e /y /q >nul
    if exist release\lib xcopy release\lib installer_package\release\lib\ /s /e /y /q >nul
    if exist release\include xcopy release\include installer_package\release\include\ /s /e /y /q >nul
)

REM Create installer launcher
echo @echo off > installer_package\install.bat
echo echo ======================================== >> installer_package\install.bat
echo echo    Displexity Language Installer >> installer_package\install.bat
echo echo ======================================== >> installer_package\install.bat
echo echo. >> installer_package\install.bat
echo echo Choose installation method: >> installer_package\install.bat
echo echo 1. GUI Installer (Recommended) >> installer_package\install.bat
echo echo 2. Text Installer >> installer_package\install.bat
echo echo 3. Wizard Installer >> installer_package\install.bat
echo echo 4. Exit >> installer_package\install.bat
echo echo. >> installer_package\install.bat
echo set /p choice="Enter your choice (1-4): " >> installer_package\install.bat
echo. >> installer_package\install.bat
echo if "%%choice%%"=="1" ( >> installer_package\install.bat
echo     echo Starting GUI installer... >> installer_package\install.bat
echo     gui_installer.exe >> installer_package\install.bat
echo ^) else if "%%choice%%"=="2" ( >> installer_package\install.bat
echo     echo Starting text installer... >> installer_package\install.bat
echo     installer_simple.exe >> installer_package\install.bat
echo ^) else if "%%choice%%"=="3" ( >> installer_package\install.bat
echo     echo Starting wizard installer... >> installer_package\install.bat
echo     create_installer_wizard.bat >> installer_package\install.bat
echo ^) else if "%%choice%%"=="4" ( >> installer_package\install.bat
echo     echo Installation cancelled. >> installer_package\install.bat
echo     exit /b 0 >> installer_package\install.bat
echo ^) else ( >> installer_package\install.bat
echo     echo Invalid choice. Please run install.bat again. >> installer_package\install.bat
echo ^) >> installer_package\install.bat
echo pause >> installer_package\install.bat

REM Create README
echo # Displexity Language Installer Package > installer_package\README.md
echo. >> installer_package\README.md
echo This package contains multiple installation options for Displexity: >> installer_package\README.md
echo. >> installer_package\README.md
echo ## Installation Options >> installer_package\README.md
echo. >> installer_package\README.md
echo 1. **GUI Installer** (gui_installer.exe) >> installer_package\README.md
echo    - Professional graphical installer >> installer_package\README.md
echo    - Mouse and keyboard support >> installer_package\README.md
echo    - Component selection >> installer_package\README.md
echo    - Written in Displexity itself! >> installer_package\README.md
echo. >> installer_package\README.md
echo 2. **Text Installer** (installer_simple.exe) >> installer_package\README.md
echo    - Command-line installer >> installer_package\README.md
echo    - Quick and simple >> installer_package\README.md
echo    - Also written in Displexity! >> installer_package\README.md
echo. >> installer_package\README.md
echo 3. **Wizard Installer** (create_installer_wizard.bat) >> installer_package\README.md
echo    - Windows batch script >> installer_package\README.md
echo    - Interactive setup >> installer_package\README.md
echo    - Automatic PATH configuration >> installer_package\README.md
echo. >> installer_package\README.md
echo ## Quick Start >> installer_package\README.md
echo. >> installer_package\README.md
echo 1. Run `install.bat` to choose installation method >> installer_package\README.md
echo 2. Or run any installer directly >> installer_package\README.md
echo. >> installer_package\README.md
echo ## What Gets Installed >> installer_package\README.md
echo. >> installer_package\README.md
echo - Displexity compiler (disp.exe) >> installer_package\README.md
echo - Standard libraries (.disll files) >> installer_package\README.md
echo - Header files (.dish files) >> installer_package\README.md
echo - Example programs >> installer_package\README.md
echo - Documentation >> installer_package\README.md
echo - System PATH integration >> installer_package\README.md
echo - File associations >> installer_package\README.md
echo. >> installer_package\README.md
echo ## System Requirements >> installer_package\README.md
echo. >> installer_package\README.md
echo - Windows 10/11 (64-bit recommended) >> installer_package\README.md
echo - 100MB free disk space >> installer_package\README.md
echo - Administrator privileges (for system integration) >> installer_package\README.md

echo [4/4] Creating distribution archive...
if exist displexity-installer.zip del displexity-installer.zip
powershell -command "Compress-Archive -Path 'installer_package\*' -DestinationPath 'displexity-installer.zip' -Force" >nul 2>&1
if errorlevel 1 (
    echo WARNING: Could not create ZIP archive
    echo Manual packaging required
) else (
    echo   ✓ Created displexity-installer.zip
)

echo.
echo ========================================
echo    Installer Package Complete!
echo ========================================
echo.
echo Created files:
echo   - installer_package\install.bat (Main launcher)
echo   - installer_package\gui_installer.exe (GUI installer)
echo   - installer_package\installer_simple.exe (Text installer)
echo   - installer_package\create_installer_wizard.bat (Wizard)
echo   - installer_package\README.md (Documentation)
echo   - installer_package\release\ (Installation files)
echo   - displexity-installer.zip (Distribution archive)
echo.
echo To test: cd installer_package ^&^& install.bat
echo To distribute: Share displexity-installer.zip
echo.
pause