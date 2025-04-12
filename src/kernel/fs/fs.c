#include <fs/fs.h>
#include <fs/file.h>
#include <task/task.h>
#include <base/string.h>
#include <drivers/device.h>
#include <drivers/disk.h>

// 文件系统初始话
void fs_init() {
    // 磁盘系统的初始化后面还要改，先放这里
    device_manager_init();
    disk_init();
    file_init();
}

// 文件系统打开文件
int fs_open(const char* file_name, const u32_t flags) {
    file_t* file = file_create();
    int fd = task_add_file(file);
    strcpy(file->name, file_name);
    file->permission = flags;
    file->reference_count = 1;
    file->position = 0;
    // 控制台设备
    if (strcmp(file_name, "/dev/console") == 0) {
        file->type = FILE_DEVICE;
        file->device_number = DEVICE_CONSOLE;
        return fd;
    }
    // 普通文件设备
    file->type = FILE_MORMAL;
    file->device_number = DEVICE_DISK;
    device_t* device = device_find(file->device_number);
    // 打开文件
    bool is_ok = device->open(file_name, file);
    if (is_ok == OK) {
        return fd;
    }
    file_free(file);
    return EOF;
}

int fs_write(const u32_t fd, const char* buf, const u32_t len) {
    file_t* file = task_get_file(fd);
    if (file->type == FILE_DEVICE) { // 设备文件
        device_t* device = device_find(file->device_number);
        device->write(buf, len);
        return len;
    } else {
        // 普通磁盘文件
        device_t* device = device_find(file->device_number);
        device->write(buf, len);
        return len;
    }
    return EOF;
}

u32_t fs_read(const fd_t fd, void* buf, const u32_t size) {
    file_t* file = task_get_file(fd);
    device_t* device = device_find(file->device_number);
    return device->read(file, buf, size);
}

u32_t fs_close(const fd_t fd) {
    file_t* file = task_get_file(fd);
    file_free(file);
    task_remove_file(fd);
    return 0;
}

int fs_closedir(DIR* dir) {
    device_t* disk_device = device_find(DEVICE_DISK);
    assert(disk_device != NULL);
    return disk_device->closedir(dir);
}

DIR* fs_opendir(const char * path) {
    device_t* disk_device = device_find(DEVICE_DISK);
    assert(disk_device != NULL);
    return disk_device->opendir(path);
}

dirent* fs_readdir(DIR* dir) {
    device_t* disk_device = device_find(DEVICE_DISK);
    assert(disk_device != NULL);
    return disk_device->readdir(dir);
}