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

; 读取内存存储的地方
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
    db "[miniOS] load loader error.", 10, 13, 0 ;\r\n\0

booting:
    db "[miniOS] enter booting.", 10, 13, 0 ;\r\n\0

; 填充 0
times 510 - ($ - $$) db 0

; 主引导扇区 512 字节，后面两个字节必须是 0xaa55
db 0x55, 0xaa