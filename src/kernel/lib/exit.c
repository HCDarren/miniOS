#include <lib/exit.h>
#include <sys_call.h>

void exit(u32_t status) {
    syscall_1(sys_exit, status);
}