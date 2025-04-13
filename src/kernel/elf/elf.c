#include <elf/elf.h>
#include <base/string.h>

void load_elf32_phdr(void* elf_data, Elf32_Phdr* elf_phdr) {
    u32_t p_offset = elf_phdr->p_offset;
    // 内存大小
    u32_t p_memsz = elf_phdr->p_memsz;
    // 文件大小
    u32_t p_filesz = elf_phdr->p_filesz;
    // 进程的虚拟地址
    void* p_vaddr = (void*)elf_phdr->p_vaddr;
    memset(p_vaddr, 0, p_memsz);
    // 程序头的内容开始位置
    void* elf_phdr_start = elf_data + p_offset;
    
    // 把数据拷过去
    memcpy(p_vaddr, elf_phdr_start, p_filesz);
}