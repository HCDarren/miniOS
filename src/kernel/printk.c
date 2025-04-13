#include <printk.h>
#include <base/stdarg.h>
#include <base/assert.h>
#include <drivers/console.h>
#include <drivers/device.h>
#include <interrupt.h>

static char buf[1024];

void printk(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int str_len = vsprintf(buf, format, args);
    va_end(args);

    device_t* console_device = device_find(DEVICE_CONSOLE);
    assert(console_device != nullptr && console_device->write != NULL);
    console_device->write(buf, str_len);
}

int sprintf(char *buf, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int str_len = vsprintf(buf, format, args);
    return str_len;
}