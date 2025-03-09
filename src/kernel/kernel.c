#include <os.h>
#include <console.h>
#include <printk.h>
#include <interrupt.h>

int os_magic = OS_MAGIC;

void kernel_init(){
    console_init();
    asm volatile("xchgw %bx, %bx");
    interrupt_init();
    asm volatile("xchgw %bx, %bx");
    int num = 2 / 0;
    int a = 10;
}