#include <interrupt.h>
#include <io.h>
#include <printk.h>

#define PIC_M_CTRL 0x20 // 主片的控制端口
#define PIC_M_DATA 0x21 // 主片的数据端口
#define PIC_S_CTRL 0xa0 // 从片的控制端口
#define PIC_S_DATA 0xa1 // 从片的数据端口

// 硬件中断芯片的中断号起始位置
#define HARDWARE_INTERRUPT_START 0x20
#define HARDWARE_INTERRUPT_END 0x30

// 中断的向量表的起始位置，覆盖 bios 的中断向量数据
#define INTERRUPT_TABLE_START 0x00000000
// 中断向量表的长度 1K / 8 = 128
#define INTERRUPT_TABLE_LENGTH 128

static idt_descriptor_ptr idt_ptr;

extern void interrupt_handler_defalut();

// 激活初始化中断芯片组
// ICW1，OCW2，OCW3 是用偶地址端口主片的 0x20 或从片的 0xA0 写入。
// ICW2~ICW4 和 OCW1 是用奇地址端口主片的 0x21 或从片的 0xA1 写入。
// 以上 4 个 ICW 要保证一定的次序写入，8259A 就知道写入端口的数据是什么了。
static void activate_interrupt_chipset()
{
    // ICW1: 边沿触发, 级联 8259, 需要ICW4.
    outb(PIC_M_CTRL, 0b00010001);
    // ICW2: 起始中断向量号 0x20
    outb(PIC_M_DATA, HARDWARE_INTERRUPT_START);
    // ICW3: IR2接从片.
    outb(PIC_M_DATA, 0b00000100);
    // ICW4: 8086模式, 自动EOI
    outb(PIC_M_DATA, 0b00000011);

    // ICW1: 边沿触发, 级联 8259, 需要ICW4.
    outb(PIC_S_CTRL, 0b00010001);
    // ICW2: 起始中断向量号 0x28
    outb(PIC_S_DATA, 0x28);
    // ICW3: 设置从片连接到主片的 IR2 引脚
    outb(PIC_S_DATA, 2);
    // ICW4: 8086模式, 自动EOI
    outb(PIC_S_DATA, 0b00000011);
    // 暂时先关闭所有硬件中断
    outb(PIC_M_DATA, 0b11111111);
    // 暂时先关闭所有硬件中断
    outb(PIC_S_DATA, 0b11111111);
}

// 初始化中断向量表
static void init_interrupt_table()
{
    idt_descriptor *interrupt_table = (idt_descriptor *)INTERRUPT_TABLE_START;
    for (size_t i = 0; i < INTERRUPT_TABLE_LENGTH; ++i)
    {
        idt_descriptor* idt = &interrupt_table[i];
        // 0 特权级别
        idt->DPL = 0;
        // 代码段选择子
        idt->selector = 1 << 3;
        // 保留不用
        idt->reserved = 0;  
        // 中断门    
        idt->type = 0b1110; 
        // 系统段    
        idt->segment = 0;  
        // 有效         
        idt->present = 1; 
        
        // 设置中断处理函数的位置
        idt->offset0 = (u32)interrupt_handler_defalut & 0xffff;
        idt->offset1 = ((u32)interrupt_handler_defalut >> 16) & 0xffff;
    }

    idt_ptr.limit = 0x3FF;
    idt_ptr.base = INTERRUPT_TABLE_START;

    asm volatile("lidt idt_ptr\n");
}

// 设置 IF 位
static void set_interrupt_state(bool state)
{
    if (state)
        asm volatile("sti\n");
    else
        asm volatile("cli\n");
}

// 注册中断处理函数
void register_interrupt_handler(int interrupt_number, interrupt_method_t method) {
    // 缺 assert
    idt_descriptor *interrupt_table = (idt_descriptor *)INTERRUPT_TABLE_START;
    idt_descriptor* idt = &interrupt_table[interrupt_number];
    // 设置中断处理函数的位置
    idt->offset0 = (u32)method & 0xffff;
    idt->offset1 = ((u32)method >> 16) & 0xffff;
}

// 打开硬件中断
void open_hardware_interrupt(int interrupt_number)
{
    // 缺 assert
    interrupt_number -= HARDWARE_INTERRUPT_START;
    if (interrupt_number < 8) {
        u8 mask = inb(PIC_M_DATA) & ~(1 << interrupt_number);
        outb(PIC_M_DATA, mask);
    } else {
        u8 mask = inb(PIC_S_DATA) & ~(1 << interrupt_number);
        outb(PIC_S_DATA, mask);
    }
}

// 打开 cpu 中断
void open_cpu_interrupt()
{
    set_interrupt_state(true);
}

// 关闭 cpu 中断
void close_cpu_interrupt()
{
    set_interrupt_state(false);
}

void do_interrupt_handler_defalut()
{
    printk("interrupt_exception!!!\r\n");
}

void interrupt_init()
{
    activate_interrupt_chipset();
    init_interrupt_table();
    open_cpu_interrupt();
}