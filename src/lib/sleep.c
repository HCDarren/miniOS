#include <lib/include/sleep.h>
#include <lib/include/syscall.h>

void sleep(u32_t sleep_time) {
    syscall_1(sys_sleep, sleep_time);
}