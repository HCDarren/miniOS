#include <memory/memory_manager.h>

// 内存可用区域，为 1 可用
#define ZONE_VALID 1 

// 默认页大小一般 4K
#define PAGE_SIZE 0x1000    
// 内存刚开始可用的起始位置，就从 1M 开始不搞那么麻烦了
#define MEMORY_VALID_BASE 0x100000 

// 前 4M 给我们的内核用，可以自行设计分配，比如我之前熟悉的 Android 操作系统 1G 给操作系统用，3G 给用户应用

void memory_init()
{
    // 内存检测信息数据的存储位置在 0xfffc
    u32_t memory_detect_ards = *((u32_t *)0xfffc);
    u32_t count = *(u32_t *)memory_detect_ards;
    memory_info_t *memory_info_ptr = (memory_info_t *)(memory_detect_ards + 4);
    for (size_t i = 0; i < count; i++ , memory_info_ptr++)
    {
        if (memory_info_ptr->type == ZONE_VALID) {
            printk("valid zone: 0x%x, len: %d \n\r", (u32_t)memory_info_ptr->base, (u32_t)memory_info_ptr->size);
        }  
    }


}