
#ifndef MINIOS_PRINTF_H
#define MINIOS_PRINTF_H

#include <base/stdarg.h>

int printf(const char *fmt, ...);

int vsprintf(char buf[], const char *format, va_list args);

#endif