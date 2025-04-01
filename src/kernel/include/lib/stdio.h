#ifndef MINIOS_STDIO_H
#define MINIOS_STDIO_H

#include <os.h>

fd_t fopen(const char* file_name, int flags);

fd_t dup(fd_t fd);

#endif