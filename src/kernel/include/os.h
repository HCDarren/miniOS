#ifndef MINIOS_H
#define MINIOS_H

#define OS_MAGIC 0x55aa
#define NULL 0
#define nullptr 0
#define EOF -1
// 用于定义特殊的结构体
#define __packed __attribute__((packed))

#define BMB asm volatile("xchgw %bx, %bx") // bochs magic breakpoint

typedef int size_t;

typedef _Bool bool;
#define true 1
#define false 0

typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned int u32_t;
typedef unsigned long long u64_t;

typedef void* sys_call_method_t;

typedef u32_t pid_t;

typedef u32_t fd_t; 

// 文件句柄描述符
typedef u32_t fd_t;
// 三个基本输入输出描述符
typedef enum std_fd_t
{
    stdin = 0, // 标准输入 0 
    stdout, // 标准输出 1
    stderr, // 标准错误 2
} std_fd_t;

// 初始化内核
void kernel_init();

#endif