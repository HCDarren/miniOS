#ifndef MINIOS_ASM_INSTRUCT_H
#define MINIOS_ASM_INSTRUCT_H

#include <os.h>

// 汇编停机指令
void hlt();

// 读寄存器 cr4
u32_t read_cr4();

// 读寄存器 cr2
u32_t read_cr2();

// 写 cr4 寄存器
void write_cr4(u32_t v);

// 读寄存器 cr3
u32_t read_cr3();

// 设置 cr3 寄存器
// 参数是页目录的地址
void set_cr3(void* pde);

// 刷新虚拟地址 vaddr 的 块表 TLB
void flush_tlb(void* vaddr);

#endif