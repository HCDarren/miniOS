[bits 32]

extern kernel_init
global _start
_start:
    call kernel_init
    jmp $

global task_switch
task_switch:
    ; 保存当前进程的 abi 寄存器数据
    push ebp
    push ebx
    push esi
    push edi
    ; 后面抹掉 000 就能获取当前的进程
    mov eax, esp
    and eax, 0xfffff000
    ; 栈顶信息保存到当前位置
    mov [eax], esp

    ; 获取到 next 有栈平衡的情况下 +8 , 这里没有栈平衡所以
    mov eax, [esp + 20]
    ; 恢复下一个进程的 esp
    mov esp, [eax]
    ; 恢复下一个进程的 abi 寄存器数据
    pop edi
    pop esi
    pop ebx
    pop ebp
    ret

extern real_init_thread
extern alloc_a_page
user_code_selector equ (4 << 3 | 0b11)
user_data_selector equ (5 << 3 | 0b11)
; ss
; esp 
; eflags
; cs
; eip
global switch_to_user_mode
switch_to_user_mode:
    ;存用户栈位置的
    push user_data_selector
    ; 创建用户栈
    call alloc_a_page
    add eax, 0x1000
    push eax
    pushf
    push user_code_selector
    push real_init_thread
    mov ax, user_data_selector
    mov gs, ax
    mov fs, ax
    mov es, ax
    mov ds, ax
    iret

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

    push esp

    call do_interrupt_handler_%1

    add esp, 4;恢复栈平衡

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
INTERRUPT_HANDLER syscall