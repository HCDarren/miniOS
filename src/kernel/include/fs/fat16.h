#ifndef MINIOS_FAT16_H
#define MINIOS_FAT16_H

#include <os.h>
#include <drivers/disk.h>
#include <lib/dirent.h>

typedef struct {
    u8_t BS_jmpBoot[3];
    u8_t BS_OEMName[8];
    u16_t BPB_BytsPerSec;               // 每扇区字节数
    u8_t BPB_SecPerClus;                // 每簇扇区数
    u16_t BPB_RsvdSecCnt;               // 保留区扇区数
    u8_t BPB_NumFATs;                   // FAT表项数
    u16_t BPB_RootEntCnt;               // 根目录项目数
    u16_t BPB_TotSec16;                 // 总的扇区数( < 32M)
    u8_t BPB_Media;
    u16_t BPB_FATSz16;                  // 每个FAT表的扇区数
    u16_t BPB_SecPerTrk;
    u16_t BPB_NumHeads;
    u32_t BPB_HiddSec;
    u32_t BPB_TotSec32;                 // 总的扇区数( > 32M)
} __packed dbr_t;

// fat16 文件系统本身信息
typedef struct {
    // FAT表起始扇区号
    u32_t fat_start_lba;                     
    // FAT表数量
    u32_t fat_count;                         
    // 每个FAT表的扇区数
    u32_t fat_sectors;                       
    // 每扇区大小
    u32_t bytes_per_sector;                     
    // 每块占几个扇区
    u32_t sectors_per_bolck;                   
    // 根目录文件的数量
    u32_t root_file_count;                        
} __packed fat16_t;

void fat16_init(disk_t* disk);

u32_t fat16_closedir(DIR* dir);

DIR* fat16_opendir(const char * path);

dirent* fat16_readdir(DIR* dir);

#endif