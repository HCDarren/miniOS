#include <task/task_manager.h>
#include <os.h>
#include <base/assert.h>
#include <gdt.h>
#include <tss.h>
#include <printk.h>
#include <interrupt.h>
#include <task/list.h>

#define PAGE_SIZE 0x1000

task_t *a = (task_t *)0x1000;
task_t *b = (task_t *)0x2000;

extern void task_switch(task_t *next);

task_t *running_task()
{
    asm volatile(
        "movl %esp, %eax\n"
        "andl $0xfffff000, %eax\n");
}

void schedule()
{
    task_t *current = running_task();
    task_t *next = current == a ? b : a;
    if (current != a && current != b) {
        task_switch(next);
        return;
    }
    --current->ticks;
    current->jiffies++;
    if (current->ticks == 0) {
        current->ticks = TASK_DEFUALT_TICKS;
        task_switch(next);
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

void task_init()
{
    task_create(a, thread_a);
    task_create(b, thread_b);

    list_t list;
    list_init(&list);

    list_node_t list_node[5];
    for (size_t i = 0; i < 5; i++)
    {
       list_add_header(&list, &list_node[i]);
       printk("add list_node: 0x%x\r\n", &list_node[i]);
    }
    
    for (size_t i = 0; i < 6; i++)
    {
        list_node_t* list_node = list_remove_tail(&list);
        printk("remove list_node: %x\r\n", list_node);
    }
}