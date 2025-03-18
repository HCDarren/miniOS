#include <task/task_manager.h>
#include <os.h>
#include <base/assert.h>
#include <gdt.h>
#include <tss.h>
#include <printk.h>
#include <interrupt.h>
#include <task/list.h>
#include <memory/memory_manager.h>

#define PAGE_SIZE 0x1000

static task_manager_t task_manager;

extern void task_switch(task_t *next);

void schedule()
{
    list_node_t* running_node = list_header(&task_manager.running_list);
    list_node_t* ready_node = list_header(&task_manager.ready_list);
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

u32_t thread_a()
{
    open_cpu_interrupt();
    while (true)
    {
        printk("A");
    }
}

u32_t thread_b()
{
    open_cpu_interrupt();
    while (true)
    {
        printk("B");
    }
}

u32_t thread_c()
{
    open_cpu_interrupt();
    while (true)
    {
        printk("C");
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
    task_t * a = alloc_a_page();
    task_create(a, thread_a);
    task_t * b = alloc_a_page();
    task_create(b, thread_b);
    task_t * c = alloc_a_page();
    task_create(c, thread_c);
    init_task_manager();
    list_add_tail(&task_manager.ready_list, &a->list_node);
    list_add_tail(&task_manager.ready_list, &b->list_node);
    list_add_tail(&task_manager.ready_list, &c->list_node);
}