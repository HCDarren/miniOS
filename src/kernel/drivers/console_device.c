#include <drivers/console_device.h>
#include <drivers/device.h>
#include <drivers/console.h>
#include <base/string.h>

static device_t console_device;

int console_device_write(const void *buf, const u32_t count) {
    console_write(buf, count);
    return 0;
}

void install_console_device() {
    device_t* device = &console_device;
    
    strcpy(device->device_name, "console");
    device->device_type = DEVICE_CHAR;
    device->device_number = DEVICE_CONSOLE;
    device->write = console_device_write;
    device->ioctl = NULL;
    device->read = NULL;

    device_install(device);
}