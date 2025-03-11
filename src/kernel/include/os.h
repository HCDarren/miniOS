#ifndef MINIOS_H
#define MINIOS_H

#define OS_MAGIC 0x55aa
#define NULL 0
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

// 初始化内核
void kernel_init();

#endif