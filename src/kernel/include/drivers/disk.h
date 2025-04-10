#ifndef MINIOS_DISK_H
#define MINIOS_DISK_H

#include <os.h>

#define DISK_NAME_LENGTH 8
// 分区表的数量
#define PARTITION_NUMBER 4
// 识别命令
#define DISK_CMD_IDENTIFY    0xec
// 读命令
#define DISK_CMD_READ    0x24
// 写命令
#define DISK_CMD_WRITE   0x34

#define DISK_IOBASE_PRIMARY   0x1F0   // 主通道基地址
#define DISK_IOBASE_SECONDARY 0x170   // 从通道基地址

typedef struct
{
    u8_t bootable;                      // 引导标志
    u8_t start_head;                    // 分区起始磁头号
    u8_t start_sector : 6;              // 分区起始扇区号
    u16_t start_cylinder : 10;          // 分区起始柱面号
    u8_t partition_type;                // 分区类型字节
    u8_t end_head;                      // 分区的结束磁头号
    u8_t end_sector : 6;                // 分区结束扇区号
    u16_t end_cylinder : 10;            // 分区结束柱面号
    u32_t lba_start;                    // 分区起始物理扇区号 LBA
    u32_t sector_count;                 // 分区占用的扇区数
} __packed disk_partition_info_t;


// 分区信息
typedef struct disk_partition
{
    char name[8];              // 分区名称
    u32_t partition_type;      // 分区类型
    u32_t lba_start;               // 分区起始物理扇区号 LBA
    u32_t sector_count;               // 分区占用的扇区数           
} __packed disk_partition_t;

// 磁盘的类型主盘还是从盘
typedef enum disk_type {
    DISK_MASTER = 0,
    DISK_SLAVE,
} disk_type_t;

// 定义磁盘的结构体
typedef struct disk
{
    // sd0，sd1，sd2，sd3
    char name[DISK_NAME_LENGTH];
    disk_type_t disk_type;
    // io 端口的开始位置
    u16_t io_port_base;
    // lba 模式扇区的数量
    u32_t lba_size;
    // 分区表 4 个
    disk_partition_t partitiont_table[PARTITION_NUMBER];
} __packed disk_t;

void disk_init(void);

void disk_io_read(const disk_t* disk, u32_t lba_start, u32_t count, u16_t * buf, u32_t size);

#endif