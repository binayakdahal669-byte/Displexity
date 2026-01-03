@echo off
echo Installing DISP Programming Language...

REM Check for admin privileges
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo Please run as Administrator
    pause
    exit /b 1
)

REM Build first if binaries don't exist
if not exist "bin\dispcompiler.exe" (
    echo Building DISP first...
    call build.bat
    if %errorlevel% neq 0 (
        echo Build failed!
        pause
        exit /b 1
    )
)

REM Install binaries
echo Installing binaries...
copy bin\dispcompiler.exe C:\Windows\System32\
copy bin\dispide.exe C:\Windows\System32\

REM Create program directory
if not exist "C:\Program Files\DISP" mkdir "C:\Program Files\DISP"
if not exist "C:\Program Files\DISP\examples" mkdir "C:\Program Files\DISP\examples"
if not exist "C:\Program Files\DISP\lib" mkdir "C:\Program Files\DISP\lib"

REM Install examples
echo Installing examples...
xcopy examples\*.* "C:\Program Files\DISP\examples\" /Y

REM Create standard library
echo Setting up standard library...
echo // DISP Standard Math Library > "C:\Program Files\DISP\lib\math.dish"
echo function sqrt(x) { return x ^ 0.5; } >> "C:\Program Files\DISP\lib\math.dish"
echo function pow(x, y) { return x ^ y; } >> "C:\Program Files\DISP\lib\math.dish"
echo function abs(x) { if (x ^< 0) return -x; else return x; } >> "C:\Program Files\DISP\lib\math.dish"

echo // DISP Standard Network Library > "C:\Program Files\DISP\lib\network.dish"
echo function http_get(url) { http url "GET" response; return response; } >> "C:\Program Files\DISP\lib\network.dish"
echo function http_post(url, data) { http url "POST" response; return response; } >> "C:\Program Files\DISP\lib\network.dish"

REM Create start menu shortcuts
echo Creating shortcuts...
powershell "$WshShell = New-Object -comObject WScript.Shell; $Shortcut = $WshShell.CreateShortcut('%APPDATA%\Microsoft\Windows\Start Menu\Programs\DISP IDE.lnk'); $Shortcut.TargetPath = 'C:\Windows\System32\dispide.exe'; $Shortcut.Save()"

REM Add file associations
echo Setting up file associations...
reg add "HKCR\.disp" /ve /d "DISPSourceFile" /f
reg add "HKCR\.dish" /ve /d "DISPHeaderFile" /f
reg add "HKCR\.disproj" /ve /d "DISPProjectFile" /f
reg add "HKCR\.disll" /ve /d "DISPLibraryFile" /f

reg add "HKCR\DISPSourceFile" /ve /d "DISP Source File" /f
reg add "HKCR\DISPSourceFile\shell\open\command" /ve /d "dispide.exe \"%%1\"" /f
reg add "HKCR\DISPSourceFile\shell\compile\command" /ve /d "dispcompiler.exe \"%%1\"" /f

echo DISP Programming Language installed successfully!
echo.
echo Usage:
echo   dispcompiler --help    - Show compiler help
echo   dispide               - Start the IDE
echo   dispcompiler hello.disp -o hello.dis  - Compile a program
echo.
echo Examples are available in: C:\Program Files\DISP\examples\
echo Standard libraries in: C:\Program Files\DISP\lib\
echo.
echo You can now double-click .disp files to open them in the IDE!
echo.
pause