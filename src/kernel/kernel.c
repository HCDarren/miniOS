#include <os.h>

char message[] = "Hello Wolrd!";

int os_magic = OS_MAGIC;

void kernel_init(){
    char* video = (char*) 0xb8000;
    for (int i = 0; i < sizeof(message); ++i) {
        video[i*2] = message[i];
    }
}