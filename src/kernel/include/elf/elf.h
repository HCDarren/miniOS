#ifndef MINIOS_ELF_H
#define MINIOS_ELF_H
#include <os.h>

// ELF相关数据类型
typedef u32_t Elf32_Addr;
typedef u16_t Elf32_Half;
typedef u32_t Elf32_Off;
typedef u32_t Elf32_Word;

// 32 位 ELF Header
typedef struct {
    unsigned char e_ident[16];  // 魔数 + 字节序 + ELF 版本等
    Elf32_Half      e_type;       // 文件类型（如可执行、共享库）
    Elf32_Half      e_machine;    // 目标架构（如 x86、ARM）
    Elf32_Word      e_version;    // 版本号
    Elf32_Word      e_entry;      // 程序入口地址
    Elf32_Word      e_phoff;      // Program Header 表偏移
    Elf32_Word      e_shoff;      // Section Header 表偏移
    Elf32_Word      e_flags;      // 处理器特定标志
    Elf32_Half      e_ehsize;     // ELF Header 大小
    Elf32_Half      e_phentsize;  // Program Header 条目大小
    Elf32_Half      e_phnum;      // Program Header 条目数量
    Elf32_Half      e_shentsize;  // Section Header 条目大小
    Elf32_Half      e_shnum;      // Section Header 条目数量
    Elf32_Half      e_shstrndx;   // Section Header 字符串表索引
} Elf32_Ehdr;

typedef struct {
    Elf32_Word p_type;
    Elf32_Off p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} Elf32_Phdr;


void load_elf32_phdr(void* elf_data, Elf32_Phdr* elf_phdr);

#endif