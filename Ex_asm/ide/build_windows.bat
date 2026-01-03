@echo off
REM Build script for Displexity IDE (Windows, MSYS2 UCRT64)
setlocal







endlocalecho Built ide\displexity-ide.exeg++ -O2 -std=c++17 -municode -mwindows -I/mingw64/include ide/main.cpp -o ide\displexity-ide.exe -lglfw -lvulkanset LIB=%LIB%;C:\msys64\ucrt64\lib
nset INCLUDE=%INCLUDE%;C:\msys64\ucrt64\includenREM Adjust paths if using different MSYS2 layout