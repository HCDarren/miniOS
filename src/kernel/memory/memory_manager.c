#include <memory/memory_manager.h>
#include <base/string.h>
#include <base/assert.h>
#include <base/asm_instruct.h>

// 内存可用区域，为 1 可用
#define ZONE_VALID 1

// 默认页大小一般 4K
#define PAGE_SIZE 0x1000
// 内存刚开始可用的起始位置，就从 1M 开始不搞那么麻烦了
#define MEMORY_VALID_BASE 0x100000
// 内核内存占用大小
#define KERNEL_MEMORY_SIZE 4 * 1024 * 1024

// 根据虚拟地址获取页目录的索引，最前面 10 位是页目录索引
#define PDE_INDEX(virtual) ((u32_t)virtual >> 22)
// 根据虚拟地址获取二级页表的索引，中间 10 位是页表索引（21～12）
#define PTE_INDEX(virtual) (((u32_t)virtual >> 12) & 0x3FF)

// 按 y 对其去掉尾数
#define DOWN_ON(x, y) (x) & ~((y) - 1)
// 按 y 对其往上加
#define UP_ON(x, y) ((x) + (y) - 1) & ~((y) - 1)

// 内存分配器和位图就放 0x7c00 就是我们原来我们 boot 的位置，反正现在没啥用了
static memory_manager_alloc_t* memory_manager_alloc;


// 设置 cr0 寄存器
// 最高位 PE 置为 1，启用分页
static inline void open_cr0_enable_page()
{
    // 0b1000_0000_0000_0000_0000_0000_0000_0000
    // 0x80000000
    asm volatile(
        "movl %cr0, %eax\n\t"
        "orl $0x80000000, %eax\n\t"
        "movl %eax, %cr0\n\t");
}

// 设置 cr3 寄存器
// 参数是页目录的地址
static inline void set_cr3(u32_t pde)
{
    asm volatile("movl %%eax, %%cr3\n" ::"a"(pde));
}

void* alloc_a_page() {
    alloc_pages(1);
}

void* alloc_pages(int page_size){
    int free_index = bitmap_scan_alloc(&memory_manager_alloc->bitmap, page_size);
    if (free_index == EOF) {
        return NULL;
    }
    void* addr = memory_manager_alloc->addr_start + (free_index * PAGE_SIZE);
    return addr;
}

void free_a_page(void* addr) {
    free_pages(addr, 1);
}

void free_pages(void* addr, int free_count){
    int free_index = (addr - memory_manager_alloc->addr_start) / PAGE_SIZE;
    bitmap_free(&memory_manager_alloc->bitmap, free_index, free_count);
}

/**
 * 查找并创建二级映射表
 */
static page_mapping_table_t* find_create_page_mapping_table(page_mapping_dir_t *page_dir, void *virtual_addr) {
    page_mapping_dir_t* target_page_dir = (page_dir + PDE_INDEX(virtual_addr));
    page_mapping_table_t* target_page_table = nullptr;
    if (target_page_dir->present) {
        // 存在
        target_page_table = (page_mapping_table_t*)(target_page_dir->phy_pt_addr << 12);
    } else {
        // 不存在创建页表
        target_page_table = alloc_a_page();
        memset(target_page_table, 0 , PAGE_SIZE);
        target_page_dir->present = 1;
        target_page_dir->read_write = 1;
        target_page_dir->user_mode_acc = 1;
        target_page_dir->phy_pt_addr = (u32_t)target_page_table >> 12;
    }
    return target_page_table + PTE_INDEX(virtual_addr);
}

/**
 * 创建一个内存映射
 * page_dir 页目录表
 * virtual_addr 虚拟地址
 * physics_addr 真实的物理地址
 * count 连续映射多长
 */
static void create_memory_mapping(page_mapping_dir_t *page_dir, void *virtual_addr, void *physics_addr, u32_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        page_mapping_table_t *page_mapping_table = find_create_page_mapping_table(page_dir, virtual_addr);
        printk("0x%x, 0x%x, 0x%x\r\n", page_mapping_table, virtual_addr, physics_addr);
        page_mapping_table->phy_page_addr = ((u32_t)physics_addr >> 12);
        assert(page_mapping_table->present == 0);
        page_mapping_table->present = 1;
        page_mapping_table->read_write = 1;
        page_mapping_table->user_mode_acc = 1;
        virtual_addr += PAGE_SIZE;
        physics_addr += PAGE_SIZE;
    }
}

// 前 4M 给我们的内核用，可以自行设计分配，比如我之前熟悉的 Android 操作系统 1G 给操作系统用，3G 给用户应用
static inline void init_kernel_mapping()
{
    page_mapping_dir_t* page_table_dir = alloc_a_page();
    memset(page_table_dir, 0, PAGE_SIZE);
    create_memory_mapping(page_table_dir, nullptr, nullptr,  KERNEL_MEMORY_SIZE / PAGE_SIZE);
    set_cr3((u32_t)page_table_dir);
    open_cr0_enable_page();
}

// 初始化内存管理分配器
static void init_memory_manager_alloc(memory_manager_alloc_t *memory_manager_alloc,
    u8_t *bits, void* memory_start, u32_t memory_size)
{
    memory_manager_alloc->addr_start = memory_start;
    memory_manager_alloc->size = memory_size;
    bitmap_init(&memory_manager_alloc->bitmap, bits, memory_size / PAGE_SIZE);
}

void memory_init()
{
    // 内存检测信息数据的存储位置在 0xfffc
    u32_t memory_detect_ards = *((u32_t *)0xfffc);
    u32_t count = *(u32_t *)memory_detect_ards;
    memory_info_t *memory_info_ptr = (memory_info_t *)(memory_detect_ards + 4);
    // 最大内存的起始位置
    void* max_memory_start = 0;
    int max_memory_size = 0;
    for (size_t i = 0; i < count; i++, memory_info_ptr++)
    {
        if (memory_info_ptr->type == ZONE_VALID)
        {
            if (memory_info_ptr->size > max_memory_size) {
                max_memory_start = (void*)memory_info_ptr->base;
                max_memory_size = memory_info_ptr->size;
                printk("max_memory_start: 0x%x, max_memory_size: %d\r\n", max_memory_start, max_memory_size);
            }
        }
    }
    // 1M 后面的内存我们都自己来管理，这里是开了上帝视角，qemu 的 1M-32M 是我们配置的
    memory_manager_alloc = (memory_manager_alloc_t*)0x7c00;
    u8_t *bits = (u8_t *)(memory_manager_alloc + sizeof(memory_manager_alloc_t));
    init_memory_manager_alloc(memory_manager_alloc, bits, max_memory_start, max_memory_size);
    
    // 这里可以再加一些各种检测、通用代码。这里规划是写死了，0M - 4M 给内核了，而且必须要 1-1 匹配，也就是虚拟地址如果访问是 1M 物理地址也要是 1M 的位置
    init_kernel_mapping();
}