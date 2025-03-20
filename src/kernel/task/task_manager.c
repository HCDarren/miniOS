#include <task/task_manager.h>
#include <os.h>
#include <base/assert.h>
#include <gdt.h>
#include <tss.h>
#include <printk.h>
#include <interrupt.h>
#include <time.h>
#include <task/list.h>
#include <memory/memory_manager.h>
#include <base/asm_instruct.h>

#define PAGE_SIZE 0x1000

static task_manager_t task_manager;

// 系统总的运行时间片
static u32_t jiffies = 0;

extern void task_switch(task_t *next);

void schedule()
{
    jiffies++;
    // 处理等待队列
    task_weakup();
    list_node_t* running_node = list_header(&task_manager.running_list);
    list_node_t* ready_node = list_header(&task_manager.ready_list);

    if (ready_node == NULL) {
        return;
    }
    
    task_t *ready_task = STRUCT_ADDR_BY_FILED_ADDR(ready_node, list_node, task_t);

    if (running_node != nullptr) {
        task_t *running_task = STRUCT_ADDR_BY_FILED_ADDR(running_node, list_node, task_t);
        --running_task->ticks;
        running_task->jiffies++;
        if (running_task->ticks == 0) {
            running_task->ticks = TASK_DEFUALT_TICKS;
            list_remove_header(&task_manager.ready_list);
            list_remove_header(&task_manager.running_list);
            list_add_tail(&task_manager.ready_list, &running_task->list_node);
            list_add_tail(&task_manager.running_list, &ready_task->list_node);
            task_switch(ready_task);
        }
    } else {
        list_remove_header(&task_manager.ready_list);
        list_add_tail(&task_manager.running_list, &ready_task->list_node);
        task_switch(ready_task);
    }
}

u32_t idle_task_work()
{
    open_cpu_interrupt();
    while (true)
    {
       hlt();
    }
}

u32_t thread_c()
{
    int i = 0;
    printk("--->");
    open_cpu_interrupt();
    while (true)
    {
        printk("C: %d\r\n", jiffies);
        printk("D: %d\r\n", jiffies);
        // 睡眠 1S 中
        task_sleep(1000);
        printk("E: %d\r\n", jiffies);
    }
}

static void task_create(task_t *task, void* target)
{
    u32_t stack = (u32_t)task + PAGE_SIZE;

    stack -= sizeof(task_frame_t);
    task_frame_t *frame = (task_frame_t *)stack;
    frame->ebx = 0x11111111;
    frame->esi = 0x22222222;
    frame->edi = 0x33333333;
    frame->ebp = 0x44444444;
    frame->eip = (void *)target;

    task->stack = (u32_t *)stack;
    task->jiffies = 0;
    task->priority = 0;
    task->ticks = TASK_DEFUALT_TICKS;
}

void init_task_manager() {
    list_init(&task_manager.all_task_list);
    list_init(&task_manager.wait_list);
    list_init(&task_manager.ready_list);
    list_init(&task_manager.running_list);
    list_init(&task_manager.zombie_list);
}

void task_init()
{
    task_t * idle_task = alloc_a_page();
    task_create(idle_task, idle_task_work);
    task_t * c = alloc_a_page();
    task_create(c, thread_c);
    init_task_manager();
    list_add_tail(&task_manager.ready_list, &idle_task->list_node);
    list_add_tail(&task_manager.ready_list, &c->list_node);
}

// 进程睡眠：时间毫秒值
void task_sleep(u32_t sleep_time) {
    list_node_t* running_node = list_remove_header(&task_manager.running_list);
    task_t *running_task = STRUCT_ADDR_BY_FILED_ADDR(running_node, list_node, task_t);
    u32_t sleep_jiffies = sleep_time / JIFFY;
    // 最少一个时间片
    running_task->sleep_stop_jiffies =  sleep_jiffies <= 0 ? (jiffies + 1) : (jiffies + sleep_jiffies);
    // 这里最好是一个排序插入，先简单写了，时间最短的在最前面
    list_add_tail(&task_manager.wait_list, running_node);
    // 直接执行调度，交出 cpu 控制权
    schedule();
}

// 进程唤醒
void task_weakup() {
    list_node_t* wait_node = list_header(&task_manager.wait_list);
    task_t *wait_task = STRUCT_ADDR_BY_FILED_ADDR(wait_node, list_node, task_t);
    if (jiffies >= wait_task->sleep_stop_jiffies) {
        // 加到就绪队列的尾部
        list_remove_header(&task_manager.wait_list);
        list_add_tail(&task_manager.ready_list, wait_node);
    }
}