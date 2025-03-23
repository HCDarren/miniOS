#include <sys_call.h>
#include <interrupt.h>
#include <printk.h>
#include <memory/memory_manager.h>

extern void interrupt_handler_syscall();

// 系统调用处理函数
void do_interrupt_handler_syscall(exception_frame_t* exception_frame) {
   printk("---->");
}

// 注册 0x80 系统调用号
void syscall_init() { 
   register_interrupt_handler(INTERRUPT_SYS_CALL_NUMBER, interrupt_handler_syscall);
}

// 三个参数的系统调用
static inline u32_t syscall3(u32_t nr, u32_t arg1, u32_t arg2, u32_t arg3)
{
    u32_t ret;
    asm volatile(
        "int $0x80\n\t"
        : "=a"(ret)
        : "a"(nr), "b"(arg1), "c"(arg2), "d"(arg3));
    return ret;
}

u32_t write(fd_t fd, char *buf, u32_t len)
{
    return syscall3(SYS_NR_WRITE, fd, (u32_t)buf, len);
}