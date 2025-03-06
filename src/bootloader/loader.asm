[org 0x1000]

dw 0x55aa

; 显示文本
mov si,loader_message
call print

detect_memory:
    xor ebx, ebx

    mov ax, 0
    mov es, ax
    mov edi, ards_buffer

    mov edx, 0x534d4150

    .next:
        mov eax, 0xe820
        mov ecx, 20
        int 0x15

        jc error
        add di, cx
        inc word [ards_count]
        cmp ebx, 0
        jnz .next

    mov si, detect_memory_message
    call print

prepare_protected_mode:
    cli

    in al, 0x92
    or al, 0b10
    out 0x92, al

    lgdt [gdt_ptr]
    
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp dword code_selector:enter_protected_mode

print:
    mov ah, 0x0e
    .next:
        mov al, [si]
        cmp al, 0
        jz .done
        int 0x10
        inc si
        jmp .next
    .done:
    ret

error:
    mov si,error_message
    call print
    hlt
    jmp $
ret

error_message:
    db "[miniOS] loader error.", 10, 13, 0 ;\r\n\0

detect_memory_message:
    db "[miniOS] detect memory success.", 10, 13, 0 ;\r\n\0

loader_message:
    db "[miniOS] loader loading success.", 10, 13, 0 ;\r\n\0

[bits 32]
enter_protected_mode:
    mov ax, data_selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x9fbff

    ; 读取内存存储的地方
    mov edi, 0x10000 
    ; 起始扇区
    mov ecx, 4
    ; 读取的扇区数量
    mov bl, 100
    call read_disk

    jmp dword code_selector:0x10000
    ud2

read_disk:
    ; 保存现场
    push ecx
    push ax
    push dx

    ; 0x1f2: 指定读取或写入的扇区数
    mov dx, 0x1f2
    mov al, bl
    out dx, al
    ; 0x1f3: lba地址的低8位
    inc dx
    mov al, cl
    out dx, al
    ; 0x1f4: lba地址的中8位
    inc dx
    shr ecx, 8
    mov al, cl
    out dx, al
    ; 0x1f5: lba地址的高8位
    inc dx
    shr ecx, 8
    mov al, cl
    out dx, al
    ; 0x1F6: 
    inc dx
    shr ecx, 8
    and cl, 0b1111
    mov al, 0b1110_0000
    or al, cl
    out dx, al
    ; 0x1F7:读取扇区:0x20 写入扇区:0x30
    inc dx
    mov al, 0x20
    out dx, al

    xor ecx, ecx
    mov cl, bl

    .read_sector:
        push cx
        call waits_disk
        call .read_a_sector
        pop cx
        loop .read_sector

    ;恢复现场
    pop dx
    pop ax
    pop ecx
ret 
    .read_a_sector:
        mov dx, 0x1f0
        mov cx, 256
        .readw:
            in ax, dx
            mov [edi], ax
            add edi, 2
            loop .readw
    ret

    ; 0x1F7:等待返回磁盘状态, 第 4 位为 1 表示硬盘准备好数据传输，第 7 位为 1 表示硬盘忙
    waits_disk:
        mov dx, 0x1f7
        .check:
            nop ;增加一个时钟周期, 同步读写
            nop
            in al, dx
            and al, 0b1000_1000
            cmp al, 0b0000_1000
            jnz .check
    ret

code_selector equ (1 << 3)
data_selector equ (2 << 3)

memory_base equ 0
memory_limit equ (1024 * 1024) - 1

gdt_ptr:
    dw (gdt_end - gdt_base) - 1
    dd gdt_base

gdt_base:
    dd 0, 0
gdt_code:
    dw memory_limit & 0xffff
    dw memory_base & 0xffff
    db (memory_base >> 16) & 0xff
    db 0b10011010
    db 0b11000000 | ((memory_limit >> 16) & 0xf)
    db (memory_base >> 24) & 0xff
gdt_data:
    dw memory_limit & 0xffff
    dw memory_base & 0xffff
    db (memory_base >> 16) & 0xff
    db 0b10010010
    db 0b11000000 | ((memory_limit >> 16) & 0xf)
    db (memory_base >> 24) & 0xff
gdt_end:

ards_count:
    db 0
ards_buffer: