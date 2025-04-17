#include <drivers/tty.h>
#include <os.h>
#include <drivers/device.h>
#include <base/string.h>
#include <lib/include/sleep.h>

static tty_char_queue_t tty_char_queue;

static inline void init_tty_queue() {
    tty_char_queue.read = tty_char_queue.buf;
    tty_char_queue.write = tty_char_queue.buf;
    tty_char_queue.end = tty_char_queue.buf + TTY_CHAR_BUF_SIZE;
}

// 写设备方法指针
static inline u32_t tty_write(const void *buf, const u32_t count) {
    device_t* device = device_find(DEVICE_CONSOLE);
    return device->write(buf, count);
}

void tty_put(const char c) {
    tty_write(&c, 1);
    // 需要做一些特殊判断

    *tty_char_queue.write++ = c;
    // write 和 read 不能碰面，否则要扩容，这里不管了
    // 到最前面
    if (tty_char_queue.write > tty_char_queue.end) {
        tty_char_queue.write = tty_char_queue.buf;
    }
    // 到最前面
    if (tty_char_queue.read > tty_char_queue.end) {
        tty_char_queue.read = tty_char_queue.buf;
    }
}

int tty_get(char* c) {
    // 要用新号和信号量了

    if (tty_char_queue.write - tty_char_queue.read == 0) {
        return 0;
    } 
    *c = *tty_char_queue.read;
    tty_char_queue.read++;
    if (tty_char_queue.read > tty_char_queue.end) {
        tty_char_queue.read = tty_char_queue.buf;
    }
}

// 读设备方法指针
static inline u32_t tty_read(void * file, const void *buf, const u32_t count) {
    if (count <= 0) {
        return ERR;
    }
    strcpy(buf, "Hello Test");
    // 键盘驱动读数据
    return OK;
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
    init_tty_queue();
}