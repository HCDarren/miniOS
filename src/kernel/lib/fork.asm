[bits 32]

global fork
fork:
    mov eax, 1
    int 0x80
    ret