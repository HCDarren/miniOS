#include <os.h>
#include <drivers/console.h>
#include <printk.h>
#include <interrupt.h>
#include <time.h>
#include <scheduler.h>
#include <gdt.h>
#include <base/assert.h>

int os_magic = OS_MAGIC;


void kernel_init(){
    gdt_init();
    console_init();
    //interrupt_init();
    time_init();
    task_init();
}