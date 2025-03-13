#ifndef MINIOS_MEMORY_MANAGER_H
#define MINIOS_MEMORY_MANAGER_H

#include <os.h>
#include <printk.h>
#include <memory/bitmap.h>

// 内存管理分配器
typedef struct memory_manager_alloc
{
    // 内存起始位置
    void* addr_start;
    // 内存大小
    u32_t size;
    // 位图
    bitmap_t bitmap;
} memory_manager_alloc_t;

// 页目录映射的结构体
typedef struct page_mapping_dir
{
    u32_t present : 1;       // 0 (P) Present; must be 1 to map a 4-KByte page
    u32_t read_write : 1; // 1 (R/W) Read/write, if 0, writes may not be allowe
    u32_t user_mode_acc : 1; // 2 (U/S) if 0, user-mode accesses are not allowed t
    u32_t write_through : 1; // 3 (PWT) Page-level write-through
    u32_t cache_disable : 1; // 4 (PCD) Page-level cache disable
    u32_t accessed : 1;      // 5 (A) Accessed
    u32_t : 1;               // 6 Ignored;
    u32_t ps : 1;            // 7 (PS)
    u32_t : 4;               // 11:8 Ignored
    u32_t phy_pt_addr : 20;  // 高20位page table物理地址
} __packed page_mapping_dir_t;

// 页表映射目录的结构体
typedef struct page_mapping_table
{
    u32_t present : 1;        // 0 (P) Present; must be 1 to map a 4-KByte page
    u32_t read_write : 1;  // 1 (R/W) Read/write, if 0, writes may not be allowe
    u32_t user_mode_acc : 1;  // 2 (U/S) if 0, user-mode accesses are not allowed t
    u32_t write_through : 1;  // 3 (PWT) Page-level write-through
    u32_t cache_disable : 1;  // 4 (PCD) Page-level cache disable
    u32_t accessed : 1;       // 5 (A) Accessed;
    u32_t dirty : 1;          // 6 (D) Dirty
    u32_t pat : 1;            // 7 PAT
    u32_t global : 1;         // 8 (G) Global
    u32_t : 3;                // Ignored
    u32_t phy_page_addr : 20; // 高20位物理地址
} __packed page_mapping_table_t;

typedef struct memory_info
{
    // 内存基地址
    u64_t base;
    // 内存长度
    u64_t size;
    // 类型
    u32_t type;
} __packed memory_info_t;

// 申请分配物理页
void* alloc_a_page();
void* alloc_pages(int page_size);

// 内存释放
void free_a_page(void* addr);
void free_pages(void* addr, int free_size);

void memory_init();

#endif