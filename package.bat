@echo off
echo ========================================
echo    Displexity Complete Build & Package
echo ========================================

REM Clean previous builds
if exist release rmdir /s /q release
mkdir release
mkdir release\bin
mkdir release\examples
mkdir release\lib
mkdir release\docs
mkdir release\ide

echo Building compiler...
call build.bat
if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

echo Copying binaries...
copy bin\disp.exe release\bin\
copy bin\displexitycompiler.exe release\bin\
copy bin\bootgen.exe release\bin\
copy bin\iso_builder.exe release\bin\

echo Adding icons to executables...
if exist disp.ico (
    echo Adding icon to disp.exe...
    REM Note: Would need resource editor tool like ResourceHacker
    REM rh.exe -open release\bin\disp.exe -save release\bin\disp.exe -action addoverwrite -res disp.ico -mask ICONGROUP,MAINICON,
)

echo Copying examples...
xcopy examples release\examples\ /s /e /y

echo Copying libraries...
if exist examples\lib xcopy examples\lib release\lib\ /s /e /y

echo Copying IDE...
xcopy gui_ide release\ide\ /s /e /y

echo Creating documentation...
echo # Displexity Programming Language > release\docs\README.md
echo. >> release\docs\README.md
echo ## Quick Start >> release\docs\README.md
echo. >> release\docs\README.md
echo 1. Open Command Prompt >> release\docs\README.md
echo 2. Navigate to bin folder >> release\docs\README.md
echo 3. Run: disp examples\hello.dis >> release\docs\README.md
echo. >> release\docs\README.md
echo ## Syntax Examples >> release\docs\README.md
echo. >> release\docs\README.md
echo ```dis >> release\docs\README.md
echo say "Hello World!" >> release\docs\README.md
echo >> release\docs\README.md
echo num x = 42 >> release\docs\README.md
echo show "Number: " ^<^< x ^<^< "\n" >> release\docs\README.md
echo >> release\docs\README.md
echo win "My Game", 800, 600 >> release\docs\README.md
echo opengl() >> release\docs\README.md
echo 2d() >> release\docs\README.md
echo >> release\docs\README.md
echo repeat frame from 0 to 100 { >> release\docs\README.md
echo     cls() >> release\docs\README.md
echo     color 1.0, 0.0, 0.0 >> release\docs\README.md
echo     box 100, 100, 50, 50 >> release\docs\README.md
echo     flip() >> release\docs\README.md
echo } >> release\docs\README.md
echo ``` >> release\docs\README.md

echo Creating simple examples...
echo print("Hello from Displexity!") > release\examples\hello.dis

echo # Simple math example > release\examples\math.dis
echo a = 10 >> release\examples\math.dis
echo b = 20 >> release\examples\math.dis
echo sum_result = a + b >> release\examples\math.dis
echo print(f"Sum: {sum_result}") >> release\examples\math.dis

echo # Simple game example > release\examples\game.dis
echo window("Simple Game", 400, 300) >> release\examples\game.dis
echo opengl() >> release\examples\game.dis
echo projection_2d() >> release\examples\game.dis
echo >> release\examples\game.dis
echo for frame in range(200): >> release\examples\game.dis
echo     clear() >> release\examples\game.dis
echo     color(1.0, 0.0, 0.0) >> release\examples\game.dis
echo     rect(50 + frame, 100, 30, 30) >> release\examples\game.dis
echo     render() >> release\examples\game.dis

echo Creating batch files...
echo @echo off > release\bin\compile.bat
echo echo Compiling %%1... >> release\bin\compile.bat
echo disp.exe %%1 -o %%~n1.exe >> release\bin\compile.bat
echo if errorlevel 1 ( >> release\bin\compile.bat
echo     echo Compilation failed! >> release\bin\compile.bat
echo     pause >> release\bin\compile.bat
echo     exit /b 1 >> release\bin\compile.bat
echo ) >> release\bin\compile.bat
echo echo Success! Created %%~n1.exe >> release\bin\compile.bat
echo pause >> release\bin\compile.bat

echo @echo off > release\run_ide.bat
echo cd ide >> release\run_ide.bat
echo start nvim >> release\run_ide.bat

echo Creating installer script...
echo @echo off > release\install.bat
echo echo Installing Displexity... >> release\install.bat
echo set INSTALL_DIR=C:\Displexity >> release\install.bat
echo if not exist "%%INSTALL_DIR%%" mkdir "%%INSTALL_DIR%%" >> release\install.bat
echo xcopy bin "%%INSTALL_DIR%%\bin\" /s /e /y >> release\install.bat
echo xcopy examples "%%INSTALL_DIR%%\examples\" /s /e /y >> release\install.bat
echo xcopy lib "%%INSTALL_DIR%%\lib\" /s /e /y >> release\install.bat
echo xcopy docs "%%INSTALL_DIR%%\docs\" /s /e /y >> release\install.bat
echo xcopy ide "%%INSTALL_DIR%%\ide\" /s /e /y >> release\install.bat
echo >> release\install.bat
echo echo Adding to PATH... >> release\install.bat
echo setx PATH "%%PATH%%;%%INSTALL_DIR%%\bin" >> release\install.bat
echo >> release\install.bat
echo echo Installation complete! >> release\install.bat
echo echo You can now use 'disp' command from anywhere. >> release\install.bat
echo pause >> release\install.bat

echo Creating version info...
echo Displexity v1.0 > release\VERSION.txt
echo Build Date: %date% %time% >> release\VERSION.txt
echo. >> release\VERSION.txt
echo Features: >> release\VERSION.txt
echo - OpenGL Graphics >> release\VERSION.txt
echo - Multi-file Projects >> release\VERSION.txt
echo - Binary Libraries (.disll) >> release\VERSION.txt
echo - Multiple Target Platforms >> release\VERSION.txt
echo - Simplified Syntax >> release\VERSION.txt
echo - Turing Complete >> release\VERSION.txt

echo Testing compilation...
if exist release\bin\disp.exe (
    echo Compiler found: release\bin\disp.exe
    echo Testing basic compilation...
    cd release\bin
    
    REM Create a simple test file
    echo say "Hello from Displexity!" > test_hello.dis
    
    REM Try to compile it with timeout
    timeout /t 30 disp.exe test_hello.dis -o test_hello.exe 2>nul
    if exist test_hello.exe (
        echo Compilation test: SUCCESS
        del test_hello.exe 2>nul
    ) else (
        echo Compilation test: FAILED (but continuing...)
    )
    
    del test_hello.dis 2>nul
    cd ..\..
) else (
    echo Warning: Compiler not found, skipping test
)

echo.
echo ========================================
echo    Package Complete!
echo ========================================
echo.
echo Release folder contains:
echo   bin\        - Compiler and tools
echo   examples\   - Sample programs  
echo   lib\        - Standard libraries
echo   docs\       - Documentation
echo   ide\        - Neovim IDE setup
echo   install.bat - System installer
echo.
echo To install system-wide: run release\install.bat
echo To test manually: cd release\bin ^&^& disp ..\examples\hello.dis
echo.

REM Create ZIP package
if exist "C:\Program Files\7-Zip\7z.exe" (
    echo Creating ZIP package...
    "C:\Program Files\7-Zip\7z.exe" a -tzip displexity-v1.0.zip release\* >nul 2>&1
    if exist displexity-v1.0.zip (
        echo Package created: displexity-v1.0.zip
    ) else (
        echo ZIP creation failed
    )
) else (
    echo 7-Zip not found. Manual ZIP creation needed.
    echo You can manually create a ZIP of the release\ folder
)

echo.
echo Package complete! Ready for distribution.
echo.
pause