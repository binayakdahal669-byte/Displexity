@echo off
title DISP Language Installer
color 1F
cls

    ====================================================
             DISP Programming Language Installer
                       64-bit Edition
    ====================================================

    This will install:
      - DISP Compiler (displexitycompiler.exe)
      - DISP Runtime (disp_runtime.exe)
      - DISP IDE (dispide.exe)
      - Examples and Documentation

    Install location: C:\Program Files\DISP

    Press any key to continue or Ctrl+C to cancel...
pause >nul

net session >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Please run as Administrator!
    echo Right-click and select "Run as administrator"
    pause
    exit /b 1
)

echo Installing DISP Language...
echo.

echo [1/4] Creating directories...
if not exist "C:\Program Files\DISP" mkdir "C:\Program Files\DISP"
if not exist "C:\Program Files\DISP\bin" mkdir "C:\Program Files\DISP\bin"
if not exist "C:\Program Files\DISP\examples" mkdir "C:\Program Files\DISP\examples"
if not exist "C:\Program Files\DISP\docs" mkdir "C:\Program Files\DISP\docs"

echo [2/4] Copying files...
copy /Y "%~dp0bin\*.*" "C:\Program Files\DISP\bin\"
copy /Y "%~dp0examples\*.*" "C:\Program Files\DISP\examples\"
copy /Y "%~dp0docs\*.*" "C:\Program Files\DISP\docs\"

echo [3/4] Adding to PATH...
setx PATH "%PATH%;C:\Program Files\DISP\bin" /M

echo [4/4] Creating shortcuts...
powershell "$ws = New-Object -ComObject WScript.Shell; $s = $ws.CreateShortcut('%USERPROFILE%\Desktop\DISP IDE.lnk'); $s.TargetPath = 'C:\Program Files\DISP\bin\dispide.exe'; $s.Save()"

echo.
echo ====================================================
echo   Installation Complete!
echo ====================================================
echo.
echo   You can now use:
echo     - dispide          (GUI IDE)
echo     - displexitycompiler (Compiler)
echo     - disp_runtime     (Runtime)
echo.
echo   Desktop shortcut created: DISP IDE
echo.
echo   NOTE: You may need to restart your terminal for
echo         PATH changes to take effect.
echo.
pause
