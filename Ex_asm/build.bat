@echo off
REM Build script for Displexity on Windows
REM Requires MinGW/GCC in PATH

setlocal enabledelayedexpansion

echo === Building Displexity Compiler ===

if not exist build mkdir build

echo Compiling disp...
g++ -std=c++17 -O2 compiler/disp.cpp -o build/disp.exe

echo Compiling IDE...
g++ -std=c++17 -O2 ide/main.cpp -o build/displexity-ide.exe

echo Build complete!
echo   Compiler: build\disp.exe
echo   IDE: build\displexity-ide.exe

if "%1"=="test" (
    echo.
    echo Running tests...
    
    build\disp.exe examples\hello.dis -o build\hello.c
    gcc build\hello.c -o build\hello.exe
    echo Test: hello.dis
    build\hello.exe
    
    build\disp.exe examples\math.dis -o build\math.c
    gcc build\math.c -o build\math.exe
    echo Test: math.dis
    build\math.exe
    
    build\disp.exe examples\conditional.dis -o build\conditional.c
    gcc build\conditional.c -o build\conditional.exe
    echo Test: conditional.dis
    build\conditional.exe
)
