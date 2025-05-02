#ifndef MINIOS_FORK_H
#define MINIOS_FORK_H

#include <os.h>

pid_t fork();

pid_t getpid();

pid_t getppid();

void* sbrk(u32_t);

#endif