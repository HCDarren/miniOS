#ifndef MINIOS_ASM_INSTRUCT_H
#define MINIOS_ASM_INSTRUCT_H

#include <os.h>

// 汇编停机指令
void hlt();

// 读寄存器 cr4
u32_t read_cr4();

// 写 cr4 寄存器
void write_cr4(u32_t v);

#endif