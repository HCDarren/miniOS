#include <os.h>
#include <console.h>

char message[] = "Hello Wolrd!\r\n";

int os_magic = OS_MAGIC;

void kernel_init(){
    console_init();
    for (int i = 0; i < 40; i++) {
        console_write(message, sizeof(message) - 1);
    }
}