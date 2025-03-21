#ifndef MINIOS_MUTEX_H
#define MINIOS_MUTEX_H

#include<task/task_manager.h>
#include<task/list.h>

// 简单互斥量
typedef struct mutex
{
    // 同一个进程允许反复进来
    task_t* owner;
    // 等待队列
    list_t wait_list;
} mutex_t;

// 初始化锁
void mutex_init(mutex_t* mutex);

// 加锁
void mutex_lock(mutex_t* mutex);

// 解锁
void mutex_unlock(mutex_t* mutex);

#endif