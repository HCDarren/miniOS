#ifndef MINIOS_STDIO_H
#define MINIOS_STDIO_H

#include <os.h>

fd_t fopen(const char* file_name, int flags);

fd_t dup(fd_t fd);

u32_t fread(fd_t fd, void* buf, u32_t size);

u32_t fgets(fd_t fd, void* buf, u32_t size);

u32_t fclose(fd_t fd);

#endif