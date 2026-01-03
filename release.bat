@echo off
setlocal enabledelayedexpansion
title DISP Programming Language - Release Builder v2.0
color 0B

echo.
echo  ============================================================
echo  =     DISP PROGRAMMING LANGUAGE - RELEASE BUILDER          =
echo  ============================================================
echo.

:: Set version
set "VERSION=2.0.0"
set "RELEASE_DIR=release\disp-%VERSION%"

:: Clean previous release
if exist "release" rmdir /s /q release
mkdir "%RELEASE_DIR%"
mkdir "%RELEASE_DIR%\bin"
mkdir "%RELEASE_DIR%\examples"
mkdir "%RELEASE_DIR%\src"
mkdir "%RELEASE_DIR%\gui_ide"
mkdir "%RELEASE_DIR%\docs"

echo [1/8] Building all components...

:: Build compiler
echo Building compiler...
g++ -std=c++17 -Wall -O2 -Isrc -DWINDOWS -DDISPLEXITY_COMPILER ^
    src/main.cpp src/compiler/lexer.cpp src/compiler/parser.cpp ^
    src/compiler/codegen.cpp src/utils/logger.cpp src/runtime/disp_vm.cpp ^
    -o "%RELEASE_DIR%\bin\displexitycompiler.exe"
if %errorlevel% neq 0 goto :error

:: Build runtime
echo Building runtime...
g++ -std=c++17 -Wall -O2 -Isrc -DWINDOWS ^
    src/runtime/disp_runtime.cpp src/runtime/disp_vm.cpp ^
    -o "%RELEASE_DIR%\bin\disp_runtime.exe"
if %errorlevel% neq 0 goto :error

:: Build GUI IDE
echo Building GUI IDE...
g++ -std=c++17 -Wall -O2 -DWINDOWS gui_ide/main.cpp ^
    -lglfw3 -lopengl32 -lgdi32 -mwindows ^
    -o "%RELEASE_DIR%\bin\dispguiide.exe"
if %errorlevel% neq 0 goto :error

echo [2/8] Copying source files...
xcopy /s /q "src\*" "%RELEASE_DIR%\src\" >nul
copy "gui_ide\main.cpp" "%RELEASE_DIR%\gui_ide\" >nul
copy "gui_ide\stb_truetype.h" "%RELEASE_DIR%\gui_ide\" >nul

echo [3/8] Copying examples...
xcopy /s /q "examples\*" "%RELEASE_DIR%\examples\" >nul

echo [4/8] Copying documentation...
copy "README.md" "%RELEASE_DIR%\" >nul 2>&1
copy "LANGUAGE_SPEC.md" "%RELEASE_DIR%\docs\" >nul 2>&1

echo [5/8] Copying build scripts...
copy "build.bat" "%RELEASE_DIR%\" >nul
copy "build.sh" "%RELEASE_DIR%\" >nul
copy "Makefile" "%RELEASE_DIR%\" >nul
copy "install_wizard.bat" "%RELEASE_DIR%\" >nul

echo [6/8] Creating README...
(
echo DISP Programming Language v%VERSION%
echo ================================
echo.
echo Quick Start:
echo   1. Run install_wizard.bat for guided installation
echo   2. Or manually run bin\dispguiide.exe for the IDE
echo.
echo Components:
echo   - displexitycompiler.exe : DISP Compiler
echo   - disp_runtime.exe       : DISP Runtime VM
echo   - dispguiide.exe         : GUI IDE with integrated terminals
echo.
echo File Extensions:
echo   .dis    - Source files
echo   .disp   - Executable files
echo   .dish   - Header files
echo   .disll  - Library files
echo   .disproj - Project files
echo.
echo IDE Shortcuts:
echo   F1          - PowerShell terminal
echo   F2          - CMD terminal
echo   F5          - Build
echo   F6          - Build and Run
echo   Ctrl+S      - Save
echo   Ctrl+`      - Toggle terminal focus
echo   ESC         - Exit terminal / Exit IDE
echo.
echo Requirements:
echo   - Windows 10/11
echo   - MSYS2 with MinGW-w64 ^(for building from source^)
echo   - GLFW library ^(installed via install_wizard.bat^)
) > "%RELEASE_DIR%\README.txt"

echo [7/8] Creating ZIP archive...
powershell -Command "Compress-Archive -Path '%RELEASE_DIR%\*' -DestinationPath 'release\disp-%VERSION%-windows.zip' -Force"

echo [8/8] Creating installer package info...
(
echo {
echo   "name": "DISP Programming Language",
echo   "version": "%VERSION%",
echo   "description": "Modern programming language with integrated IDE",
echo   "author": "DISP Team",
echo   "license": "MIT",
echo   "components": [
echo     "displexitycompiler.exe",
echo     "disp_runtime.exe", 
echo     "dispguiide.exe"
echo   ],
echo   "features": [
echo     "Integrated PowerShell terminal",
echo     "Integrated CMD terminal",
echo     "Syntax highlighting",
echo     "File explorer",
echo     "Build system integration"
echo   ]
echo }
) > "%RELEASE_DIR%\package.json"

echo.
echo  ============================================================
echo  =              RELEASE BUILD COMPLETE!                     =
echo  ============================================================
echo.
echo  Release directory: %RELEASE_DIR%
echo  ZIP archive: release\disp-%VERSION%-windows.zip
echo.
echo  Contents:
dir /b "%RELEASE_DIR%\bin"
echo.
pause
exit /b 0

:error
echo.
echo [ERROR] Build failed!
pause
exit /b 1
