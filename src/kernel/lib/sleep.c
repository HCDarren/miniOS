#include <lib/sleep.h>
#include <sys_call.h>

void sleep(u32_t sleep_time) {
    syscall_1(sys_sleep, sleep_time);
}