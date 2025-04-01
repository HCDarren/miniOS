#include <lib/stdio.h>
#include <sys_call.h>

int fopen(const char* file_name, int flags) {
    return syscall_2(sys_open, (u32_t)file_name, flags);
}