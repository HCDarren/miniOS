#ifndef MINIOS_DEVICE_H
#define MINIOS_DEVICE_H
#include <os.h>

#define DEVICE_NAME_LENGTH 32

// 设备类型
typedef enum device_type
{
    // 字符设备：键盘、控制台
    DEVICE_CHAR = 1,
    // 块设备：磁盘、软盘
    DEVICE_BLOCK,
} device_type_t;

// 设备号
typedef enum device_number
{
    // 控制台
    DEVICE_CONSOLE = 1,
    // 键盘
    DEVICE_KEYBOARD,
} device_number_t;

// 抽象设备
typedef struct device
{
    char device_name[DEVICE_NAME_LENGTH];
    // 设备类型
    device_type_t device_type;
    // 设备号
    device_number_t device_number;
    // 设备控制方法指针
    int (*ioctl)(const int cmd, const void *args, const int flags);
    // 读设备方法指针
    int (*read)(const void *buf, const u32_t count);
    // 写设备方法指针
    int (*write)(const void *buf, const u32_t count);
} device_t;

// linux 一切皆文件的思想，所以统一接口抽象成设备文件，方法，安装，读，写，控制

// 设备管理初始化
void device_manager_init();

// 设备安装
int device_install(device_t *device);

// 设备查找
device_t* device_find(device_number_t device_number);

#endif