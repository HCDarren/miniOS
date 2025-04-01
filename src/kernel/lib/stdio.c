#include <lib/stdio.h>
#include <sys_call.h>

fd_t fopen(const char* file_name, int flags) {
    return syscall_2(sys_open, (u32_t)file_name, flags);
}

fd_t dup(fd_t fd) {
    return syscall_1(sys_dup, fd);
}