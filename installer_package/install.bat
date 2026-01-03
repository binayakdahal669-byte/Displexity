@echo off 
echo ======================================== 
echo    Displexity Language Installer 
echo ======================================== 
echo. 
echo Choose installation method: 
echo 1. GUI Installer (Recommended) 
echo 2. Text Installer 
echo 3. Wizard Installer 
echo 4. Exit 
echo. 
set /p choice="Enter your choice (1-4): " 
 
if "%choice%"=="1" ( 
    echo Starting GUI installer... 
    gui_installer.exe 
) else if "%choice%"=="2" ( 
    echo Starting text installer... 
    installer_simple.exe 
) else if "%choice%"=="3" ( 
    echo Starting wizard installer... 
    create_installer_wizard.bat 
) else if "%choice%"=="4" ( 
    echo Installation cancelled. 
    exit /b 0 
) else ( 
    echo Invalid choice. Please run install.bat again. 
) 
pause 
