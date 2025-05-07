#ifndef MINIOS_MIO_H
#define MINIOS_MIO_H

#include <os.h>

// 内存输入字节 8bit
u8_t minb(u32_t addr);
// 内存输入字 16bit
u16_t minw(u32_t addr);
// 内存输入双字 32bit
u32_t mindw(u32_t addr);

// 内存输出字节
void moutb(u32_t addr, u8_t value);
// 内存输出字
void moutw(u32_t addr, u16_t value);
// 内存输出双字
void moutdw(u32_t addr, u32_t value);

#endif