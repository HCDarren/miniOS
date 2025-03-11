#ifndef MINIOS_GDT_H
#define MINIOS_GDT_H
#include <os.h>

#define GDT_TABLE_SIZE 128

// 内核数据段的选择子
#define KERNEL_CODE_SELECTOR (1 << 3)
// 内核代码段的选择子
#define KERNEL_DATA_SELECTOR (2 << 3)

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