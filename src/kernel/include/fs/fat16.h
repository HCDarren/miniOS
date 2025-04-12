#ifndef MINIOS_FAT16_H
#define MINIOS_FAT16_H

#include <os.h>
#include <drivers/disk.h>
#include <lib/dirent.h>
#include <fs/file.h>

/**
 * 目录项
 */
typedef struct {
    u8_t  Name[8];      // 文件名（8字节，不足补空格）
    u8_t  Ext[3];       // 扩展名（3字节，不足补空格）
    u8_t  Attr;         // 文件属性（见下文说明）
    u8_t  Reserved;      // 保留字段（通常为0）
    u8_t  CreateTimeMs;  // 创建时间的毫秒（实际为10ms单位）
    u16_t CreateTime;    // 创建时间（小时、分钟、秒）
    u16_t CreateDate;    // 创建日期（年、月、日）
    u16_t AccessDate;    // 最后访问日期
    u16_t ClusterHigh;   // 起始簇号高16位（FAT16中通常为0）
    u16_t ModifyTime;    // 最后修改时间
    u16_t ModifyDate;    // 最后修改日期
    u16_t ClusterLow;    // 起始簇号低16位
    u32_t FileSize;      // 文件大小（字节）
} __packed dir_entry_t;

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
    // 根目录在磁盘中的开始位置
    u32_t root_start_lba;  
    // 数据存放的起始位置
    u32_t data_start_lba; 
    // 块大小
    u32_t block_size; 
    // buf 放在这里面
    u16_t data_buf[256];
    u32_t buf_size;
} __packed fat16_t;

void fat16_init(disk_t* disk);

u32_t fat16_closedir(DIR* dir);

DIR* fat16_opendir(const char * path);

dirent* fat16_readdir(DIR* dir);

u32_t fat16_open(const char* path, file_t* file);

u32_t fat16_read(file_t* file, void* buf, u32_t size);

#endif