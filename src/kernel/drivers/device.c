#include <drivers/device.h>
#include <base/assert.h>

#define DEVICE_NUMBER 128
// 只能安装 128 个设备
static device_t* devices[DEVICE_NUMBER] = {0};

// 设备管理初始化
void device_manager_init() {
    // 可以把安装代码写到这里面

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