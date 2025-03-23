#include <os.h>
#include <drivers/console.h>
#include <printk.h>
#include <interrupt.h>
#include <time.h>
#include <task/task_manager.h>
#include <gdt.h>
#include <sys_call.h>
#include <base/assert.h>
#include <base/asm_instruct.h>
#include <memory/memory_manager.h>

int os_magic = OS_MAGIC;


void kernel_init(){
    gdt_init();
    console_init();
    memory_init();
    interrupt_init();
    syscall_init();
    task_init();
    time_init();
}