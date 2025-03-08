
#ifndef MINIOS_CONSOLE_H
#define MINIOS_CONSOLE_H
#include <os.h>

void console_init();
void console_clear();
void console_write(const char *buf, u32 count);

#endif