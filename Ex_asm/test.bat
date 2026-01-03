@echo off
REM Test suite for Displexity compiler (Windows)

setlocal enabledelayedexpansion

set COMPILER=build\disp.exe
set CC=gcc

set test_count=0
set pass_count=0
set fail_count=0

echo === Displexity Test Suite (Windows) ===
echo.

REM Test 1: Hello World
echo Test 1: hello...
%COMPILER% examples\hello.dis -o build\test_hello.c >nul 2>&1
if !errorlevel! equ 0 (
    %CC% build\test_hello.c -o build\test_hello.exe >nul 2>&1
    if !errorlevel! equ 0 (
        for /f "delims=" %%I in ('build\test_hello.exe') do set output=%%I
        if "!output!"=="Hello, Displexity!" (
            echo PASS
            set /a pass_count+=1
        ) else (
            echo FAIL ^(output: !output!^)
            set /a fail_count+=1
        )
    ) else (
        echo FAIL ^(linking error^)
        set /a fail_count+=1
    )
) else (
    echo FAIL ^(compilation error^)
    set /a fail_count+=1
)
set /a test_count+=1

REM Test 2: Conditional
echo Test 2: conditional...
%COMPILER% examples\conditional.dis -o build\test_cond.c >nul 2>&1
if !errorlevel! equ 0 (
    %CC% build\test_cond.c -o build\test_cond.exe >nul 2>&1
    if !errorlevel! equ 0 (
        for /f "delims=" %%I in ('build\test_cond.exe') do set output=%%I
        if "!output!"=="Large" (
            echo PASS
            set /a pass_count+=1
        ) else (
            echo FAIL ^(output: !output!^)
            set /a fail_count+=1
        )
    ) else (
        echo FAIL ^(linking error^)
        set /a fail_count+=1
    )
) else (
    echo FAIL ^(compilation error^)
    set /a fail_count+=1
)
set /a test_count+=1

echo.
echo Summary:
echo   Total: !test_count!
echo   Passed: !pass_count!
echo   Failed: !fail_count!
echo.

if !fail_count! equ 0 (
    echo All tests passed!
    exit /b 0
) else (
    echo Some tests failed.
    exit /b 1
)
