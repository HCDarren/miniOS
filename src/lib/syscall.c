
#include <lib/include/syscall.h>

// 三个参数的系统调用
u32_t syscall_3(u32_t nr, u32_t arg1, u32_t arg2, u32_t arg3)
{
    u32_t ret;
    asm volatile(
        "int $0x80\n\t"
        : "=a"(ret)
        : "a"(nr), "b"(arg1), "c"(arg2), "d"(arg3));
    return ret;
}

// 两个参数的系统调用
u32_t syscall_2(u32_t nr, u32_t arg1, u32_t arg2)
{
    u32_t ret;
    asm volatile(
        "int $0x80\n\t"
        : "=a"(ret)
        : "a"(nr), "b"(arg1), "c"(arg2));
    return ret;
}

// 一个参数的系统调用
u32_t syscall_1(u32_t nr, u32_t arg1)
{
    u32_t ret;
    asm volatile(
        "int $0x80\n\t"
        : "=a"(ret)
        : "a"(nr), "b"(arg1));
    return ret;
}

// 0个参数的系统调用
u32_t syscall_0(u32_t nr)
{
    u32_t ret;
    asm volatile(
        "int $0x80\n\t"
        : "=a"(ret)
        : "a"(nr));
    return ret;
}