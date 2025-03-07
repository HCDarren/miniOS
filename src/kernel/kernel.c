#include <os.h>
#include <io.h>

char message[] = "Hello Wolrd!";

int os_magic = OS_MAGIC;

void kernel_init(){
    // 读屏幕光标位置
    outb(0x3D4, 0x0E); 
    u8 cursor_h = inb(0x3D5);
    outb(0x3D4, 0x0F); 
    u8 cursor_l = inb(0x3D5);
    u16 cursor = cursor_h * 255 + cursor_l;

    // 写屏幕光标位置
    outb(0x3D4, 0x0E); 
    outb(0x3D5, 0x01);
}