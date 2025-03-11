#ifndef MINIOS_MEMORY_H
#define MINIOS_MEMORY_H

#include <os.h>

// 拷贝数据
void memcpy(void *dst, const void *src, size_t count);

// 填充数据
void memset(void *dst, const size_t data, const size_t len);

#endif