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

#define TASK_NAME_SIZE 32

typedef enum task_state
{
    // 初始化
    TASK_INIT,
    // 运行     
    TASK_RUNNING, 
    // 就绪 
    TASK_READY, 
    // 阻塞  
    TASK_BLOCKED,
    // 睡眠  
    TASK_SLEEPING, 
    // 等待
    TASK_WAITING, 
    // 死亡 
    TASK_DIED,     
} task_state_t;

// pcb 进程控制块
typedef struct task_struct
{
    // 内核栈
    u32_t *stack;
    // 取巧的方式放一个 list_node 用于进程管理
    list_node_t list_node;
    // 当前任务的状态
    task_state_t state;
    // 任务的名字
    char task_name[TASK_NAME_SIZE];
    // 进程优先级任务优先级
    u32_t priority;
    // 进程 pid
    u32_t pid;
    // 进程 uid
    u32_t uid;
    // 剩余时间片
    u32_t ticks;
    // 当前进程获取到的时间片
    u32_t jiffies;
} task_t;

// 进程管理
typedef struct task_manager
{
    // 正在运行的队列
    list_t running_list;
    // 等待队列：睡眠等待，等待磁盘数据都可以放等待队列，还可以新增睡眠队列，后面再看吧
    list_t wait_list;
    // 准备对列
    list_t ready_list;
    // 所有进程对列
    list_t all_task_list;
    // 进程僵尸队列，等待收尸
    list_t zombie_list;
} task_manager_t;

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