#include <base/asm_instruct.h>

// 汇编停机指令
void hlt() {
    asm volatile("hlt\n");
}

// 读寄存器 cr4
u32_t read_cr4() {
    u32_t cr4;
    __asm__ __volatile__("mov %%cr4, %[v]":[v]"=r"(cr4));
    return cr4;
}

// 写 cr4 寄存器
void write_cr4(u32_t v) {
    __asm__ __volatile__("mov %[v], %%cr4"::[v]"r"(v));
}
