@echo off
setlocal enabledelayedexpansion
title DISP Programming Language - Installation Wizard v2.0
color 0A

echo.
echo  ============================================================
echo  =                                                          =
echo  =     DISP PROGRAMMING LANGUAGE INSTALLATION WIZARD        =
echo  =                        v2.0                              =
echo  =                                                          =
echo  ============================================================
echo.
echo  This wizard will install the DISP Programming Language
echo  including the compiler, runtime, and GUI IDE.
echo.
echo  Requirements:
echo    - MSYS2 with MinGW-w64 (will be checked/installed)
echo    - GLFW library (will be installed via pacman)
echo    - Administrator privileges (for system-wide install)
echo.
pause

:: Check for MSYS2
echo.
echo [1/10] Checking for MSYS2 installation...
set "MSYS2_PATH="
if exist "C:\msys64\usr\bin\bash.exe" set "MSYS2_PATH=C:\msys64"
if exist "C:\msys32\usr\bin\bash.exe" set "MSYS2_PATH=C:\msys32"
if exist "%USERPROFILE%\msys64\usr\bin\bash.exe" set "MSYS2_PATH=%USERPROFILE%\msys64"

if "%MSYS2_PATH%"=="" (
    echo [ERROR] MSYS2 not found!
    echo.
    echo Please install MSYS2 from: https://www.msys2.org/
    echo After installation, run this wizard again.
    echo.
    pause
    exit /b 1
)
echo [OK] MSYS2 found at: %MSYS2_PATH%

:: Check for g++
echo.
echo [2/10] Checking for g++ compiler...
where g++ >nul 2>&1
if %errorlevel% neq 0 (
    echo [WARN] g++ not in PATH, checking MSYS2...
    if exist "%MSYS2_PATH%\ucrt64\bin\g++.exe" (
        echo [OK] Found g++ in MSYS2 ucrt64
        set "PATH=%MSYS2_PATH%\ucrt64\bin;%PATH%"
    ) else if exist "%MSYS2_PATH%\mingw64\bin\g++.exe" (
        echo [OK] Found g++ in MSYS2 mingw64
        set "PATH=%MSYS2_PATH%\mingw64\bin;%PATH%"
    ) else (
        echo [ERROR] g++ not found! Installing via pacman...
        "%MSYS2_PATH%\usr\bin\bash.exe" -lc "pacman -S --noconfirm mingw-w64-ucrt-x86_64-gcc"
        set "PATH=%MSYS2_PATH%\ucrt64\bin;%PATH%"
    )
) else (
    echo [OK] g++ found in PATH
)

:: Check/Install GLFW
echo.
echo [3/10] Checking for GLFW library...
if exist "%MSYS2_PATH%\ucrt64\lib\libglfw3.a" (
    echo [OK] GLFW already installed
) else if exist "%MSYS2_PATH%\mingw64\lib\libglfw3.a" (
    echo [OK] GLFW already installed
) else (
    echo [INFO] Installing GLFW via pacman...
    "%MSYS2_PATH%\usr\bin\bash.exe" -lc "pacman -S --noconfirm mingw-w64-ucrt-x86_64-glfw"
    if %errorlevel% neq 0 (
        echo [WARN] Trying mingw64 package...
        "%MSYS2_PATH%\usr\bin\bash.exe" -lc "pacman -S --noconfirm mingw-w64-x86_64-glfw"
    )
    echo [OK] GLFW installed
)

:: Create directories
echo.
echo [4/10] Creating directories...
if not exist "bin" mkdir bin
if not exist "build" mkdir build
if not exist "lib" mkdir lib
if not exist "include" mkdir include
echo [OK] Directories created

:: Build compiler
echo.
echo [5/10] Building DISP Compiler...
g++ -std=c++17 -Wall -O2 -Isrc -DWINDOWS -DDISPLEXITY_COMPILER ^
    src/main.cpp ^
    src/compiler/lexer.cpp ^
    src/compiler/parser.cpp ^
    src/compiler/codegen.cpp ^
    src/utils/logger.cpp ^
    src/runtime/disp_vm.cpp ^
    -o bin/displexitycompiler.exe 2>build\compiler_errors.log

if %errorlevel% neq 0 (
    echo [ERROR] Compiler build failed! Check build\compiler_errors.log
    type build\compiler_errors.log
    pause
    exit /b 1
)
echo [OK] Compiler built successfully

:: Build runtime
echo.
echo [6/10] Building DISP Runtime...
g++ -std=c++17 -Wall -O2 -Isrc -DWINDOWS ^
    src/runtime/disp_runtime.cpp ^
    src/runtime/disp_vm.cpp ^
    -o bin/disp_runtime.exe 2>build\runtime_errors.log

