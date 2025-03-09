[bits 32]

extern kernel_init
global _start
_start:
    call kernel_init
    jmp $

%macro INTERRUPT_HANDLER 1
extern do_interrupt_handler_%1
global interrupt_handler_%1
interrupt_handler_%1:
    ; 保存上文寄存器信息
    push ds
    push es
    push fs
    push gs
    pusha

    call do_interrupt_handler_%1

    ; 恢复下文寄存器信息
    popa
    pop gs
    pop fs
    pop es
    pop ds

    iret
%endmacro

INTERRUPT_HANDLER defalut
INTERRUPT_HANDLER time