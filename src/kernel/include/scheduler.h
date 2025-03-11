#ifndef MINIOS_SCHEDER_H
#define MINIOS_SCHEDER_H
#include <tss.h>
#include <os.h>

// pcb 进程控制块
typedef struct task_struct {
    tss_t tss;
    int tss_selector;
} __packed task_t;


void init_task(task_t* task, u32_t* entry, u32_t* esp);

#endif