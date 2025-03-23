#ifndef MINIOS_SYS_CALL_H
#define MINIOS_SYS_CALL_H

// 系统调用号 80
#define INTERRUPT_SYS_CALL_NUMBER 0x80

void syscall_init();

#endif