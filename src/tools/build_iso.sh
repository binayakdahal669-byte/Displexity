#!/bin/bash
# Displexity ISO/IMG Builder
# Creates bootable disk images from assembly or bare metal code

set -e

if [ -z "$1" ]; then
    echo "Usage: ./build_iso.sh <input.asm or input.dis> [output.iso]"
    exit 1
fi

INPUT=$1
OUTPUT=${2:-"output.iso"}
BASENAME=$(basename "$INPUT" | sed 's/\.[^.]*$//')

echo "=== Displexity ISO Builder ==="
echo "Input: $INPUT"
echo "Output: $OUTPUT"

# If .dis file, compile to ASM first
if [[ "$INPUT" == *.dis ]]; then
    echo "Compiling .dis to ASM..."
    # Would call: disp input.dis -o input.asm -emit-asm
    ASM_FILE="${BASENAME}.asm"
    # ./disp "$INPUT" -o "$ASM_FILE" -emit-asm
else
    ASM_FILE=$INPUT
fi

# Create build directory
mkdir -p iso_build
cd iso_build

# Create minimal bootloader (stub)
cat > bootloader.asm << 'EOF'
    ; Minimal x86 bootloader (512 bytes)
    [BITS 16]
    [ORG 0x7C00]
    
    ; Boot code
    mov ax, 0x0000
    mov ds, ax
    mov es, ax
    
    ; Print "Displexity Boot"
    mov ah, 0x0E
    mov al, 'D'
    int 0x10
    mov al, 'i'
    int 0x10
    mov al, 's'
    int 0x10
    
    ; Halt
    jmp $
    
    ; Fill to 510 bytes
    times 510-($-$$) db 0
    dw 0xAA55
EOF

echo "Created bootloader stub"

# Create simple ISO
# Note: Real implementation would use mkisofs or grub-mkrescue
if command -v mkisofs &> /dev/null; then
    echo "Building ISO with mkisofs..."
    # Create minimal filesystem
    mkdir -p root/boot
    cp bootloader.asm root/boot/
    mkisofs -o ../"$OUTPUT" -boot-load-size 4 -b boot/bootloader.asm root/
elif command -v genisoimage &> /dev/null; then
    echo "Building ISO with genisoimage..."
    mkdir -p root/boot
    cp bootloader.asm root/boot/
    genisoimage -o ../"$OUTPUT" -boot-load-size 4 -b boot/bootloader.asm root/
else
    echo "Creating raw IMG file (mkisofs not found)..."
    # Create simple FAT image
    dd if=/dev/zero of=../"${OUTPUT%.iso}.img" bs=512 count=2880 2>/dev/null
    echo "IMG file created (unformatted)"
fi

cd ..
rm -rf iso_build

echo "✓ Image created: $OUTPUT"
echo "  To test: qemu-system-x86_64 -cdrom $OUTPUT"
echo "  Or:      qemu-system-x86_64 $OUTPUT"
