#include <lib/include/stdio.h>
#include <lib/include/syscall.h>

fd_t fopen(const char* file_name, int flags) {
    return syscall_2(sys_open, (u32_t)file_name, flags);
}

fd_t dup(fd_t fd) {
    return syscall_1(sys_dup, fd);
}

u32_t fread(fd_t fd, void* buf, u32_t size) {
    return syscall_3(sys_read, fd, (u32_t)buf, size);
}

u32_t fgets(fd_t fd, void* buf, u32_t size) {
    return syscall_3(sys_read, fd, (u32_t)buf, size);
}

u32_t fclose(fd_t fd) {
    return syscall_1(sys_close, fd);
}