#include <lib/include/exit.h>
#include <lib/include/syscall.h>

void exit(u32_t status) {
    syscall_1(sys_exit, status);
}