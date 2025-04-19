#ifndef MINIOS_MEMORY_H
#define MINIOS_MEMORY_H

#include <os.h>

// 字符串拷贝
void strcpy(char dst[], char* src);

// 拷贝数据
void memcpy(void *dst, const void *src, size_t count);

// 填充数据
void memset(void *dst, const size_t data, const size_t len);

// 字符串的比较
int strcmp(const char *s1, const char *s2);

#endif