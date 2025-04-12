#include <fs/fat16.h>
#include <drivers/disk.h>
#include <printk.h>
#include <base/string.h>
#include <memory/memory_manager.h>

static disk_t* fat16_disk;
static fat16_t fat16;

// 格式化输出文件/文件夹名称
static inline void format_file_name(char format_name[], char name[], char ext[]){
    for (u32_t i = 0; i < 8; i++)
    {
        if (name[i] == ' ') {
            break;
        }
        *format_name++ = name[i];
    }
    if (ext[0] == ' ') {
        return;
    }
    *format_name++ = '.';
    for (u32_t i = 0; i < 3; i++)
    {
        *format_name++ = ext[i];
    }
    *format_name = '\0';
}

// 文件系统的测试代码都写这里面了
static inline void fat16_test_code() {
    /*
    // 读取根目录下面的文件信息
    u16_t buf[256] = {0};
    disk_io_read(fat16_disk, 132, 1, buf, sizeof(buf));
    dir_entry_t* dir_entry = (dir_entry_t*)buf;
    for (size_t i = 0; i < sizeof(buf) / sizeof(dir_entry_t); i++, dir_entry++)
    {
        if (dir_entry->Name[0] == 0x0) {
            break;
        }
        char format_name[13] = {0};
        format_file_name(format_name, dir_entry->Name, dir_entry->Ext);
        printk("name: %s\r\n", format_name); 
        printk("attr: 0x%x\r\n", dir_entry->Attr); 
    }
    */
}

void fat16_init(disk_t* disk) {
    fat16_disk = disk;
    // 读取基础数据在第一个扇区
    u16_t buf[512] = {0};
    disk_io_read(fat16_disk, 0, 1, buf, sizeof(buf));
    dbr_t* dbr = (dbr_t*)buf;
    // 设置 fat16 的基础数据
    fat16.fat_start_lba = dbr->BPB_RsvdSecCnt;
    fat16.fat_count = dbr->BPB_NumFATs;
    fat16.fat_sectors = dbr->BPB_FATSz16;
    fat16.bytes_per_sector = dbr->BPB_BytsPerSec;
    fat16.sectors_per_bolck = dbr->BPB_SecPerClus;
    fat16.root_file_count = dbr->BPB_RootEntCnt;
    fat16.root_start_lba = fat16.fat_start_lba + (fat16.fat_count * fat16.fat_sectors);
    fat16.data_start_lba = fat16.root_start_lba + fat16.root_file_count * 32 / dbr->BPB_BytsPerSec;
    fat16.block_size = fat16.sectors_per_bolck * fat16.bytes_per_sector;
}

u32_t fat16_read(file_t* file, void* buf, u32_t size) {
    u32_t sector = fat16.data_start_lba + (file->c_block - 2) * fat16.sectors_per_bolck;
    // 一次应该读一个块大小，不知道为啥，我这里不能这么搞
    u16_t data_buf[256];
    disk_io_read(fat16_disk, sector, 1, data_buf, sizeof(data_buf));
    for (size_t i = 0; i < size / 2; i++)
    {
        memcpy(buf, data_buf, size);
    }
    return file->size;
}

u32_t fat16_open(const char* path, file_t* file) {
    u16_t buf[256];
    disk_io_read(fat16_disk, fat16.root_start_lba, 1, buf, sizeof(buf));
    dir_entry_t* dir_entry = (dir_entry_t*)buf;
    for (size_t i = 0; i < fat16.root_file_count; i++, dir_entry++)
    {
        if (dir_entry->Name[0] == 0x0) {
            break;
        }
        char format_name[13] = {0};
        format_file_name(format_name, dir_entry->Name, dir_entry->Ext);
        if (strcmp(format_name, path) == 0) {
            file->c_block = ((dir_entry->ClusterHigh << 16) | dir_entry->ClusterLow);
            file->size = dir_entry->FileSize;
            return OK;
        }
    }
    return EOF;
}

// 时间精力有限，文件系统我不打算花很多时间，sql 数据库我们倒是可以再仔细花一些时间
u32_t fat16_closedir(DIR* dir) {
    return 0;
}

DIR* fat16_opendir(const char * path) {
    return NULL;
}

dirent* fat16_readdir(DIR* dir) {
    return NULL;
}