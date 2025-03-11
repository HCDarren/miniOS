#ifndef MINIOS_INTERRUPT_H
#define MINIOS_INTERRUPT_H

#include <os.h>

 // 时钟中断号
#define INTERRUPT_CLOCK_NUMBER 0x20 

typedef void *interrupt_method_t; // 中断处理函数

// 中断描述符表指针
typedef struct idt_descriptor_ptr
{
    u16_t limit;
    u32_t base;
} __packed idt_descriptor_ptr;

// 中断描述符
typedef struct idt_descriptor
{
    // 段内偏移 0 ~ 15 位
    u16_t offset0;   
    // 代码段选择子 
    u16_t selector;   
    // 保留不用
    u8_t reserved;    
    // 任务门/中断门/陷阱门
    u8_t type : 4;    
    // segment = 0 表示系统段
    u8_t segment : 1; 
    // 使用 int 指令访问的最低权限
    u8_t DPL : 2;    
    // 是否有效 
    u8_t present : 1; 
    // 段内偏移 16 ~ 31 位
    u16_t offset1;    
} __packed idt_descriptor;

// 激活初始化中断芯片
void interrupt_init();

// 打开硬件中断
void open_hardware_interrupt(int interrupt_number);

// 注册中断处理函数
void register_interrupt_handler(int interrupt_number, interrupt_method_t method);

#endif