#include <gdt.h>
#include <base/string.h>
#include <base/assert.h>
gdt_descriptor_t gdt_table[GDT_TABLE_SIZE] = {0}; 
gdt_descriptor_pointer_t gdt_ptr;          // 内核全局描述符表指针

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