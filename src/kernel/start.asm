[bits 32]

extern kernel_init

global _start
_start:
    xchg bx, bx
    call kernel_init
    xchg bx, bx
    jmp $