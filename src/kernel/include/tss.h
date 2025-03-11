#ifndef MINIOS_TSS_H
#define MINIOS_TSS_H
#include <os.h>
typedef struct task_state_segment {
    // 上一个任务的指针
    u32_t previous_task_link;
    // 三个不同优先级的栈
    u32_t* esp0;
    u32_t ss0;
    u32_t* esp1;
    u32_t ss1;
    u32_t* esp2;
    u32_t ss2;
    // 跟虚拟内存有关，指向页表
    u32_t cr3;
    // 当前任务运行的代码位置
    u32_t* eip;
    // 标识位寄存器
    u32_t eflags;
    // 通用寄存器
    u32_t eax, ecx, edx, ebx;
    u32_t* esp;
    u32_t ebp, esi, edi;
    // 段寄存器
    u32_t es, cs, ss ,ds, fs, gs;
    // ldt 表
    u32_t ldt;
    // io 位图
    u32_t io_base;
    // 任务影子栈指针，用不上
    u32_t ssp;

} __packed tss_t;

#endif