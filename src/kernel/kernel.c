#include <os.h>
#include <drivers/console.h>
#include <printk.h>
#include <interrupt.h>
#include <time.h>
#include <task/task_manager.h>
#include <gdt.h>
#include <base/assert.h>
#include <memory/memory_manager.h>

int os_magic = OS_MAGIC;


void kernel_init(){
    console_init();
    memory_init();
    task_init();
    while (true)
    {
        /* code */
    }
    
}