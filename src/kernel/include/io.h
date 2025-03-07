#ifndef MINIOS_IO_H
#define MINIOS_IO_H
#include <os.h>

// 读取一个字节
extern u8 inb(u16 port);
// 读取一个字
extern u16 inw(u16 port);
// 读取一个双字
extern u32 indw(u16 port);

// 输出一个字节
extern void outb(u16 port, u8 value);
// 输出一个字
extern void outw(u16 port, u16 value);
// 输出一个双字
extern void outdw(u16 port, u32 value);
#endif