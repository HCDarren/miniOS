#include <task/task.h>
#include <task/idle_task.h>
#include <task/init_task.h>
#include <base/assert.h>
#include <base/string.h>
#include <gdt.h>
#include <printk.h>
#include <interrupt.h>
#include <time.h>
#include <task/list.h>
#include <memory/memory_manager.h>
#include <base/asm_instruct.h>
#include <ipc/mutex.h>

static task_manager_t task_manager;

// 系统总的运行时间片
static u32_t jiffies = 0;

static task_t* idle_task;

static mutex_t mutex;

static u32_t task_pid_index = 0;

extern void task_switch(task_t *next);

// 从内核态切到用户态
extern void switch_to_user_mode();

extern tss_t tss;

void task_yield()
{
    // 从对头拿出来一个
    list_node_t *ready_node = list_header(&task_manager.ready_list);
    assert(ready_node != nullptr);
    task_t *ready_task = STRUCT_ADDR_BY_FILED_ADDR(ready_node, list_node, task_t);
    assert(((u32_t)ready_task & 0x07) == 0);
    // 切过去
    task_manager.running_task = ready_task;
    task_manager.running_task->state = TASK_RUNNING;
    // 用户进程的特殊处理
    if (ready_task->user_type == NORMAL_USER)
    {
        tss.esp0 = (u32_t)ready_task + PAGE_SIZE;
        set_cr3(ready_task->pde);
    }
    task_switch(ready_task);
}

void schedule()
{
    jiffies++;
    // 处理等待队列
    task_weakup();
    
    task_t* running_task = task_manager.running_task;

    if (running_task != NULL) {
        --running_task->ticks;
        running_task->jiffies++;
        if (running_task->ticks > 0) {
            return;
        }
        running_task->ticks = TASK_DEFUALT_TICKS;
        bool is_removed = list_remove(&task_manager.ready_list, &running_task->list_node);
        set_task_ready(running_task);
    }

    task_yield();
}

static void task_create(task_t *task, void* target, u32_t user_type)
{
    u32_t stack = (u32_t)task + PAGE_SIZE;

    stack -= sizeof(task_frame_t);
    task_frame_t *frame = (task_frame_t *)stack;
    frame->ebx = frame->esi = frame->edi = frame->ebp = 0x00000000;
    frame->eip = (void *)target;
    task->stack = (u32_t *)stack;
    task->jiffies = 0;
    task->user_type = user_type;
    task->pid = task_pid_index++;
    task->ppid = 0;
    task->priority = 0;
    task->ticks = TASK_DEFUALT_TICKS;
    memset(task->file_descriptor_table, 0, sizeof(task->file_descriptor_table));
}

void init_task_manager() {
    list_init(&task_manager.all_task_list);
    list_init(&task_manager.wait_list);
    list_init(&task_manager.ready_list);
    task_manager.running_task = NULL;
    list_init(&task_manager.zombie_list);
    mutex_init(&mutex);
}

// 从内核态切到用户态
extern void switch_to_user_mode();

void task_init()
{
    idle_task = alloc_a_page();
    task_create(idle_task, idle_task_work, KERNEL_USER);
    // 创建 init_task 准备进入用户态
    task_t * init_task = alloc_a_page();
    // 拷贝一个页目录表给 init_task
    init_task->pde = copy_pde();
    task_create(init_task, init_thread, NORMAL_USER);

    init_task_manager();
    list_add_tail(&task_manager.ready_list, &idle_task->list_node);
    list_add_tail(&task_manager.ready_list, &init_task->list_node);
}

// 进程睡眠：时间毫秒值
void task_sleep(u32_t sleep_time) {
    mutex_lock(&mutex);
    task_t *running_task = current_running_task();
    u32_t sleep_jiffies = sleep_time / JIFFY;
    // 最少一个时间片
    running_task->sleep_stop_jiffies = sleep_jiffies <= 0 ? (jiffies + 1) : (jiffies + sleep_jiffies);
    running_task->state = TASK_SLEEPING;
    list_add_tail(&task_manager.wait_list, &running_task->wait_list_node);
    mutex_unlock(&mutex);
    set_task_block(running_task);
}

// 进程唤醒
void task_weakup() {
    if (list_is_empty(&task_manager.wait_list)) {
        return;
    }
    list_node_t* wait_node = list_header(&task_manager.wait_list);
    while (wait_node && wait_node != &task_manager.wait_list.tail)
    {
        task_t *wait_task = STRUCT_ADDR_BY_FILED_ADDR(wait_node, wait_list_node, task_t);
        if (jiffies >= wait_task->sleep_stop_jiffies) {
            // 加到就绪队列的尾部
            list_remove(&task_manager.wait_list, wait_node);
            set_task_ready(wait_task);
        }
        wait_node = wait_node->next;
    }
}

