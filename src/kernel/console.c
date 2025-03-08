
#include <console.h>
#include <io.h>

#define ERASE 0x0720
// 显卡内存起始位置
#define CRT_MEM_BASE 0xB8000              
// 显卡能显示的字符个数
#define CRT_MEM_SIZE 0x4000  
// 显卡结束位置                    
#define CRT_MEM_END (CRT_MEM_BASE + CRT_MEM_SIZE) 
// CRT(6845)索引寄存器
#define CRT_ADDR_REG 0x3D4 
// CRT(6845)数据寄存器
#define CRT_DATA_REG 0x3D5 
// 显示内存起始位置 - 高位
#define CRT_START_ADDR_H 0xC 
// 显示内存起始位置 - 低位
#define CRT_START_ADDR_L 0xD 
// 光标位置 - 高位
#define CRT_CURSOR_H 0xE   
// 光标位置 - 低位  
#define CRT_CURSOR_L 0xF    
// 屏幕文本列数 
#define CRT_WIDTH 80                      
// 光标位置
static u16 cursor_position;
// 当前显卡的显示位置
static char* vga_position;

// 设置光标的位置
static void set_cursor_position() {
    outb(CRT_ADDR_REG, CRT_CURSOR_H); 
    u8 cursor_h = cursor_position >> 8;
    outb(CRT_DATA_REG, cursor_h);
    outb(CRT_ADDR_REG, CRT_CURSOR_L); 
    u8 cursor_l = cursor_position;
    outb(CRT_DATA_REG, cursor_l);
}

// 设置显卡内存开始的显示位置
static void set_vga_position() {
    outb(CRT_ADDR_REG, CRT_START_ADDR_H); 
    outb(CRT_DATA_REG, 0);
    outb(CRT_ADDR_REG, CRT_START_ADDR_L); 
    outb(CRT_DATA_REG, 0);
}

// 清屏操作
void console_clear() {
    // 清屏怎么做？只是清除当前屏幕的操作，mac 上数据往上翻还是能翻到数据
    // 这里我们简单一点，全部清掉。
    // 1、屏幕缓冲区都填充空格
    u16* vga_mem_start = (u16*)CRT_MEM_BASE;
    for (int i = 0; i < CRT_MEM_SIZE; i++) {
        vga_mem_start[i] = ERASE;
    }
    // 2、重新设置屏幕翻屏显示位置
    set_vga_position();
    // 3、设置光标为 0 
    cursor_position = 0;
    set_cursor_position();
}

void console_init() {
    vga_position = (char*)CRT_MEM_BASE;
    console_clear();
}

void console_write(const char *buf, u32 count) {
    // 往里面写怎么写？边界场景需要考虑哪一些?
    // 目前先够用就行，后续键盘驱动起来了再来新增调整代码
    for(int i = 0; i < count; i++) {
        char c = buf[i];
        switch (c)
        {
        case '\n': // 换行
            cursor_position += CRT_WIDTH;
            vga_position += CRT_WIDTH * 2;
            break;
        case '\r': // 回车
            int delete = (cursor_position % CRT_WIDTH);
            cursor_position -= delete;
            vga_position -= delete * 2;
            break;
        default:
            // 滚屏问题
            cursor_position += 1;
            *vga_position = c;
            vga_position += 2;
            break;
        }
    }
    set_cursor_position();
}