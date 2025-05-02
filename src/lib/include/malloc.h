#ifndef MINIOS_MALLOC_H
#define MINIOS_MALLOC_H

#include <os.h>

// 内存控制块
typedef struct memory_control_block
{
    u32_t size; // 申请的动态内存大小
    bool is_free; // 是否空闲
    struct memory_control_block* prev; // 前一个动态内存控制块
} memory_control_block_t;

void* malloc(size_t size);

void* remalloc(void* ptr, size_t size);

void free(void* ptr);

#endif