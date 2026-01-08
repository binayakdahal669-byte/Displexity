@echo off
echo Building Displexity Compiler...

if not exist bin mkdir bin

REM Optional: accept DISLLS as first arg or from environment (comma-separated patterns)
if "%~1"=="" (
    if defined DISLLS (
        echo Using environment DISLLS=%DISLLS%
    ) else (
        set DISLLS=
    )
) else (
    if not defined DISLLS set DISLLS=%~1
)

if defined DISLLS (
    echo Linking dislls: %DISLLS%
    python scripts\link_dislls.py "%DISLLS%" || python3 scripts\link_dislls.py "%DISLLS%"
)

REM Build the compiler
REM Generate icons (logs, nvim)
echo Generating icons...
python scripts\generate_icons.py release\resources || python3 scripts\generate_icons.py release\resources || echo Could not generate icons (Pillow missing)

REM Compile resources (requires windres)
where windres >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo Compiling resources with windres
    windres src\disp.rc -O coff -o src\disp_res.o
    set RES_OBJ=src\disp_res.o
) else (
    set RES_OBJ=
)

REM Build the compiler and link resource object if present
if defined RES_OBJ (
    g++ -std=c++17 -O2 -o bin/displexitycompiler.exe src/disp.cpp src\displexity_disll_loader.cpp %RES_OBJ%
) else (
    g++ -std=c++17 -O2 -o bin/displexitycompiler.exe src/disp.cpp src\displexity_disll_loader.cpp
)

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
