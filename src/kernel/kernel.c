#include <os.h>
#include <console.h>
#include <printk.h>
#include <interrupt.h>
#include <time.h>

int os_magic = OS_MAGIC;

void kernel_init(){
    console_init();
    interrupt_init();
    time_init();
    for (;;)
    {
       int i = 10;
    }
}