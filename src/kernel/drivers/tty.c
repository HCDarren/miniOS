#include <drivers/tty.h>
#include <os.h>
#include <drivers/device.h>
#include <lib/include/string.h>
#include <lib/include/sleep.h>
#include <ipc/semaphore.h>

static tty_char_queue_t tty_char_queue;
static semaphore_t semaphore;

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

static void tty_get(char* c) {
    // 等待键盘输入字符
    sem_wait(&semaphore);
    if (tty_char_queue.write - tty_char_queue.read == 0) {
        return;
    }
    *c = *tty_char_queue.read;
    tty_char_queue.read++;
    if (tty_char_queue.read > tty_char_queue.end) {
        tty_char_queue.read = tty_char_queue.buf;
    }
}

void tty_put(const char c) {
    // 需要做一些特殊判断
    if (semaphore.count >= TTY_CHAR_BUF_SIZE) {
        // 丢掉
        return;
    }
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

    // 写完通知
    sem_notify(&semaphore);
}

// 读设备方法指针
static inline u32_t tty_read(void * file, void *buf, const u32_t count) {
    if (count <= 0) {
        return ERR;
    }
    u32_t len = 0;
    char* c_buf = buf;
    while (len++ < count)
    {
        char ch;
        tty_get(&ch);
        switch (ch)
        {
        case '\n':
            return len;
        case '\b':
            if (len > 0) {
                len--;
                buf--;
            }
        default:
            tty_write(&ch, 1);
            *c_buf++ = ch;
            break;
        }
    }
    // 键盘驱动读数据
    return len;
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
    sem_init(&semaphore, 0);
}