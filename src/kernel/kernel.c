#include <os.h>
#include <console.h>
#include <printk.h>
#include <stdarg.h>

int os_magic = OS_MAGIC;

void kernel_init(){
    console_init();
    for (int i = 0; i < 40; i++) {
        printk("Hello Wolrd! %d : %s\r\n", i, "Darren");
    }
}