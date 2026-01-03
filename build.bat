@echo off
echo Building Displexity Compiler...

if not exist bin mkdir bin

REM Build the compiler
g++ -std=c++17 -O2 -o bin/displexitycompiler.exe src/disp.cpp

if %ERRORLEVEL% EQU 0 (
    echo Build successful: bin/displexitycompiler.exe
    copy bin\displexitycompiler.exe bin\disp.exe >nul
    echo Also copied to: bin/disp.exe
) else (
    echo Build failed!
    exit /b 1
)

REM Build tools if they exist
if exist src\tools\bootgen.cpp (
    echo Building bootgen...
    g++ -std=c++17 -O2 -o bin/bootgen.exe src/tools/bootgen.cpp
)

if exist src\tools\iso_builder.cpp (
    echo Building iso_builder...
    g++ -std=c++17 -O2 -o bin/iso_builder.exe src/tools/iso_builder.cpp
)

echo Done!
