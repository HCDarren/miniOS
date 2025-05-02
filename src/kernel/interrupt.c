#include <interrupt.h>
#include <drivers/io.h>
#include <printk.h>
#include <base/assert.h>
#include <base/asm_instruct.h>
#include <task/task.h>
#include <memory/memory_manager.h>

#define PIC_M_CTRL 0x20 // 主片的控制端口
#define PIC_M_DATA 0x21 // 主片的数据端口
#define PIC_S_CTRL 0xa0 // 从片的控制端口
#define PIC_S_DATA 0xa1 // 从片的数据端口

// 硬件中断芯片的中断号起始位置
#define HARDWARE_INTERRUPT_START 0x20
#define HARDWARE_INTERRUPT_END 0x30

// 通知中断控制器中断结束
#define PIC_EOI 0x20    

// 中断的向量表的起始位置，覆盖 bios 的中断向量数据
#define INTERRUPT_TABLE_START 0x00000000
// 中断向量表的长度 1K / 8 = 128
#define INTERRUPT_TABLE_LENGTH 256

static idt_descriptor_ptr idt_ptr;

// 默认中断函数
extern void interrupt_handler_defalut();
// 一般性保护异常函数
extern void interrupt_handler_general_protection();
// 页错误中断函数
extern void interrupt_handler_page_fault();

// 缺页异常
void do_interrupt_handler_page_fault(exception_frame_t* exception_frame) {
    // 建立页表映射，输出错误码信息，缺页的地址在寄存器 cr2 中
    void* page_fault_addr = (void*)read_cr2();
    task_t* task = current_running_task();
    if (task && task->sbrk != 0 && page_fault_addr > task->sbrk) {
        // 非法的访问，要发信号给用户进程，然后终止这个进程，没那么多时间没写了
        return;
    }
    page_mapping_dir_t * page_dir = (page_mapping_dir_t *)read_cr3();
    // 添加一个物理页作为映射
    void* new_physics_addr = alloc_a_page();
    // 页往下对其
    void* virtual_addr = (void*)DOWN_ON(page_fault_addr, PAGE_SIZE);
    // printk("do_interrupt_handler_page_fault: page_fault_addr = 0x%x, page_dir = 0x%x, new_physics_addr = 0x%x, virtual_addr = 0x%x\r\n", page_fault_addr, page_dir, new_physics_addr, virtual_addr);
    create_memory_mapping(page_dir, (void*)virtual_addr, new_physics_addr, 1);
    // 刷新快表，好像不刷新也没有关系，应该是虚拟机的问题
    flush_tlb(page_fault_addr);
}

// 一般性保护异常
void do_interrupt_handler_general_protection(exception_frame_t* exception_frame) {
    // 0-1G 只允许用户进程访问，elf 某些段用户进程也不能写，这些都会触发一般性保护
    // 发送一个信号给进程，执行进程信号注册函数，终止进程，没那么多时间没写了
    printk("do_interrupt_handler_general_protection\r\n");
}

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
    // ICW4: 8086模式, 手动EOI
    outb(PIC_M_DATA, 0b00000001);

    // ICW1: 边沿触发, 级联 8259, 需要ICW4.
    outb(PIC_S_CTRL, 0b00010001);
    // ICW2: 起始中断向量号 0x28
    outb(PIC_S_DATA, 0x28);
    // ICW3: 设置从片连接到主片的 IR2 引脚
    outb(PIC_S_DATA, 2);
    // ICW4: 8086模式, 手动EOI
    outb(PIC_S_DATA, 0b00000001);
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
        idt->DPL = 3;
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
        idt->offset0 = (u32_t)interrupt_handler_defalut & 0xffff;
        idt->offset1 = ((u32_t)interrupt_handler_defalut >> 16) & 0xffff;
    }

    idt_ptr.limit = 0x7FF;
    idt_ptr.base = INTERRUPT_TABLE_START;

    asm volatile("lidt idt_ptr\n");

    register_interrupt_handler(PAGE_FAULT_INTERRUPT_NUMBER, interrupt_handler_page_fault);
    register_interrupt_handler(GENERAL_PROTECTION_INTERRUPT_NUMBER, interrupt_handler_general_protection);
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
    assert((interrupt_number >= 0) && (interrupt_number < INTERRUPT_TABLE_LENGTH));
    idt_descriptor *interrupt_table = (idt_descriptor *)INTERRUPT_TABLE_START;
    idt_descriptor* idt = &interrupt_table[interrupt_number];
    // 设置中断处理函数的位置
    idt->offset0 = (u32_t)method & 0xffff;
    idt->offset1 = ((u32_t)method >> 16) & 0xffff;
}

// 打开硬件中断
void open_hardware_interrupt(int interrupt_number)
{
    assert((HARDWARE_INTERRUPT_START <= interrupt_number) && (interrupt_number <= HARDWARE_INTERRUPT_END));
    interrupt_number -= HARDWARE_INTERRUPT_START;
    if (interrupt_number < 8) {
        u8_t mask = inb(PIC_M_DATA) & ~(1 << interrupt_number);
        outb(PIC_M_DATA, mask);
    } else {
        u8_t mask = inb(PIC_S_DATA) & ~(1 << interrupt_number);
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

void do_interrupt_handler_defalut(exception_frame_t* exception_frame)
{
    printk("interrupt_exception!!!\r\n");
}

void pic_send_eoi(int interrupt_number) {
    assert((HARDWARE_INTERRUPT_START <= interrupt_number) && (interrupt_number <= HARDWARE_INTERRUPT_END));
    interrupt_number -= HARDWARE_INTERRUPT_START;
    if (interrupt_number < 8) {
        outb(PIC_M_CTRL, PIC_EOI);
    } else {
        outb(PIC_S_CTRL, PIC_EOI);
    }
}

void interrupt_init()
{
    activate_interrupt_chipset();
    init_interrupt_table();
    open_cpu_interrupt();
}

// 关中断的方式进入临界保护区
bool enter_critical_protection()
{
    // eflag 没办法直接获取，但是有 push 和 pop 一套组合拳
    asm volatile(
        "pushfl\n"
        "cli\n"
        "popl %eax\n"
        "shrl $9, %eax\n"
        "andl $1, %eax\n"
    );
}

// 退出临界保护区，恢复之前中断状态
void leave_critical_protection(bool state)
{
    set_interrupt_state(state);
}