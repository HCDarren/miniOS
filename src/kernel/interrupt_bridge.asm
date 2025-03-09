[bits 32]

extern interrupt_handler

global interrupt_bridge
interrupt_bridge:
    ; 保存上文寄存器信息
    push ds
    push es
    push fs
    push gs
    pusha

    call interrupt_handler

    ; 恢复下文寄存器信息
    popa
    pop gs
    pop fs
    pop es
    pop ds
    
    iret