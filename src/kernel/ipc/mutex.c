#include <ipc/mutex.h>
#include <interrupt.h>

// 初始化锁
void mutex_init(mutex_t* mutex) {
    mutex->owner = nullptr;
    list_init(&mutex->wait_list);
}

// 加锁
void mutex_lock(mutex_t* mutex) {
    bool state = enter_critical_protection();
    task_t* running_task = current_running_task();
    if (mutex->owner == NULL || mutex->owner == running_task) {
        mutex->owner = running_task;
    } else {
        list_add_tail(&mutex->wait_list, &running_task->list_node);
        set_task_block(running_task);
    }
    leave_critical_protection(state);
}

// 解锁
void mutex_unlock(mutex_t* mutex) {
    bool state = enter_critical_protection();
    task_t* running_task = current_running_task();
    if (mutex->owner == running_task) {
        mutex->owner = nullptr;
        if (!list_is_empty(&mutex->wait_list)) {
            list_node_t* list_node = list_remove_header(&mutex->wait_list);
            task_t *task = STRUCT_ADDR_BY_FILED_ADDR(list_node, list_node, task_t);
            set_task_ready(task);
        }
    } else {
        assert(false);
    }
    leave_critical_protection(state);
}