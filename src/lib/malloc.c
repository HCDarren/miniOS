#include <lib/include/malloc.h>
#include <lib/include/unistd.h>

// 指向最后一块动态内存
static memory_control_block_t* g_top = NULL;

#define MCB_SIZE sizeof(memory_control_block_t)

void* malloc(size_t size) {
    // 第一次申请 33 页
    // 用完了不够后续都按页分配
    // 要加锁，多线程问题，自旋锁就可以，自旋锁待实现
    // 我们实现的跟官方提供的很多不一样，简单但是不高效

    // 1. 先去找合适的大小
    memory_control_block_t* mcb = NULL;
    for (mcb = g_top; mcb != NULL; mcb = mcb->prev)
    {
        if (mcb->is_free && mcb->size >= size) {
            break;
        }
    }
    
    if (mcb == NULL) {
        // 2. 没有找到空闲的，注意这里没有用
        mcb = sbrk(size + MCB_SIZE);
        mcb->size = size;
        mcb->prev = g_top;
        g_top = mcb;
    }
    mcb->is_free = false;
    // 返回的是
    return mcb + 1;
}

// 不打算写
void* remalloc(void* ptr, size_t size) {

}

void free(void* ptr) {
    if (ptr == NULL) {
        return;
    }
    memory_control_block_t* mcb = (memory_control_block_t*)ptr - 1;
    mcb->is_free = true;
}