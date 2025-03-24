#ifndef MINIOS_SYS_CALL_H
#define MINIOS_SYS_CALL_H

#include <os.h>

// 系统调用号 80
#define INTERRUPT_SYS_CALL_NUMBER 0x80

typedef enum syscall_t
{
    SYS_NR_WRITE = 4,
};

void syscall_init();

// 三个参数的系统调用
u32_t syscall3(u32_t nr, u32_t arg1, u32_t arg2, u32_t arg3);

#endif