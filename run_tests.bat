@echo off
echo Running DISP Compiler Test Suite...
echo ====================================

if not exist "bin\dispcompiler.exe" (
    echo Building compiler first...
    call build.bat
    if %errorlevel% neq 0 (
        echo Build failed!
        pause
        exit /b 1
    )
)

mkdir test_results 2>nul

echo.
echo Test 1: Basic Hello World
echo // Hello World Test > test_results\hello_test.disp
echo function main() { >> test_results\hello_test.disp
echo     Array[] msg = "Hello, DISP!"; >> test_results\hello_test.disp
echo     print(msg); >> test_results\hello_test.disp
echo     return 0; >> test_results\hello_test.disp
echo } >> test_results\hello_test.disp

bin\dispcompiler.exe test_results\hello_test.disp -o test_results\hello_test.dis
if %errorlevel% equ 0 (
    echo [PASS] Hello World compilation
) else (
    echo [FAIL] Hello World compilation
)

echo.
echo Test 2: Array Operations
echo // Array Test > test_results\array_test.disp
echo function main() { >> test_results\array_test.disp
echo     Array[] word = "test"; >> test_results\array_test.disp
echo     print(word[0]); >> test_results\array_test.disp
echo     Array[] numbers = [1, 2, 3]; >> test_results\array_test.disp
echo     print(numbers[1]); >> test_results\array_test.disp
echo     return 0; >> test_results\array_test.disp
echo } >> test_results\array_test.disp

bin\dispcompiler.exe test_results\array_test.disp -o test_results\array_test.dis
if %errorlevel% equ 0 (
    echo [PASS] Array operations
) else (
    echo [FAIL] Array operations
)

echo.
echo Test 3: Functions
echo // Function Test > test_results\function_test.disp
echo function add(a, b) { >> test_results\function_test.disp
echo     return a + b; >> test_results\function_test.disp
echo } >> test_results\function_test.disp
echo function main() { >> test_results\function_test.disp
echo     result = add(5, 3); >> test_results\function_test.disp
echo     print(result); >> test_results\function_test.disp
echo     return 0; >> test_results\function_test.disp
echo } >> test_results\function_test.disp

bin\dispcompiler.exe test_results\function_test.disp -o test_results\function_test.dis
if %errorlevel% equ 0 (
    echo [PASS] Function declarations
) else (
    echo [FAIL] Function declarations
)

echo.
echo Test 4: Classes
echo // Class Test > test_results\class_test.disp
echo class Math { >> test_results\class_test.disp
echo     function multiply(x, y) { >> test_results\class_test.disp
echo         return x * y; >> test_results\class_test.disp
echo     } >> test_results\class_test.disp
echo } >> test_results\class_test.disp
echo function main() { >> test_results\class_test.disp
echo     result = Math.multiply(4, 5); >> test_results\class_test.disp
echo     print(result); >> test_results\class_test.disp
echo     return 0; >> test_results\class_test.disp
echo } >> test_results\class_test.disp

bin\dispcompiler.exe test_results\class_test.disp -o test_results\class_test.dis
if %errorlevel% equ 0 (
    echo [PASS] Class declarations
) else (
    echo [FAIL] Class declarations
)

echo.
echo Test 5: Control Flow
echo // Control Flow Test > test_results\control_test.disp
echo function main() { >> test_results\control_test.disp
echo     x = 10; >> test_results\control_test.disp
echo     if (x ^> 5) { >> test_results\control_test.disp
echo         print("Greater than 5"); >> test_results\control_test.disp
echo     } else { >> test_results\control_test.disp
echo         print("Less or equal to 5"); >> test_results\control_test.disp
echo     } >> test_results\control_test.disp
echo     i = 0; >> test_results\control_test.disp
echo     while (i ^< 3) { >> test_results\control_test.disp
echo         print(i); >> test_results\control_test.disp
echo         i = i + 1; >> test_results\control_test.disp
echo     } >> test_results\control_test.disp
echo     return 0; >> test_results\control_test.disp
echo } >> test_results\control_test.disp

bin\dispcompiler.exe test_results\control_test.disp -o test_results\control_test.dis
if %errorlevel% equ 0 (
    echo [PASS] Control flow statements
) else (
    echo [FAIL] Control flow statements
)

echo.
echo Test 6: HTTP Requests
echo // HTTP Test > test_results\http_test.disp
echo function main() { >> test_results\http_test.disp
echo     http "https://httpbin.org/get" "GET" response; >> test_results\http_test.disp
echo     print(response); >> test_results\http_test.disp
echo     return 0; >> test_results\http_test.disp
echo } >> test_results\http_test.disp

bin\dispcompiler.exe test_results\http_test.disp -o test_results\http_test.dis
if %errorlevel% equ 0 (
    echo [PASS] HTTP requests
) else (
    echo [FAIL] HTTP requests
)

echo.
echo Test 7: Assembly Generation
bin\dispcompiler.exe test_results\hello_test.disp -asm -o test_results\hello_test.s
if %errorlevel% equ 0 (
    echo [PASS] Assembly generation
) else (
    echo [FAIL] Assembly generation
)

echo.
echo Test 8: Direct Assembly Mode
bin\dispcompiler.exe test_results\hello_test.disp -direct -o test_results\hello_direct.s
if %errorlevel% equ 0 (
    echo [PASS] Direct assembly mode
) else (
    echo [FAIL] Direct assembly mode
)

echo.
echo Test 9: BIOS Mode
bin\dispcompiler.exe test_results\hello_test.disp -bios -o test_results\hello_bios.bin
if %errorlevel% equ 0 (
    echo [PASS] BIOS compilation mode
) else (
    echo [FAIL] BIOS compilation mode
)

echo.
echo Test 10: Graphics Mode
echo // Graphics Test > test_results\graphics_test.disp
echo function main() { >> test_results\graphics_test.disp
echo     clear(); >> test_results\graphics_test.disp
echo     draw_pixel(); >> test_results\graphics_test.disp
echo     return 0; >> test_results\graphics_test.disp
echo } >> test_results\graphics_test.disp

bin\dispcompiler.exe test_results\graphics_test.disp -graphics -o test_results\graphics_test.dis
if %errorlevel% equ 0 (
    echo [PASS] Graphics compilation
) else (
    echo [FAIL] Graphics compilation
)

echo.
echo Test 11: Cross-Platform Compilation
bin\dispcompiler.exe test_results\hello_test.disp -target linux -o test_results\hello_linux.dis
if %errorlevel% equ 0 (
    echo [PASS] Linux target compilation
) else (
    echo [FAIL] Linux target compilation
)

bin\dispcompiler.exe test_results\hello_test.disp -target win -o test_results\hello_win.exe
if %errorlevel% equ 0 (
    echo [PASS] Windows target compilation
) else (
    echo [FAIL] Windows target compilation
)

bin\dispcompiler.exe test_results\hello_test.disp -target mac -o test_results\hello_mac.app
if %errorlevel% equ 0 (
    echo [PASS] macOS target compilation
) else (
    echo [FAIL] macOS target compilation
)

echo.
echo Test 12: Syntax Tests
bin\dispcompiler.exe -test
if %errorlevel% equ 0 (
    echo [PASS] Built-in syntax tests
) else (
    echo [FAIL] Built-in syntax tests
)

echo.
echo ====================================
echo Test Suite Complete!
echo Check test_results\ directory for generated files
echo Check log files for detailed compilation information
echo.
pause