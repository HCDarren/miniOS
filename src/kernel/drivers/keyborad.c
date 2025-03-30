#include <drivers/keyborad.h>
#include <drivers/io.h>
#include <interrupt.h>
#include <printk.h>

// 键盘数据端口
#define KEYBOARD_DATA_PORT 0x60
// 键盘控制端口
#define KEYBOARD_CTRL_PORT 0x64

extern void interrupt_handler_keyborad();

void do_interrupt_handler_keyborad(exception_frame_t* exception_frame) {
    pic_send_eoi(KEYBORAD_INTERRUPT_NUMBER);
    u32_t status = inb(KEYBOARD_DATA_PORT);
    printk("do_interrupt_handler_keyborad---->%d\n\r", status);
}

// 键盘驱动初始化
void keyborad_init() {
    register_interrupt_handler(KEYBORAD_INTERRUPT_NUMBER, interrupt_handler_keyborad);
    open_hardware_interrupt(KEYBORAD_INTERRUPT_NUMBER);
}