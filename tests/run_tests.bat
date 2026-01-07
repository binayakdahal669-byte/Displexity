@echo off
REM Displexity Test Suite Runner
REM Run all tests and report results

echo ========================================
echo    Displexity Test Suite v1.2.0
echo ========================================
echo.

set PASS=0
set FAIL=0
set DISP=..\bin\disp.exe

if not exist %DISP% (
    set DISP=..\release\bin\disp.exe
)

if not exist %DISP% (
    echo ERROR: Cannot find disp.exe compiler
    echo Please build the compiler first
    exit /b 1
)

echo Using compiler: %DISP%
echo.

REM Test 1: Basic features
echo [TEST 1] Basic Language Features...
%DISP% test_basic.dis -emit-exe -o test_basic.exe -no-log
if %ERRORLEVEL% EQU 0 (
    test_basic.exe > test_basic_output.txt 2>&1
    if %ERRORLEVEL% EQU 0 (
        echo   PASS: Basic features compiled and ran
        set /a PASS+=1
    ) else (
        echo   FAIL: Basic features runtime error
        set /a FAIL+=1
    )
) else (
    echo   FAIL: Basic features compilation error
    set /a FAIL+=1
)

REM Test 2: Control flow
echo [TEST 2] Control Flow...
%DISP% test_control_flow.dis -emit-exe -o test_control_flow.exe -no-log
if %ERRORLEVEL% EQU 0 (
    test_control_flow.exe > test_control_flow_output.txt 2>&1
    if %ERRORLEVEL% EQU 0 (
        echo   PASS: Control flow compiled and ran
        set /a PASS+=1
    ) else (
        echo   FAIL: Control flow runtime error
        set /a FAIL+=1
    )
) else (
    echo   FAIL: Control flow compilation error
    set /a FAIL+=1
)

REM Test 3: Functions
echo [TEST 3] Functions...
%DISP% test_functions.dis -emit-exe -o test_functions.exe -no-log
if %ERRORLEVEL% EQU 0 (
    test_functions.exe > test_functions_output.txt 2>&1
    if %ERRORLEVEL% EQU 0 (
        echo   PASS: Functions compiled and ran
        set /a PASS+=1
    ) else (
        echo   FAIL: Functions runtime error
        set /a FAIL+=1
    )
) else (
    echo   FAIL: Functions compilation error
    set /a FAIL+=1
)

REM Test 4: TUI bytecode generation
echo [TEST 4] TUI Bytecode Generation...
%DISP% test_tui.dis -emit-tui -o test_tui.tui -no-log
if %ERRORLEVEL% EQU 0 (
    echo   PASS: TUI bytecode generated
    set /a PASS+=1
) else (
    echo   FAIL: TUI bytecode generation error
    set /a FAIL+=1
)

REM Test 5: TUI bytecode execution
echo [TEST 5] TUI Bytecode Execution...
%DISP% run test_tui.tui > test_tui_output.txt 2>&1
if %ERRORLEVEL% EQU 0 (
    echo   PASS: TUI bytecode executed
    set /a PASS+=1
) else (
    echo   FAIL: TUI bytecode execution error
    set /a FAIL+=1
)

REM Test 6: DISP package creation
echo [TEST 6] DISP Package Creation...
%DISP% test_basic.dis -emit-disp -o test_basic.disp -no-log
if %ERRORLEVEL% EQU 0 (
    echo   PASS: DISP package created
    set /a PASS+=1
) else (
    echo   FAIL: DISP package creation error
    set /a FAIL+=1
)

REM Test 7: C output
echo [TEST 7] C Code Generation...
%DISP% test_basic.dis -emit-c -o test_basic_gen.c -no-log
if %ERRORLEVEL% EQU 0 (
    echo   PASS: C code generated
    set /a PASS+=1
) else (
    echo   FAIL: C code generation error
    set /a FAIL+=1
)

REM Test 8: Emit modes test program
echo [TEST 8] Emit Modes Test Program...
%DISP% test_emit_modes.dis -emit-exe -o test_emit_modes.exe -no-log
if %ERRORLEVEL% EQU 0 (
    test_emit_modes.exe > test_emit_modes_output.txt 2>&1
    if %ERRORLEVEL% EQU 0 (
        echo   PASS: Emit modes test compiled and ran
        set /a PASS+=1
    ) else (
        echo   FAIL: Emit modes test runtime error
        set /a FAIL+=1
    )
) else (
    echo   FAIL: Emit modes test compilation error
    set /a FAIL+=1
)

REM Test 9: C to Displexity transpiler
echo [TEST 9] C to Displexity Transpiler...
%DISP% transpile test_transpiler.c -o test_transpiler_out.dis -no-log 2>nul
if %ERRORLEVEL% EQU 0 (
    if exist test_transpiler_out.dis (
        echo   PASS: C transpiled to Displexity
        set /a PASS+=1
    ) else (
        echo   FAIL: Transpiler output not created
        set /a FAIL+=1
    )
) else (
    echo   FAIL: Transpiler error
    set /a FAIL+=1
)

REM Test 10: WebAssembly output
echo [TEST 10] WebAssembly Generation...
%DISP% test_basic.dis -emit-wasm -o test_basic.wat -no-log
if %ERRORLEVEL% EQU 0 (
    echo   PASS: WebAssembly generated
    set /a PASS+=1
) else (
    echo   FAIL: WebAssembly generation error
    set /a FAIL+=1
)

echo.
echo ========================================
echo    Test Results: %PASS% passed, %FAIL% failed
echo ========================================

REM Cleanup
del /q *.exe 2>nul
del /q *.tui 2>nul
del /q *.disp 2>nul
del /q *_gen.c 2>nul
del /q *_out.dis 2>nul
del /q *.wat 2>nul
del /q *_output.txt 2>nul

if %FAIL% GTR 0 (
    exit /b 1
)
exit /b 0
