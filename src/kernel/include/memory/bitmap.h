#ifndef MINIOS_BITMAP_H
#define MINIOS_BITMAP_H

#include <os.h>

/*
定义位图结构体
0 为可用，1 为不可用
*/
typedef struct bitmap
{
    // 位图开始的位置，只能按照字节存取
    u8_t* bits;
    // 位图的大小
    u32_t size;
} __packed bitmap_t;

void bitmap_init(bitmap_t* bitmap, u8_t* bits, u32_t count);

// 扫描分配，连续的标记分配
int bitmap_scan_alloc(bitmap_t* bitmap, const u32_t count);

// 释放 bitmap
void bitmap_free(bitmap_t* bitmap, u32_t index, const u32_t free_count);
#endif