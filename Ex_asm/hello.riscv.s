.section .text
.globl main
.align 2

.section .data
str_displexity:
    .string "Displexity RISC-V"

.section .text
main:
    addi sp, sp, -16        # Allocate 16 bytes on stack
    sd ra, 8(sp)            # Save return address
    sd s0, 0(sp)            # Save frame pointer
    addi s0, sp, 16         # Set frame pointer
    
    la a0, str_displexity   # Load address of string
    jal ra, printf          # Call printf
    
    li a0, 0                # Return 0
    ld s0, 0(sp)            # Restore frame pointer
    ld ra, 8(sp)            # Restore return address
    addi sp, sp, 16         # Deallocate stack
    ret                     # Return to caller

# === RISC-V RV64I ABI Notes ===
# Argument passing: a0-a7
# Return value: a0
# Caller-saved: a0-a7, t0-t6
# Callee-saved: s0-s11, sp
# Stack must be 16-byte aligned
# Return address: ra (x1)
