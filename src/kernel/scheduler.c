#include <scheduler.h>
#include <os.h>
#include <base/assert.h>
#include <gdt.h>
#include <tss.h>

void init_task(task_t* task, u32_t* entry, u32_t* esp) {
    assert(task != NULL);
}