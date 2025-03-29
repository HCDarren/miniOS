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

// 读寄存器 cr2
u32_t read_cr2() {
    u32_t cr2;
    __asm__ __volatile__("mov %%cr2, %[v]":[v]"=r"(cr2));
    return cr2;
}

// 读寄存器 cr3
u32_t read_cr3() {
    u32_t cr3;
    __asm__ __volatile__("mov %%cr3, %[v]":[v]"=r"(cr3));
    return cr3;
}

// 设置 cr3 寄存器
// 参数是页目录的地址
void set_cr3(void* pde)
{
    asm volatile("movl %%eax, %%cr3\n" ::"a"(pde));
}

// 刷新虚拟地址 vaddr 的 块表 TLB
void flush_tlb(void* vaddr)
{
    asm volatile("invlpg (%0)" ::"r"(vaddr)
                 : "memory");
}