@echo off
REM Packaging script for Windows

setlocal enabledelayedexpansion

set VERSION=0.1.0
set DIST=dist

if not exist %DIST% mkdir %DIST%

echo === Displexity Packaging Script (Windows) ===

REM Build
echo Building...
call build.bat

REM Create ZIP (requires PowerShell or 7-Zip)
set ZIPNAME=displexity-%VERSION%-windows.zip

REM Try with PowerShell
powershell -Command "Compress-Archive -Path build\*.exe -DestinationPath %DIST%\%ZIPNAME% -Force"

if exist %DIST%\%ZIPNAME% (
    echo Packaged: %DIST%\%ZIPNAME%
) else (
    echo Warning: Could not create ZIP. Copy build\*.exe manually.
)

echo Packaging complete!