// 获取当前正在运行的进程
task_t* current_running_task() {
    return task_manager.running_task;
}

// 设置当前进程为 block
void set_task_block(task_t* task) {
    task->state == TASK_BLOCKED;
    bool is_removed = list_remove(&task_manager.ready_list, &task->list_node);
    task->ticks = 1;
    task_yield();
}

// 设置当前进程为 ready
void set_task_ready(task_t* task) {
    task->state == TASK_READY;
    list_add_tail(&task_manager.ready_list, &task->list_node);
}

void task_init_new_fork(task_t* fork_task, task_t* parent_task) {
    // 1、直接从父进程拷贝
    memcpy(fork_task, parent_task, PAGE_SIZE);
    // 2、设置 fork 进程的 pid 和 ppid
    fork_task->pid = task_pid_index++;
    fork_task->ppid = parent_task->pid;
    fork_task->state = TASK_READY;
    // 3、拷贝页目录表
    fork_task->pde = copy_pde();

    // 3. 返回用户态数据准备
    u32_t stack = (u32_t)fork_task + PAGE_SIZE;
    stack -= sizeof(user_intrrupt_frame_t);
    exception_frame_t* exception_frame = (exception_frame_t*)(tss.esp0 - sizeof(exception_frame_t));
    user_intrrupt_frame_t* user_intrrupt_frame = (user_intrrupt_frame_t*)stack;
    user_intrrupt_frame->eax = 0;
    user_intrrupt_frame->ss = exception_frame->ss;
    user_intrrupt_frame->cs = exception_frame->cs;
    user_intrrupt_frame->ds = exception_frame->ds;
    user_intrrupt_frame->es = exception_frame->es;
    user_intrrupt_frame->fs = exception_frame->fs;
    user_intrrupt_frame->gs = exception_frame->gs;
    user_intrrupt_frame->esp = exception_frame->uesp;
    user_intrrupt_frame->eip = exception_frame->eip;
    user_intrrupt_frame->eflags = exception_frame->eflags;

    // 4. 内核栈数据构造
    stack -= sizeof(task_frame_t);
    task_frame_t *frame = (task_frame_t *)stack;
    frame->edi = exception_frame->edi;
    frame->esi = exception_frame->esi;
    frame->ebx = exception_frame->ebx;
    frame->ebp = exception_frame->ebp;
    frame->eip = switch_to_user_mode;
    fork_task->stack = (u32_t *)stack;
}

pid_t task_fork() {
    mutex_lock(&mutex);

    task_t* current_task = current_running_task();
    // 创建一个新的进程
    task_t * new_task = alloc_a_page();
    task_init_new_fork(new_task, current_task);
    list_add_tail(&task_manager.ready_list, &new_task->list_node);

    mutex_unlock(&mutex);
    return new_task->pid;
}

// 进程退出
void task_exit() {
    task_t* current_task = current_running_task();
    current_task->state == TASK_DIED;
    bool is_removed = list_remove(&task_manager.ready_list, &current_task->list_node);
    assert(is_removed == true);
    // TODO: 释放页目录，页目录和进程私有进程的物理内存也要释放
    free_a_page(current_task);
    task_yield();
}

fd_t task_add_file(file_t* file) {
    assert(file != nullptr);
    fd_t fd = 0;
    mutex_lock(&mutex);

    task_t* task = current_running_task();

    for (u32_t i = 0; i < FILE_DESCRIPTOR_TABLE_COUNT; i++)
    {
        if (task->file_descriptor_table[i] == NULL) {
            fd = i;
            task->file_descriptor_table[i] = file;
            break;
        }
    }
    
    mutex_unlock(&mutex);

    assert(fd >=0 && fd < FILE_DESCRIPTOR_TABLE_COUNT);

    return fd;
}

file_t* task_get_file(u32_t fd) {
    assert(fd >=0 && fd < FILE_DESCRIPTOR_TABLE_COUNT);
    return current_running_task()->file_descriptor_table[fd];
}

void task_remove_file(u32_t fd) {
    assert(fd >=0 && fd < FILE_DESCRIPTOR_TABLE_COUNT);
    assert(current_running_task()->file_descriptor_table[fd] != NULL);
    current_running_task()->file_descriptor_table[fd] = NULL;
}