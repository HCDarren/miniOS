#ifndef MINIOS_PRINTK_H
#define MINIOS_PRINTK_H

#include <base/stdarg.h>

void printk(const char* format, ...);

int vsprintf(char buf[], const char *format, va_list args);

#endif