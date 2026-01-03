@echo off
REM Displexity ISO/IMG Builder for Windows
REM Creates bootable disk images

setlocal enabledelayedexpansion

if "%1"=="" (
    echo Usage: build_iso.bat ^<input.asm or input.dis^> [output.iso]
    exit /b 1
)

set INPUT=%1
set OUTPUT=%2
if "%OUTPUT%"=="" set OUTPUT=output.iso

echo === Displexity ISO Builder ===
echo Input: %INPUT%
echo Output: %OUTPUT%

REM If .dis file, compile to ASM first
if "%INPUT:~-4%"==".dis" (
    echo Compiling .dis to ASM...
    REM Would call: disp.exe input.dis -o input.asm -emit-asm
)

REM Create build directory
if not exist iso_build mkdir iso_build
cd iso_build

REM Create minimal bootloader stub
(
    echo ; Minimal x86 bootloader
    echo [BITS 16]
    echo [ORG 0x7C00]
    echo.
    echo mov ax, 0x0000
    echo mov ds, ax
    echo mov es, ax
    echo.
    echo ; Print "Displexity"
    echo mov ah, 0x0E
    echo mov al, 'D'
    echo int 0x10
    echo.
    echo jmp $
    echo times 510-($-$$) db 0
    echo dw 0xAA55
) > bootloader.asm

echo Created bootloader stub

cd ..

echo.
echo ISO/IMG creation requires external tools (mkisofs, grub-mkrescue, or VirtualBox)
echo To complete: 
echo   1. Use VirtualBox: File ^> New ^> Expert Mode
echo   2. Or use: mkisofs -o output.iso -b bootloader.asm .
echo   3. Test with: qemu-system-x86_64 -cdrom output.iso
echo.
echo Note: For complete bootable image, assemble with NASM and create filesystem
echo   nasm -f bin bootloader.asm -o bootloader.bin
echo   dd if=bootloader.bin of=disk.img
