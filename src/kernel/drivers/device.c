#include <drivers/device.h>
#include <base/assert.h>
#include <drivers/console.h>
#include <drivers/disk.h>
#include <drivers/tty.h>
#include <base/string.h>
#include <fs/fat16.h>
#include <fs/file.h>

#define DEVICE_NUMBER 128
// 只能安装 128 个设备
static device_t* devices[DEVICE_NUMBER] = {0};

/************* console_device start ************/
static device_t console_device;
static inline u32_t console_device_write(const void *buf, const u32_t count) {
    console_write(buf, count);
    return 0;
}

static inline void install_console_device() {
    console_init();

    device_t* device = &console_device;
    
    strcpy(device->device_name, "/dev/console");
    device->device_type = DEVICE_CHAR;
    device->device_number = DEVICE_CONSOLE;
    device->write = console_device_write;
    device->ioctl = NULL;
    device->read = NULL;

    device_install(device);
}
/************* console_device end **************/


/************* disk device start ************/

// 读设备方法指针
static inline u32_t disk_read(void * file, const void *buf, const u32_t count) {
    return fat16_read((file_t*)file, buf, count);
}

// 写设备方法指针
static inline u32_t disk_write(const void *buf, const u32_t count) {
    return 0;
}
// 打开设备方法指针
static inline u32_t disk_open(const char* path, const void* file) {
    return fat16_open(path, (file_t*)file);
}
// seek 设备方法指针
static inline u32_t disk_seek(const u32_t offset) {
    return 0;
}

static inline u32_t disk_closedir(DIR* dir) {
    return fat16_closedir(dir);
}

static inline DIR* disk_opendir(const char * path) {
    return fat16_opendir(path);
}

static inline dirent* disk_readdir(DIR* dir) {
    return fat16_readdir(dir);
}

static device_t disk_device;
static inline void install_disk_device() {
    device_t* device = &disk_device;
    
    strcpy(device->device_name, "/home");
    device->device_type = DEVICE_BLOCK;
    device->device_number = DEVICE_DISK;
    device->write = disk_write;
    device->ioctl = NULL;
    device->read = disk_read;
    device->open = disk_open;
    device->seek = disk_seek;
    device->readdir = disk_readdir;
    device->opendir = disk_opendir;
    device->closedir = disk_closedir;

    device_install(device);
}

/************* disk device end **************/

// 设备管理初始化
void device_manager_init() {
    // 可以把安装代码写到这里面
    install_console_device();
    install_disk_device();
    tty_init();
}

// 设备安装
int device_install(device_t* device) {
    assert(device != nullptr);
    for (size_t i = 0; i < DEVICE_NUMBER; i++)
    {
        if (devices[i] == nullptr) {
            devices[i] = device;
            return 0;
        }
    }
    return EOF;
}

// 设备查找
device_t* device_find(device_number_t device_number) {
    for (size_t i = 0; i < DEVICE_NUMBER; i++)
    {
        if (devices[i] != nullptr && devices[i]->device_number == device_number) {
            return devices[i];
        }
    }
    assert(false);
    return NULL;
}