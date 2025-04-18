#include <ipc/semaphore.h>
#include<task/task.h>
#include<base/assert.h>
#include <interrupt.h>

void sem_init(semaphore_t* sem, u32_t init_count) {
    assert(sem != NULL);
    sem->count = init_count;
    list_init(&sem->wait_list);
}

void sem_wait(semaphore_t* sem) {
    assert(sem != NULL);
    bool state = enter_critical_protection();
    if (sem->count > 0) {
        sem->count--;
    } else {
        task_t* task = current_running_task();
        set_task_block(task);
        list_add_tail(&sem->wait_list, &task->block_list_node);
        task_yield();
    }
    leave_critical_protection(state);
}

void sem_notify(semaphore_t* sem) {
    assert(sem != NULL);
    bool state = enter_critical_protection();
    if (!list_is_empty(&sem->wait_list)) {
        list_node_t* task_node = list_remove_header(&sem->wait_list);
        task_t* task = STRUCT_ADDR_BY_FILED_ADDR(task_node, block_list_node, task_t);
        set_task_ready(task);
    } else {
        sem->count++;
    }
    leave_critical_protection(state);
}
