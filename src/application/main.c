// nasm -f elf32 start.s -o start.o
// x86_64-elf-gcc -m32 -c main.c -o main.o
// x86_64-elf-ld -m elf_i386 -static main.o start.o -o main.elf -Ttext 0x40000000
int syscall_3(int nr, int arg1, int arg2, int arg3){
    int ret;
    asm volatile(
        "int $0x80\n\t"
        : "=a"(ret)
        : "a"(nr), "b"(arg1), "c"(arg2), "d"(arg3));
    return ret;
}

int main(void){
    char* message = "Hello MiniOS\r\n";
    // print
    syscall_3(2, 1, (int)message, 14);
    // exit
    syscall_3(9, 0, 0, 0);
    return 0;
}