#include <os.h>
#include <drivers/console.h>
#include <printk.h>
#include <interrupt.h>
#include <time.h>
#include <scheduler.h>
#include <gdt.h>
#include <base/assert.h>

int os_magic = OS_MAGIC;

void other_process();
void main_process();

extern void task_switch(void* eip);

task_t main_task = {0};
task_t other_task = {0};

char main_stack[1024];
char other_stack[1024];

void kernel_init(){
    gdt_init();
    console_init();
    interrupt_init();
    time_init();

    init_task(&other_task, (u32_t*)other_process, (u32_t*)&main_stack[1024]);
    init_task(&main_task, (u32_t*)main_process, (u32_t*)&other_stack[1024]);
    other_process();
}

void main_process(){
    int count = 0;
    for (;;)
    {
       printk("main process: %d\r\n",++count);
       task_switch(other_process);
    }
}

void other_process(){
    int count = 0;
    for (;;)
    {
       printk("other process: %d\r\n",++count);
       task_switch(main_process);
    }
}