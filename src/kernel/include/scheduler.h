#ifndef MINIOS_SCHEDER_H
#define MINIOS_SCHEDER_H
#include <tss.h>
#include <os.h>

// pcb 进程控制块
typedef struct task_struct
{
    u32_t *stack; // 内核栈
    u32_t ticks;               // 剩余时间片
    u32_t jiffies;             // 上次执行时全局时间片
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

void init_task(task_t* task, u32_t* entry, u32_t* esp);

// 调度
void schedule();

#endif