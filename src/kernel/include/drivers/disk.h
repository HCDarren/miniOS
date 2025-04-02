#ifndef MINIOS_DISK_H
#define MINIOS_DISK_H

#include <os.h>

#define DISK_NAME_LENGTH 32
// 识别命令
#define DISK_CMD_IDENTIFY    0xec
// 读命令
#define DISK_CMD_READ    0x24
// 写命令
#define DISK_CMD_WRITE   0x34

#define DISK_IOBASE_PRIMARY   0x1F0   // 主通道基地址
#define DISK_IOBASE_SECONDARY 0x170   // 从通道基地址

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
    int lba_size;
} __packed disk_t;

void disk_init(void);

#endif