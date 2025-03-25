#ifndef MINIOS_SYS_CALL_H
#define MINIOS_SYS_CALL_H

#include <os.h>

// 系统调用号 80
#define INTERRUPT_SYS_CALL_NUMBER 0x80

typedef enum syscall_t
{
    sys_write = 1,
    sys_fork,
    sys_getpid,
    sys_getppid
};

void syscall_init();

// 三个参数的系统调用
u32_t syscall_3(u32_t nr, u32_t arg1, u32_t arg2, u32_t arg3);

// 两个参数的系统调用
u32_t syscall_2(u32_t nr, u32_t arg1, u32_t arg2);

// 一个参数的系统调用
u32_t syscall_1(u32_t nr, u32_t arg1);

// 0个参数的系统调用
u32_t syscall_0(u32_t nr);

#endif