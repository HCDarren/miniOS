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

void bitmap_init(bitmap_t* bitmap, u8_t* bits, u32_t count, u8_t init_value);

#endif