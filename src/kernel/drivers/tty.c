#include <drivers/tty.h>
#include <os.h>
#include <drivers/device.h>
#include <base/string.h>
#include <lib/include/sleep.h>

// 读设备方法指针
static inline u32_t tty_read(void * file, const void *buf, const u32_t count) {
    if (count <= 0) {
        return ERR;
    }
    strcpy(buf, "Hello Test");
    // 键盘驱动读数据
    return OK;
}

// 写设备方法指针
static inline u32_t tty_write(const void *buf, const u32_t count) {
    device_t* device = device_find(DEVICE_CONSOLE);
    return device->write(buf, count);
}

static device_t tty_device;
static inline void install_tty_device() {
    device_t* device = &tty_device;
    strcpy(device->device_name, "/tty");
    device->device_type = DEVICE_CHAR;
    device->device_number = DEVICE_TTY;
    device->write = tty_write;
    device->ioctl = NULL;
    device->read = tty_read;
    device->open = NULL;
    device->seek = NULL;
    device->readdir = NULL;
    device->opendir = NULL;
    device->closedir = NULL;
    device_install(device);
}

void tty_init() {
    install_tty_device();
}