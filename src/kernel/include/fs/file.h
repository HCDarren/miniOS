#ifndef MINIOS_FILE_H
#define MINIOS_FILE_H

#include <os.h>
#include <drivers/device.h>

#define FILE_NAME_LENGTH 32

typedef enum file_type {
    // 普通磁盘文件
    FILE_MORMAL = 1,
    // 设备文件
    FILE_DEVICE,
} file_type_t;

typedef struct file {
    char name[FILE_NAME_LENGTH];
    // 文件类型
    file_type_t type;
    // 文件大小
    u32_t size;
    // 引用的次数
    u32_t reference_count;
    // 位置
    u32_t position; 
    // 文件权限，可读可写可执行
    u32_t permission;
    // 如果是设备文件，设备 id 就有值
    device_number_t device_number;
} file_t;

file_t* file_create();

void file_free(file_t* file);

void file_init();

#endif