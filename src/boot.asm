[org 0x7c00]

; 清屏
mov ax, 3
int 0x10

; 初始化段寄存器
mov ax, 0
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7c00

; 显示文本
mov si,booting
call print

; 读到内存的什么位置？
mov edi, 0x1000 
; 起始扇区
mov ecx, 2
; 读取的扇区数量
mov bl, 4
call read_disk

cmp word [0x1000], 0x55aa
jnz error

jmp 0:0x1002

; 阻塞
jmp $

error:
    mov si,error_message
    call print
    hlt
    jmp $
ret

read_disk:
    mov dx, 0x1f2
    mov al, bl
    out dx, al

    inc dx
    mov al, cl
    out dx, al

    inc dx
    shr ecx, 8
    mov al, cl
    out dx, al

    inc dx
    shr ecx, 8
    mov al, cl
    out dx, al

    inc dx
    shr ecx, 8
    and cl, 0b1111
    mov al, 0b1110_0000
    or al, cl
    out dx, al

    inc dx
    mov al, 0x20
    out dx, al

    xor ecx, ecx
    mov cl, bl

    .read:
        push cx
        call waits_disk
        call .reads
        pop cx
        loop .read
ret 
    .reads:
        mov dx, 0x1f0
        mov cx, 256
        .readw:
            in ax, dx
            mov [edi], ax
            add edi, 2
            loop .readw
    ret

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

error_message:
    db "[miniOS] read loader error ...", 10, 13, 0 ;\r\n\0

booting:
    db "[miniOS] enter booting ...", 10, 13, 0 ;\r\n\0

; 填充 0
times 510 - ($ - $$) db 0

; 主引导扇区 512 字节，后面两个字节必须是 0xaa55
db 0x55, 0xaa