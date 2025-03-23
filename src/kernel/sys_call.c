#include <sys_call.h>
#include <interrupt.h>
#include <printk.h>

extern void interrupt_handler_syscall();

// 系统调用处理函数
void do_interrupt_handler_syscall(exception_frame_t* exception_frame) {
   
}

void syscall_init() { 
   register_interrupt_handler(INTERRUPT_SYS_CALL_NUMBER, interrupt_handler_syscall);
   asm volatile("int $0x80");
}