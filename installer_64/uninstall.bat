@echo off
title DISP Language Uninstaller
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo Please run as Administrator!
    pause
    exit /b 1
)
echo Uninstalling DISP Language...
rmdir /s /q "C:\Program Files\DISP"
del "%USERPROFILE%\Desktop\DISP IDE.lnk" 2>nul
echo DISP Language uninstalled.
pause
