#include <os.h>
#include <fs/fs.h>
#include <drivers/keyborad.h>
#include <printk.h>
#include <interrupt.h>
#include <time.h>
#include <task/task.h>
#include <gdt.h>
#include <kernel_call.h>
#include <base/assert.h>
#include <base/asm_instruct.h>
#include <memory/memory_manager.h>

int os_magic = OS_MAGIC;


void kernel_init(){
    interrupt_init();
    fs_init();
    memory_init();
    gdt_init();
    keyborad_init();
    syscall_init();
    task_init();
    time_init();
}