@echo off
REM Create release folder structure for 32-bit and 64-bit builds

echo Creating release folder structure...

REM Create main directories
mkdir release\bin\win32 2>nul
mkdir release\bin\win64 2>nul
mkdir release\lib\displexity 2>nul
mkdir release\include\displexity 2>nul
mkdir release\docs 2>nul
mkdir release\examples 2>nul
mkdir release\resources 2>nul
mkdir release\languages\gcc 2>nul
mkdir release\languages\python 2>nul

echo.
echo Release structure:
echo.
echo release/
echo ├── bin/
echo │   ├── win32/          (32-bit executables)
echo │   └── win64/          (64-bit executables)
echo ├── lib/
echo │   └── displexity/     (Standard libraries .disll)
echo ├── include/
echo │   └── displexity/     (Headers .dish)
echo ├── docs/               (Documentation)
echo ├── examples/           (Example programs)
echo ├── resources/          (Icons, assets)
echo └── languages/          (Optional language tools)
echo.

REM Copy existing files if present
if exist bin\disp.exe (
    copy bin\disp.exe release\bin\win64\ >nul 2>&1
    echo   [✓] Copied compiler to release\bin\win64\
)

if exist bin\displexitycompiler.exe (
    copy bin\displexitycompiler.exe release\bin\win64\ >nul 2>&1
    echo   [✓] Copied displexitycompiler to release\bin\win64\
)

REM Copy documentation
if exist docs\*.md (
    copy docs\*.md release\docs\ >nul 2>&1
    echo   [✓] Copied documentation
)

if exist docs\*.html (
    copy docs\*.html release\docs\ >nul 2>&1
)

REM Copy examples
if exist examples\*.dis (
    copy examples\*.dis release\examples\ >nul 2>&1
    echo   [✓] Copied examples
)

REM Copy icons/resources
if exist exe.ico copy exe.ico release\resources\ >nul 2>&1
if exist disp.ico copy disp.ico release\resources\ >nul 2>&1
if exist ide.ico copy ide.ico release\resources\ >nul 2>&1
if exist tui.ico copy tui.ico release\resources\ >nul 2>&1
if exist package.ico copy package.ico release\resources\ >nul 2>&1

if exist release\resources\*.ico (
    echo   [✓] Copied icons
)

echo.
echo Creating version file...
echo Displexity v1.2.0 > release\VERSION.txt
echo Built on %date% %time% >> release\VERSION.txt
echo.
echo [✓] Release folder structure created successfully!
echo.
echo Next steps:
echo 1. Build 32-bit compiler: build_x86.bat
echo 2. Build 64-bit installer: makensis displexity-x64.nsi
echo 3. Build 32-bit installer: makensis displexity-x86.nsi
echo.
pause
