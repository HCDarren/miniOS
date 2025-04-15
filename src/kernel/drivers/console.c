
#include <drivers/console.h>
#include <drivers/io.h>

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
// 屏幕文本行数 
#define CRT_HEIGHT 25                     
// 光标位置
static u16_t cursor_position;
// 当前显卡的显示位置
static char* vga_position;

// 设置光标的位置
static void set_cursor_position() {
    outb(CRT_ADDR_REG, CRT_CURSOR_H); 
    u8_t cursor_h = cursor_position >> 8;
    outb(CRT_DATA_REG, cursor_h);

    outb(CRT_ADDR_REG, CRT_CURSOR_L); 
    u8_t cursor_l = cursor_position;
    outb(CRT_DATA_REG, cursor_l);
}

// 设置显卡滚屏位置
static void set_vga_scroll_position(u16_t scroll_position) {
    outb(CRT_ADDR_REG, CRT_START_ADDR_H); 
    u8_t scroll_position_h = scroll_position >> 8;
    outb(CRT_DATA_REG, scroll_position_h);

    outb(CRT_ADDR_REG, CRT_START_ADDR_L); 
    u8_t scroll_position_l = scroll_position;
    outb(CRT_DATA_REG, scroll_position_l);
}

// 设置显卡滚屏位置
static u16_t get_vga_scroll_position() {
    outb(CRT_ADDR_REG, CRT_START_ADDR_H); 
    u8_t scroll_position_h = inb(CRT_DATA_REG);
    outb(CRT_ADDR_REG, CRT_START_ADDR_L); 
    u8_t scroll_position_l = inb(CRT_DATA_REG);
    u16_t scroll_position = scroll_position_h << 8;
    scroll_position |= scroll_position_l;
    return scroll_position;
}

// 是否需要滚屏操作
static void scroll_up(int offset) {
    int current_cursor_position = cursor_position;
    if (current_cursor_position >= CRT_MEM_SIZE) {
        // 把当前屏幕的数据拷贝到最前面去
        u16_t scroll_position = get_vga_scroll_position();
        int char_number = current_cursor_position - scroll_position;
        int char_mem = char_number * 2;
        char* dst = (char*)CRT_MEM_BASE;
        char* res = (vga_position - char_mem);
        for (int i = 0; i < char_mem; i++) {
            dst[i] = res[i];
        }
        // 把后面的区域清掉
        u16_t* erase_start = (u16_t*)CRT_MEM_BASE + char_mem;
        int len = CRT_MEM_SIZE - char_number;
        for (int i = 0; i < CRT_MEM_SIZE; i++) {
            erase_start[i] = ERASE;
        }
        // 重新设置光标、滚动位置等
        set_vga_scroll_position(0);
        cursor_position = char_number;
        set_cursor_position();
        vga_position = (char*)CRT_MEM_BASE + char_mem;
        return;
    } 
    u16_t scroll_position = get_vga_scroll_position();
    current_cursor_position -= scroll_position;
    if (current_cursor_position >= CRT_WIDTH * CRT_HEIGHT) {
        // 滚动一屏幕
        scroll_position += 2 * CRT_WIDTH;
        set_vga_scroll_position(scroll_position);
        return;
    }
}

// 清屏操作
void console_clear() {
    // 清屏怎么做？只是清除当前屏幕的操作，mac 上数据往上翻还是能翻到数据
    // 这里我们简单一点，全部清掉。
    // 1、屏幕缓冲区都填充空格
    u16_t* vga_mem_start = (u16_t*)CRT_MEM_BASE;
    for (u32_t i = 0; i < CRT_MEM_SIZE; i++) {
        vga_mem_start[i] = ERASE;
    }
    // 2、重新设置屏幕翻屏显示位置
    set_vga_scroll_position(0);
    // 3、设置光标为 0 
    cursor_position = 0;
    set_cursor_position();
}

void console_init() {
    vga_position = (char*)CRT_MEM_BASE;
    console_clear();
}

// 删除字符
static inline void console_erase(u32_t erase_count) {
    cursor_position -= erase_count;
    u16_t* cur_vga_position = (u16_t*)vga_position;
    vga_position -= erase_count * 2;

    for (u16_t* start = (u16_t*)vga_position; start < cur_vga_position; start++) {
        *start = ERASE;
    }
}

void console_write(const char *buf, u32_t count) {
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
        case '\b': // 删除
            console_erase(1);
            break;
        default:
            // 滚屏问题
            cursor_position += 1;
            *vga_position = c;
            vga_position += 2;
            break;
        }
    }
    scroll_up(0);
    set_cursor_position();
}