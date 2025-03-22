#include <time.h>
#include <drivers/io.h>
#include <interrupt.h>
#include <printk.h>
#include <task/task_manager.h>

#define PIT_CHAN0_REG 0X40
#define PIT_CTRL_REG 0X43

extern void interrupt_handler_time();

static int index = 0;
void do_interrupt_handler_time(exception_frame_t* exception_frame) {
    pic_send_eoi(INTERRUPT_CLOCK_NUMBER);
    open_cpu_interrupt();
    schedule();
}

void time_init(void) {
    // 配置计数器 0 时钟
    outb(PIT_CTRL_REG, 0b00110110);
    outb(PIT_CHAN0_REG, CLOCK_COUNTER & 0xff);
    outb(PIT_CHAN0_REG, (CLOCK_COUNTER >> 8) & 0xff);
    
    // 注册时钟中断函数
    register_interrupt_handler(INTERRUPT_CLOCK_NUMBER, interrupt_handler_time);
    // 打开时钟中断
    open_hardware_interrupt(INTERRUPT_CLOCK_NUMBER);
}

