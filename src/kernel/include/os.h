#ifndef MINIOS_H
#define MINIOS_H

#define OS_MAGIC 0x55aa

#define true 1

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

// 初始化内核
void kernel_init();

#endif