#include <gdt.h>
#include <base/string.h>
#include <base/assert.h>
#include <memory/memory_manager.h>
gdt_descriptor_t gdt_table[GDT_TABLE_SIZE] = {0}; 
// 内核全局描述符表指针
gdt_descriptor_pointer_t gdt_ptr;
// 所有任务共用一个 tss
tss_t tss;

static void init_tss() {
    memset(&tss, 0, sizeof(tss_t));
    // 主要是为了设置 ss0 用于进内核优先级切换
    tss.ss0 = KERNEL_DATA_SELECTOR;
    tss.io_base = sizeof(tss_t);
    gdt_descriptor_t * gdt_descriptor = &gdt_table[KERNEL_TSS_INDEX];
    // 设置 base 、limit
    u32_t base = (u32_t)&tss;
    u32_t limit = sizeof(tss_t)-1;
    gdt_descriptor->base_low = base & 0xffffff;
    gdt_descriptor->base_high = (base >> 24) & 0xff;
    gdt_descriptor->limit_low = limit & 0xffff;
    gdt_descriptor->limit_high = (limit >> 16) & 0xf;
    // 系统段
    gdt_descriptor->segment_type = 0;    
    gdt_descriptor->granularity = 0; 
    gdt_descriptor->big = 0;         
    gdt_descriptor->long_mode = 0; 
    // 在内存中  
    gdt_descriptor->present = 1;
    // 用于任务门或调用门     
    gdt_descriptor->DPL = 0;  
    // 32 位可用 tss       
    gdt_descriptor->type = 0b1001;   
    asm volatile("ltr %%ax\n" ::"a"(KERNEL_TSS_SELECTOR));
}

// 安装初始化用户进程的描述符
static void init_user_gdt() {
    gdt_descriptor_t * kernel_code_gdt_descriptor = &gdt_table[KERNEL_CODE_INDEX];
    gdt_descriptor_t * user_code_gdt_descriptor = &gdt_table[USER_CODE_INDEX];
    memcpy(user_code_gdt_descriptor, kernel_code_gdt_descriptor, sizeof(gdt_descriptor_t));
    user_code_gdt_descriptor->DPL = 3;

    gdt_descriptor_t * kernel_data_gdt_descriptor = &gdt_table[KERNEL_DATA_INDEX];
    gdt_descriptor_t * user_data_gdt_descriptor = &gdt_table[USER_DATA_INDEX];
    memcpy(user_data_gdt_descriptor, kernel_data_gdt_descriptor, sizeof(gdt_descriptor_t));
    user_data_gdt_descriptor->DPL = 3;
}

void gdt_init() {
    // 1、先获取到之前设置的 gdt 数据
    asm volatile("sgdt gdt_ptr");
    // 2、拷贝到新的 gdt 表中
    memcpy(gdt_table, (void*)gdt_ptr.base, gdt_ptr.limit + 1);
    // 3、重新设置 gdt
    gdt_ptr.base = (u32_t)&gdt_table;
    gdt_ptr.limit = sizeof(gdt_table) - 1;
    // 4、后面没用的都标记为可以用
    int available_start = 3;
    for (size_t i = available_start; i < GDT_TABLE_SIZE; i++)
    {
        gdt_table[i].available = 1;
    }
    asm volatile("lgdt gdt_ptr");
    init_tss();
    init_user_gdt();
}

int search_spare_descriptor() {
    for (size_t i = 0; i < GDT_TABLE_SIZE; ++i) {
        if(gdt_table[i].available == 1) {
            // 标记为占用
            gdt_table[i].available = 0;
            return i;
        }
    }
    return -1;
}

void free_descriptor(int selector) {
    gdt_table[selector].available == 0;
}