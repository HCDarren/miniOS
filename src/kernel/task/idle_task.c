#include <task/idle_task.h>
#include <task/task_manager.h>
#include <base/asm_instruct.h>
#include <interrupt.h>

u32_t idle_task_work()
{
    open_cpu_interrupt();
    while (true)
    {
       current_running_task()->ticks = 1; 
       hlt();
    }
    return 0;
}