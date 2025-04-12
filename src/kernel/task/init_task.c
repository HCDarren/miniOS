#include <task/init_task.h>
#include <task/task.h>
#include <gdt.h>
#include <memory/memory_manager.h>
#include <lib/printf.h>
#include <lib/unistd.h>
#include <lib/stdio.h>
#include <lib/sleep.h>
#include <lib/exit.h>
#include <lib/execve.h>
#include <base/asm_instruct.h>

void real_init_thread()
{
    fd_t fd = fopen("/dev/console", 0);
    dup(fd);
    dup(fd);

    pid_t pid = fork();
    if (pid == 0) {
        // 子进程，执行 elf 文件
        execve("MAIN.ELF", NULL, NULL);
    }
    
    while (true)
    {
       sleep(1000);
    }
}

void init_thread()
{
    pre_jmp_to_user_mode(real_init_thread);
}