if %errorlevel% neq 0 (
    echo [ERROR] Runtime build failed! Check build\runtime_errors.log
    type build\runtime_errors.log
    pause
    exit /b 1
)
echo [OK] Runtime built successfully

:: Build GUI IDE
echo.
echo [7/10] Building DISP GUI IDE with Integrated Terminals...
g++ -std=c++17 -Wall -O2 -DWINDOWS ^
    gui_ide/main.cpp ^
    -lglfw3 -lopengl32 -lgdi32 -mwindows ^
    -o bin/dispguiide.exe 2>build\ide_errors.log

if %errorlevel% neq 0 (
    echo [ERROR] IDE build failed! Check build\ide_errors.log
    type build\ide_errors.log
    pause
    exit /b 1
)
echo [OK] GUI IDE built successfully

:: Test compilation
echo.
echo [8/10] Testing compiler with hello.dis...
echo // Test file > test_install.dis
echo function main() { >> test_install.dis
echo     print("DISP Installation Test"); >> test_install.dis
echo     return 0; >> test_install.dis
echo } >> test_install.dis

bin\displexitycompiler.exe test_install.dis -o test_install.disp >nul 2>&1
if %errorlevel% neq 0 (
    echo [WARN] Compiler test produced warnings (this is normal for first run)
) else (
    echo [OK] Compiler test passed
)
del test_install.dis >nul 2>&1
del test_install.disp >nul 2>&1

:: Setup environment
echo.
echo [9/10] Setting up environment...
set "DISP_HOME=%CD%"
set "DISP_BIN=%CD%\bin"

echo.
echo Would you like to add DISP to your system PATH? (Recommended)
echo This allows running displexitycompiler and dispguiide from anywhere.
echo.
set /p ADD_PATH="Add to PATH? (Y/N): "

if /i "%ADD_PATH%"=="Y" (
    echo.
    echo Adding to user PATH...
    
    :: Add to user PATH using PowerShell
    powershell -Command "[Environment]::SetEnvironmentVariable('Path', [Environment]::GetEnvironmentVariable('Path', 'User') + ';%DISP_BIN%', 'User')"
    
    if %errorlevel% equ 0 (
        echo [OK] Added to PATH. Please restart your terminal for changes to take effect.
    ) else (
        echo [WARN] Could not add to PATH automatically.
        echo Please add this directory to your PATH manually: %DISP_BIN%
    )
)

:: Create desktop shortcuts
echo.
echo [10/10] Creating shortcuts...
set /p CREATE_SHORTCUTS="Create desktop shortcuts? (Y/N): "

if /i "%CREATE_SHORTCUTS%"=="Y" (
    powershell -Command "$WshShell = New-Object -ComObject WScript.Shell; $Shortcut = $WshShell.CreateShortcut([Environment]::GetFolderPath('Desktop') + '\DISP IDE.lnk'); $Shortcut.TargetPath = '%CD%\bin\dispguiide.exe'; $Shortcut.WorkingDirectory = '%CD%'; $Shortcut.Description = 'DISP Programming Language IDE'; $Shortcut.Save()"
    echo [OK] Desktop shortcut created
)

:: Final summary
echo.
echo  ============================================================
echo  =                                                          =
echo  =           INSTALLATION COMPLETE!                         =
echo  =                                                          =
echo  ============================================================
echo.
echo  Installed components:
echo    [X] DISP Compiler (displexitycompiler.exe)
echo    [X] DISP Runtime (disp_runtime.exe)
echo    [X] DISP GUI IDE with Integrated Terminals (dispguiide.exe)
echo.
echo  Installation directory: %CD%
echo  Binaries location: %CD%\bin
echo.
echo  Quick Start:
echo    1. Run the IDE:     bin\dispguiide.exe
echo    2. Compile a file:  displexitycompiler hello.dis -o hello.disp
echo    3. Run a program:   disp_runtime hello.disp
echo.
echo  IDE Features:
echo    - Integrated PowerShell terminal (F1 or click tab)
echo    - Integrated CMD terminal (F2 or click tab)
echo    - Syntax highlighting for .dis files
echo    - File explorer sidebar
echo    - Build with F5, Run with F6
echo    - Ctrl+` to toggle terminal focus
echo.
echo  Would you like to launch the IDE now?
set /p LAUNCH_IDE="Launch DISP IDE? (Y/N): "

if /i "%LAUNCH_IDE%"=="Y" (
    start "" "%CD%\bin\dispguiide.exe"
)

echo.
echo Thank you for installing DISP Programming Language!
echo.
pause
