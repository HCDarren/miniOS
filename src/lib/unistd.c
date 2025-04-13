#include <lib/include/unistd.h>
#include <lib/include/syscall.h>

pid_t fork() {
    return syscall_0(sys_fork);
}

pid_t getpid() {
    return syscall_0(sys_getpid);
}

pid_t getppid() {
    return syscall_0(sys_getppid);
}