#ifndef MINIOS_KEYBORAD_H
#define MINIOS_KEYBORAD_H

#include <os.h>

// 键盘驱动初始化
void keyborad_init();

typedef struct input_keymap_entry
{
    u8_t keycode;
    // 按下 shift 键
	u8_t shift_keycode;
} input_keymap_entry_t;


#endif