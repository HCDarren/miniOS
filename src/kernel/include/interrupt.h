#ifndef MINIOS_INTERRUPT_H
#define MINIOS_INTERRUPT_H

#include <os.h>

extern void interrupt_bridge();

// 中断描述符表指针
typedef struct idt_descriptor_ptr
{
    u16 limit;
    u32 base;
} __packed idt_descriptor_ptr;

// 中断描述符
typedef struct idt_descriptor
{
    // 段内偏移 0 ~ 15 位
    u16 offset0;   
    // 代码段选择子 
    u16 selector;   
    // 保留不用
    u8 reserved;    
    // 任务门/中断门/陷阱门
    u8 type : 4;    
    // segment = 0 表示系统段
    u8 segment : 1; 
    // 使用 int 指令访问的最低权限
    u8 DPL : 2;    
    // 是否有效 
    u8 present : 1; 
    // 段内偏移 16 ~ 31 位
    u16 offset1;    
} __packed idt_descriptor;

// 激活初始化中断芯片
void interrupt_init();

#endif