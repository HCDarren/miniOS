#ifndef MINIOS_MEMORY_MANAGER_H
#define MINIOS_MEMORY_MANAGER_H

#include <os.h>
#include <printk.h>

typedef struct memory_info
{
    // 内存基地址
    u64_t base; 
    // 内存长度
    u64_t size; 
    // 类型
    u32_t type; 
} __packed memory_info_t;

void memory_init();

#endif