#ifndef MINIOS_TASK_MANAGER_H
#define MINIOS_TASK_MANAGER_H
#include <tss.h>
#include <os.h>

// 每个进程每次执行的默认时间片
#define TASK_DEFUALT_TICKS 100

// pcb 进程控制块
typedef struct task_struct
{
    // 内核栈
    u32_t *stack; 
    // 进程优先级任务优先级
    u32_t priority;            
    // 剩余时间片
    u32_t ticks;   
    // 进程获取到的时间片   
    u32_t jiffies;  
               
} task_t;

typedef struct task_frame_t
{
    u32_t edi;
    u32_t esi;
    u32_t ebx;
    u32_t ebp;
    void (*eip)(void);
} task_frame_t;

void task_init();

// 调度
void schedule();

#endif