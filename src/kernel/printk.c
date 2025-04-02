#include <printk.h>
#include <base/stdarg.h>
#include <base/assert.h>
#include <drivers/console.h>
#include <drivers/device.h>
#include <interrupt.h>

static char buf[1024];

static char* digits[] = {"","","","","","","","","01234567","","0123456789","","","","","","0123456789ABCDEF"};

static char *number(char str[], int num, int scale)
{
    int len = 0;
    char tmp[36];
    const char *digit = digits[scale];

    if (num == 0)
    {
        tmp[len++] = '0';
    }
    else
    {
        while (num > 0)
        {
            int index = num % scale;
            num /= scale;
            tmp[len++] = digit[index];
        }
    }

    while (len-- > 0)
    {
        *str++ = tmp[len];
    }
    return str;
}

int vsprintf(char buf[], const char *format, va_list args)
{
    char *str;
    int num = 0;
    for (str = buf; *format; ++format)
    {
        // 不是 % 的默认处理方式
        if (*format != '%')
        {
            *str++ = *format;
            continue;
        }

        // 含有百分号的方式，第一个百分号要过掉，读取后面的类型
        ++format;

        switch (*format)
        {
        case 'c':
            *str++ = (unsigned char)va_arg(args, int);
        break;
        case 'd':
            num = va_arg(args, int);
            str = number(str, num, 10);
            break;
        case 'x':
            num = va_arg(args, int);
            str = number(str, num, 16);
            break;
        case 's':
            char *s = va_arg(args, char *);
            while (*s != '\0')
            {
                *str++ = *s++;
            }
            break;

            // ... 其他后面有遇到再去实现
        }
    }
    return str - buf;
    ;
}

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