#ifndef MINIOS_GDT_H
#define MINIOS_GDT_H
#include <os.h>

#define GDT_TABLE_SIZE 128

#define KERNEL_CODE_INDEX 1
#define KERNEL_DATA_INDEX 2
#define KERNEL_TSS_INDEX 3
#define USER_CODE_INDEX 4
#define USER_DATA_INDEX 5

// 内核代码段的选择子
#define KERNEL_CODE_SELECTOR (KERNEL_CODE_INDEX << 3)
// 内核数据段的选择子
#define KERNEL_DATA_SELECTOR (KERNEL_DATA_INDEX << 3)
// 内核 tss 段选择子
#define KERNEL_TSS_SELECTOR (KERNEL_TSS_INDEX << 3)
// 用户代码段的选择子
#define USER_CODE_SELECTOR (USER_CODE_INDEX << 3 | 0b11)
// 用户数据段的选择子
#define USER_DATA_SELECTOR (USER_DATA_INDEX << 3 | 0b11)

enum DLP {
    DPL_0 = 0,
    DPL_1,
    DPL_2,
    DPL_3
};

// 全局描述符
typedef struct gdt_descriptor
{
    // 段界限 0 ~ 15 位
    unsigned short limit_low;
    // 基地址 0 ~ 23 位 16M
    unsigned int base_low : 24;
    // 段类型
    unsigned char type : 4;
    // 1 表示代码段或数据段，0 表示系统段
    unsigned char segment_type : 1;
    // Descriptor Privilege Level 描述符特权等级 0 ~ 3
    unsigned char DPL : 2;
    // 存在位，1 在内存中，0 在磁盘上
    unsigned char present : 1;
    // 段界限 16 ~ 19;
    unsigned char limit_high : 4;
    // 给操作系统留的，我们来用
    unsigned char available : 1;
    // 64 位扩展标志
    unsigned char long_mode : 1;
    // 32 位 还是 16 位;
    unsigned char big : 1;
    // 粒度 4KB 或 1B
    unsigned char granularity : 1;
    // 基地址 24 ~ 31 位
    unsigned char base_high;
} __packed gdt_descriptor_t;

typedef struct task_state_segment {
    // 上一个任务的指针
    u32_t previous_task_link;
    // 三个不同优先级的栈
    u32_t esp0;
    u32_t ss0;
    u32_t esp1;
    u32_t ss1;
    u32_t esp2;
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

// 段选择子
typedef struct gdt_selector
{
    // 请求特权级别
    u8_t RPL : 2;
    // Table Indicator
    u8_t TI : 1;
    // 在描述符表的什么位置
    u16_t index : 13;
} __packed selector_t;

// 全局描述符表指针
typedef struct gdt_descriptor_pointer
{
    // 界限
    u16_t limit;
    // 内存开始位置
    u32_t base;
} __packed gdt_descriptor_pointer_t;

// 重新初始化 gdt 表
void gdt_init();

// 释放描述符号
void free_descriptor(int selector);

#endif