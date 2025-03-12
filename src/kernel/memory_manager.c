#include <memory_manager.h>

#define ZONE_VALID 1 
#define ZONE_INVALID 2 

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