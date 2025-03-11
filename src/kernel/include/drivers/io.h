#ifndef MINIOS_IO_H
#define MINIOS_IO_H
#include <os.h>

// 读取一个字节
extern u8_t inb(u16_t port);
// 读取一个字
extern u16_t inw(u16_t port);
// 读取一个双字
extern u32_t indw(u16_t port);

// 输出一个字节
extern void outb(u16_t port, u8_t value);
// 输出一个字
extern void outw(u16_t port, u16_t value);
// 输出一个双字
extern void outdw(u16_t port, u32_t value);
#endif