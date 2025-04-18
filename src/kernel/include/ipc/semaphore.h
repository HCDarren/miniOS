#ifndef MINIOS_SEMAPHORE_H
#define MINIOS_SEMAPHORE_H

#include <os.h>
#include<task/list.h>

typedef struct 
{
    // 信号数量
    u32_t count;
    list_t wait_list;
} semaphore_t;

void sem_init(semaphore_t* sem, u32_t init_count);

// 等信号
void sem_wait(semaphore_t* sem);

// 发信号
void sem_notify(semaphore_t* sem);

#endif