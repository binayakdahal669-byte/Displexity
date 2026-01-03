@echo off
echo ============================================
echo   DISP Language 64-bit Installer Creator
echo ============================================
echo.

REM Build all components first
echo [1/5] Building compiler...
g++ -std=c++17 -Wall -O2 -Isrc -DWINDOWS -DDISPLEXITY_COMPILER ^
    src/main.cpp src/compiler/lexer.cpp src/compiler/parser.cpp ^
    src/compiler/codegen.cpp src/utils/logger.cpp src/runtime/disp_vm.cpp ^
    -o bin/displexitycompiler.exe
if %errorlevel% neq 0 (echo Compiler build failed! & pause & exit /b 1)

echo [2/5] Building runtime...
g++ -std=c++17 -Wall -O2 -Isrc -DWINDOWS ^
    src/runtime/disp_runtime.cpp src/runtime/disp_vm.cpp ^
    -o bin/disp_runtime.exe
if %errorlevel% neq 0 (echo Runtime build failed! & pause & exit /b 1)

echo [3/5] Building GUI IDE...
g++ -std=c++17 -Wall -O2 gui_ide/main.cpp ^
    -lcomctl32 -lgdi32 -lcomdlg32 -mwindows ^
    -o bin/dispide.exe
if %errorlevel% neq 0 (echo IDE build failed! & pause & exit /b 1)

echo [4/5] Creating installer package...
if exist "installer_64" rmdir /s /q installer_64
mkdir installer_64
mkdir installer_64\bin
mkdir installer_64\examples
mkdir installer_64\docs

REM Copy binaries
copy bin\displexitycompiler.exe installer_64\bin\
copy bin\disp_runtime.exe installer_64\bin\
copy bin\dispide.exe installer_64\bin\

REM Copy examples
copy examples\*.dis installer_64\examples\ 2>nul
copy examples\*.disp installer_64\examples\ 2>nul

REM Copy docs
copy README.md installer_64\docs\
copy LANGUAGE_SPEC.md installer_64\docs\

REM Create the installer executable script
echo [5/5] Creating installer script...

(
echo @echo off
echo title DISP Language Installer
echo color 1F
echo cls
echo.
echo     ====================================================
echo              DISP Programming Language Installer
echo                        64-bit Edition
echo     ====================================================
echo.
echo     This will install:
echo       - DISP Compiler ^(displexitycompiler.exe^)
echo       - DISP Runtime ^(disp_runtime.exe^)
echo       - DISP IDE ^(dispide.exe^)
echo       - Examples and Documentation
echo.
echo     Install location: C:\Program Files\DISP
echo.
echo     Press any key to continue or Ctrl+C to cancel...
echo pause ^>nul
echo.
echo net session ^>nul 2^>^&1
echo if %%errorlevel%% neq 0 ^(
echo     echo ERROR: Please run as Administrator!
echo     echo Right-click and select "Run as administrator"
echo     pause
echo     exit /b 1
echo ^)
echo.
echo echo Installing DISP Language...
echo echo.
echo.
echo echo [1/4] Creating directories...
echo if not exist "C:\Program Files\DISP" mkdir "C:\Program Files\DISP"
echo if not exist "C:\Program Files\DISP\bin" mkdir "C:\Program Files\DISP\bin"
echo if not exist "C:\Program Files\DISP\examples" mkdir "C:\Program Files\DISP\examples"
echo if not exist "C:\Program Files\DISP\docs" mkdir "C:\Program Files\DISP\docs"
echo.
echo echo [2/4] Copying files...
echo copy /Y "%%~dp0bin\*.*" "C:\Program Files\DISP\bin\"
echo copy /Y "%%~dp0examples\*.*" "C:\Program Files\DISP\examples\"
echo copy /Y "%%~dp0docs\*.*" "C:\Program Files\DISP\docs\"
echo.
echo echo [3/4] Adding to PATH...
echo setx PATH "%%PATH%%;C:\Program Files\DISP\bin" /M
echo.
echo echo [4/4] Creating shortcuts...
echo powershell "$ws = New-Object -ComObject WScript.Shell; $s = $ws.CreateShortcut('%%USERPROFILE%%\Desktop\DISP IDE.lnk'); $s.TargetPath = 'C:\Program Files\DISP\bin\dispide.exe'; $s.Save()"
echo.
echo echo.
echo echo ====================================================
echo echo   Installation Complete!
echo echo ====================================================
echo echo.
echo echo   You can now use:
echo echo     - dispide          ^(GUI IDE^)
echo echo     - displexitycompiler ^(Compiler^)
echo echo     - disp_runtime     ^(Runtime^)
echo echo.
echo echo   Desktop shortcut created: DISP IDE
echo echo.
echo echo   NOTE: You may need to restart your terminal for
echo echo         PATH changes to take effect.
echo echo.
echo pause
) > installer_64\disp_installer_64.bat

REM Create uninstaller
(
echo @echo off
echo title DISP Language Uninstaller
echo net session ^>nul 2^>^&1
echo if %%errorlevel%% neq 0 ^(
echo     echo Please run as Administrator!
echo     pause
echo     exit /b 1
echo ^)
echo echo Uninstalling DISP Language...
echo rmdir /s /q "C:\Program Files\DISP"
echo del "%%USERPROFILE%%\Desktop\DISP IDE.lnk" 2^>nul
echo echo DISP Language uninstalled.
echo pause
) > installer_64\uninstall.bat

REM Create ZIP
echo Creating ZIP package...
powershell Compress-Archive -Path installer_64\* -DestinationPath disp_installer_64.zip -Force

echo.
echo ============================================
echo   Installer created successfully!
echo ============================================
echo.
echo   Output: disp_installer_64.zip
echo.
echo   Contents:
echo     - disp_installer_64.bat (run as admin)
echo     - uninstall.bat
echo     - bin\ (executables)
echo     - examples\ (sample code)
echo     - docs\ (documentation)
echo.
pause
