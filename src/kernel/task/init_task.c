#include <task/init_task.h>
#include <task/task.h>
#include <gdt.h>
#include <memory/memory_manager.h>
#include <lib/include/printf.h>
#include <lib/include/unistd.h>
#include <lib/include/stdio.h>
#include <lib/include/sleep.h>
#include <lib/include/exit.h>
#include <lib/include/execve.h>
#include <base/asm_instruct.h>

void real_init_thread()
{
    fd_t fd = fopen("/dev/tty", 0);
    dup(fd);
    dup(fd);

    pid_t pid = fork();
    if (pid == 0) {
        // 执行 shell 进程
        execve("SHELL.ELF", NULL, NULL);
    } else {
        // init 进程简单写了，一直睡眠
        while (true)
        {
           sleep(10000);
        }
    }
}

void init_thread()
{
    pre_jmp_to_user_mode(real_init_thread);
}