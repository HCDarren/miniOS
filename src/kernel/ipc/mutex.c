#include <ipc/mutex.h>
#include <interrupt.h>

// 初始化锁
void mutex_init(mutex_t* mutex) {
    mutex->owner = nullptr;
    list_init(&mutex->wait_list);
}

// 尝试获取锁（非阻塞版本）
static bool mutex_trylock(mutex_t* mutex) {
    assert(mutex != NULL);
    bool state = enter_critical_protection();
    bool acquired = false;
    task_t* current = current_running_task();
    if (mutex->owner == NULL) {
        mutex->owner = current;
        acquired = true;
    } else if (mutex->owner == current) {
        acquired = true;
    }
    leave_critical_protection(state);
    return acquired;
}

// 加锁
void mutex_lock(mutex_t* mutex) {
    assert(mutex != NULL);
    while (!mutex_trylock(mutex))
    {
        bool state = enter_critical_protection();
        task_t* current = current_running_task();
        
        // 确保在添加到等待列表前再次检查锁状态
        if (mutex->owner == NULL || mutex->owner == current) {
            leave_critical_protection(state);
            continue;
        }
        
        list_add_tail(&mutex->wait_list, &current->block_list_node);
        set_task_block(current);
        task_yield();
        leave_critical_protection(state);
    }
}

// 解锁
void mutex_unlock(mutex_t* mutex) {
    assert(mutex != NULL);
    bool state = enter_critical_protection();
    task_t* current = current_running_task();
    // 有异常
    if (mutex->owner != current) {
        leave_critical_protection(state);
        assert(false && "Mutex unlock not owned by current task");
        return;
    }
    mutex->owner = NULL;
    // 唤醒等待队列中的第一个任务
    if (!list_is_empty(&mutex->wait_list)) {
        list_node_t* node = list_remove_header(&mutex->wait_list);
        task_t* task = STRUCT_ADDR_BY_FILED_ADDR(node, block_list_node, task_t);
        set_task_ready(task);
    }
    
    leave_critical_protection(state);
}