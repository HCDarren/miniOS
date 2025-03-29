#include <task/init_task.h>
#include <task/task_manager.h>
#include <gdt.h>
#include <memory/memory_manager.h>
#include <lib/printf.h>
#include <lib/unistd.h>
#include <lib/sleep.h>
#include <lib/exit.h>
#include <lib/execve.h>

// 用户栈顶 4G
static u32_t USER_STACK_TOP = 0x0;

void real_init_thread()
{
    pid_t pid = fork();
    if (pid == 0)
    {
        printf("child process: %d\r\n", pid);
        char* args[] = {"args1", "args2", "args3", nullptr};
        execve("lib/shell.elf", args, nullptr);
    }
    else
    {
        printf("parent process: %d\r\n", pid);
    }

    while (true)
    {
        sleep(1000);
    }
}

void init_thread()
{
    user_intrrupt_frame_t user_intrrupt_frame;
    // 设置各个段寄存器的变量
    user_intrrupt_frame.eax = 0;
    user_intrrupt_frame.ss = USER_DATA_SELECTOR;
    user_intrrupt_frame.cs = USER_CODE_SELECTOR;
    user_intrrupt_frame.ds = USER_DATA_SELECTOR;
    user_intrrupt_frame.es = USER_CODE_SELECTOR;
    user_intrrupt_frame.fs = USER_CODE_SELECTOR;
    user_intrrupt_frame.gs = USER_CODE_SELECTOR;
    // 用户栈的 esp 位置
    user_intrrupt_frame.esp = USER_STACK_TOP;
    user_intrrupt_frame.eip = (u32_t)real_init_thread;
    user_intrrupt_frame.eflags = (0 << 12 | 0b10 | 1 << 9);
    
    void* esp = &user_intrrupt_frame;
    // 把 user_intr_frame 赋值给 esp
    asm volatile(
        "movl %0, %%esp\n\t"
        "jmp switch_to_user_mode\n\t" ::"m"(esp));
}