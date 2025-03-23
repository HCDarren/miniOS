#ifndef MINIOS_SYS_CALL_H
#define MINIOS_SYS_CALL_H

#include <os.h>

// 系统调用号 80
#define INTERRUPT_SYS_CALL_NUMBER 0x80

typedef enum syscall_t
{
    SYS_NR_WRITE = 4,
};

void syscall_init();


u32_t write(fd_t fd, char *buf, u32_t len);

#endif