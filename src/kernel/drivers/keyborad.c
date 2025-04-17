#include <drivers/keyborad.h>
#include <drivers/io.h>
#include <interrupt.h>
#include <printk.h>
#include <drivers/tty.h>
// 键盘数据端口
#define KEYBOARD_DATA_PORT 0x60
// 键盘控制端口
#define KEYBOARD_CTRL_PORT 0x64

#define KEY_SHIFT_DOWN 42
#define KEY_SHIFT_UP 170

static bool is_shift_down = false;

extern void interrupt_handler_keyborad();

static const input_keymap_entry_t keymap[] = {
    [0x02] = {'1', '!'},
    [0x03] = {'2', '@'},
    [0x04] = {'3', '#'},
    [0x05] = {'4', '$'},
    [0x06] = {'5', '%'},
    [0x07] = {'6', '^'},
    [0x08] = {'7', '&'},
    [0x09] = {'8', '*'},
    [0x0A] = {'9', '('},
    [0x0B] = {'0', ')'},
    [0x0C] = {'-', '_'},
    [0x0D] = {'=', '+'},
    [0x0E] = {'\b', '\b'},
    [0x0F] = {'\t', '\t'},
    [0x10] = {'q', 'Q'},
    [0x11] = {'w', 'W'},
    [0x12] = {'e', 'E'},
    [0x13] = {'r', 'R'},
    [0x14] = {'t', 'T'},
    [0x15] = {'y', 'Y'},
    [0x16] = {'u', 'U'},
    [0x17] = {'i', 'I'},
    [0x18] = {'o', 'O'},
    [0x19] = {'p', 'P'},
    [0x1A] = {'[', '{'},
    [0x1B] = {']', '}'},
    [0x1C] = {'\n', '\n'},
    [0x1E] = {'a', 'A'},
    [0x1F] = {'s', 'B'},
    [0x20] = {'d', 'D'},
    [0x21] = {'f', 'F'},
    [0x22] = {'g', 'G'},
    [0x23] = {'h', 'H'},
    [0x24] = {'j', 'J'},
    [0x25] = {'k', 'K'},
    [0x26] = {'l', 'L'},
    [0x27] = {';', ':'},
    [0x28] = {'\'', '"'},
    [0x29] = {'`', '~'},
    [0x2B] = {'\\', '|'},
    [0x2C] = {'z', 'Z'},
    [0x2D] = {'x', 'X'},
    [0x2E] = {'c', 'C'},
    [0x2F] = {'v', 'V'},
    [0x30] = {'b', 'B'},
    [0x31] = {'n', 'N'},
    [0x32] = {'m', 'M'},
    [0x33] = {',', '<'},
    [0x34] = {'.', '>'},
    [0x35] = {'\/', '?'},
    [0x39] = {' ', ' '},
};

// 定义键盘码转换表

void do_interrupt_handler_keyborad(exception_frame_t *exception_frame)
{
    pic_send_eoi(KEYBORAD_INTERRUPT_NUMBER);
    u32_t scancode = inb(KEYBOARD_DATA_PORT);

    // shift 按下
    if (scancode == KEY_SHIFT_DOWN) {
        is_shift_down = true;
        return;
    }
    // shift 抬起
    if (scancode == KEY_SHIFT_UP) {
        is_shift_down = false;
        return;
    }

    // 扫描码抬起
    if (((scancode & 0x0080) != 0))
    {
        return;
    }
    
    char keycode = is_shift_down ? keymap[scancode].shift_keycode : keymap[scancode].keycode;
    tty_put(keycode);
}

// 键盘驱动初始化
void keyborad_init()
{
    register_interrupt_handler(KEYBORAD_INTERRUPT_NUMBER, interrupt_handler_keyborad);
    open_hardware_interrupt(KEYBORAD_INTERRUPT_NUMBER);
}