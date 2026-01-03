@echo off
echo ========================================
echo    Displexity Simple Package
echo ========================================

REM Clean and create release directory
if exist release rmdir /s /q release
mkdir release
mkdir release\bin
mkdir release\examples
mkdir release\docs

echo Building compiler...
call build.bat
if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

echo Copying files...
copy bin\disp.exe release\bin\
copy examples\*.dis release\examples\
copy docs\*.md release\docs\

echo Creating simple test...
echo print("Hello from Displexity!") > release\examples\hello.dis

echo.
echo ========================================
echo    Package Complete!
echo ========================================
echo.
echo Files ready in release\ folder
echo Test with: release\bin\disp.exe release\examples\hello.dis
echo.
pause