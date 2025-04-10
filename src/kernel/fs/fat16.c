#include <fs/fat16.h>
#include <drivers/disk.h>

static disk_t* fat16_disk;

static fat16_t fat16;

void fat16_init(disk_t* disk) {
    fat16_disk = disk;
    // 读取基础数据在第一个扇区
    u16_t buf[256];
    disk_io_read(fat16_disk, 0, 1, buf, sizeof(buf));
    dbr_t* dbr = (dbr_t*)buf;
    // 设置 fat16 的基础数据
    fat16.fat_start_lba = dbr->BPB_RsvdSecCnt;
    fat16.fat_count = dbr->BPB_NumFATs;
    fat16.fat_sectors = dbr->BPB_FATSz16;
    fat16.bytes_per_sector = dbr->BPB_BytsPerSec;
    fat16.sectors_per_bolck = dbr->BPB_SecPerClus;
    fat16.root_file_count = dbr->BPB_RootEntCnt;
}

u32_t fat16_closedir(DIR* dir) {
    
}

DIR* fat16_opendir(const char * path) {

}

dirent* fat16_readdir(DIR* dir) {

}