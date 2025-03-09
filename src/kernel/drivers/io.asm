global inb ; 符号 inb 导出
inb:
    ; 保存栈帧
    push ebp 
    mov ebp, esp
    ; 将 eax 清空
    xor eax, eax 
    mov edx, [ebp + 8]
    ; 读端口数据
    in al, dx
    ; 恢复栈帧
    leave 
    ret

global outb
outb:
    ; 保存栈帧
    push ebp 
    mov ebp, esp
    ; 读参数 port，value
    mov edx, [ebp + 8] 
    mov eax, [ebp + 12]
    ; 读写有端口有规则，不能乱用寄存器
    out dx, al
    ; 恢复栈帧
    leave 
    ret

global inw
inw:
    ; 保存栈帧
    push ebp
    mov ebp, esp
    ; 将 eax 清空
    xor eax, eax 
    mov edx, [ebp + 8]
    ; 读端口数据
    in ax, dx
    ; 恢复栈帧
    leave 
    ret

global outw
outw:
    ; 保存栈帧
    push ebp 
    mov ebp, esp
    ; 读参数 port，value
    mov edx, [ebp + 8] 
    mov eax, [ebp + 12]
    ; 读写有端口有规则，不能乱用寄存器
    out dx, ax
    ; 恢复栈帧
    leave 
    ret

global indw
indw:
    ; 保存栈帧
    push ebp
    mov ebp, esp
    ; 将 eax 清空
    xor eax, eax 
    mov edx, [ebp + 8]
    ; 读端口数据
    in eax, dx
    ; 恢复栈帧
    leave 
    ret

global outdw
outdw:
    ; 保存栈帧
    push ebp 
    mov ebp, esp
    ; 读参数 port，value
    mov edx, [ebp + 8] 
    mov eax, [ebp + 12]
    ; 读写有端口有规则，不能乱用寄存器
    out dx, eax
    ; 恢复栈帧
    leave 
    ret