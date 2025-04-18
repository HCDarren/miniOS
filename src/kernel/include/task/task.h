#ifndef MINIOS_TASK_MANAGER_H
#define MINIOS_TASK_MANAGER_H
#include <os.h>
#include <task/list.h>
#include <fs/file.h>

#define FILE_DESCRIPTOR_TABLE_COUNT 64

// 计算 filed 属性在结构体中的偏移
#define FIELD_OFFSET_IN_STRUCT(STRUCT_NAME, FIELD_NAME) \
    (u32_t) & (((STRUCT_NAME *)0)->FIELD_NAME)

// 根据 file 的内存位置得到结构体的开始内存位置
#define STRUCT_ADDR_BY_FILED_ADDR(FIELD_ADDR, FIELD_NAME, STRUCT_NAME) \
    (STRUCT_NAME *)((u32_t)(FIELD_ADDR) - FIELD_OFFSET_IN_STRUCT(STRUCT_NAME, FIELD_NAME))

// 每个进程每次执行的默认时间片
#define TASK_DEFUALT_TICKS 10

#define TASK_NAME_SIZE 32

// 1G - 4G 给用户态用，后面可能运行 elf 文件要改，先这样吧。
#define USER_TASK_BASE 0x40000000 // 1G
// 用户栈顶 4G
#define USER_STACK_TOP 0x0

typedef enum user_type {
    // 内核用户
    KERNEL_USER,
    // 普通用户
    NORMAL_USER
} user_type_t;

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
    // 死亡 
    TASK_DIED,     
} task_state_t;

// pcb 进程控制块
typedef struct task_struct
{
    // 内核栈
    u32_t *stack;
    // 每个进程拥有自己的页目录表
    void* pde;
    // 每个进程打开的文件描述符表
    file_t* file_descriptor_table[FILE_DESCRIPTOR_TABLE_COUNT];
    // 取巧的方式放一个 list_node 用于进程管理
    list_node_t list_node;
    // 用于 bolck
    list_node_t block_list_node;
    // 用于 sleep 等待队列
    list_node_t wait_list_node;
    // 当前任务的状态
    task_state_t state;
    // 任务的名字
    char task_name[TASK_NAME_SIZE];
    // 进程优先级任务优先级
    u32_t priority;
    // 进程 pid
    u32_t pid;
    // 父进程 pid
    u32_t ppid;
    // 进程用户类型
    user_type_t user_type;
    // 剩余时间片
    u32_t ticks;
    // 当前进程获取到的时间片
    u32_t jiffies;
    // 睡眠结束的时间片
    u32_t sleep_stop_jiffies;
} __packed task_t;

// 进程管理
typedef struct task_manager
{
    // 正在运行的 task
    task_t* running_task;
    // 等待队列：睡眠等待，等待磁盘数据都可以放等待队列，还可以新增睡眠队列，后面再看吧
    list_t wait_list;
    // 阻塞队列
    list_t block_list;
    // 准备对列
    list_t ready_list;
    // 所有进程对列
    list_t all_task_list;
    // 进程僵尸队列，等待收尸
    list_t zombie_list;
} task_manager_t;

typedef struct task_frame
{
    u32_t edi;
    u32_t esi;
    u32_t ebx;
    u32_t ebp;
    void (*eip)(void);
} task_frame_t;

// 中断回到用户态的中断帧
typedef struct user_intrrupt_frame
{
    u32_t eax;
    u32_t ds;
    u32_t es;
    u32_t fs;
    u32_t gs;

    u32_t eip;
    u32_t cs;
    u32_t eflags;
    u32_t esp;
    u32_t ss;
} __packed user_intrrupt_frame_t;

void task_init();

// 进程睡眠：时间毫秒值
void task_sleep(u32_t sleep_time);

// 进程唤醒
void task_weakup();

// 主动交出 cpu 的控制权
void task_yield();

// 调度
void schedule();

// 获取当前正在运行的进程
task_t* current_running_task();

// 设置进程为 block
void set_task_block(task_t* task);

// 设置进程为 ready
void set_task_ready(task_t* task);

// fork 一个进程
pid_t task_fork();

// 进程退出
void task_exit();

// 添加一个文件返回一个文件描述符
fd_t task_add_file(file_t* file);

// 通过文件描述符获取文件
file_t* task_get_file(fd_t fd);

// 根据文件描述符移除一个文件
void task_remove_file(fd_t fd);

int task_execve(const char* file_name);

void pre_jmp_to_user_mode(void* eip);

#endif