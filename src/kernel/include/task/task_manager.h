#ifndef MINIOS_TASK_MANAGER_H
#define MINIOS_TASK_MANAGER_H
#include <tss.h>
#include <os.h>
#include <task/list.h>

// 计算 filed 属性在结构体中的偏移
#define FIELD_OFFSET_IN_STRUCT(STRUCT_NAME, FIELD_NAME) \
    (u32_t) & (((STRUCT_NAME *)0)->FIELD_NAME)

// 根据 file 的内存位置得到结构体的开始内存位置
#define STRUCT_ADDR_BY_FILED_ADDR(FIELD_ADDR, FIELD_NAME, STRUCT_NAME) \
    (STRUCT_NAME *)((u32_t)(FIELD_ADDR) - FIELD_OFFSET_IN_STRUCT(STRUCT_NAME, FIELD_NAME))

// 每个进程每次执行的默认时间片
#define TASK_DEFUALT_TICKS 100

// pcb 进程控制块
typedef struct task_struct
{
    // 内核栈
    u32_t *stack;
    // 取巧的方式放一个 list_node 用于增删改查
    list_node_t list_node;
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