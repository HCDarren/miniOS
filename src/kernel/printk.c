#include <printk.h>
#include <stdarg.h>
#include <console.h>

static char buf[1024];

static char* number(char str[], int num) {
    int len = 0;
    char tmp[36];
    const char *digits = "0123456789";

    if (num == 0) {
        tmp[len++] = '0';
    } else {
        while (num > 0) {
            int index = num % 10;
            num /= 10;
            tmp[len++] = digits[index];
        }
    }
    
    while (len-- > 0) {
        *str++ = tmp[len];
    }
    return str;
}

static int vsprintf(char buf[], const char *format, va_list args) {
    char *str;

    for (str = buf; *format; ++format) {
        // 不是 % 的默认处理方式
        if (*format != '%')
        {
            *str++ = *format;
            continue;
        }

        // 含有百分号的方式，第一个百分号要过掉，读取后面的类型
        ++format;

        switch(*format) {
            case 'd':
                int num = va_arg(args, int);
                str = number(str, num);
            break;
            case 's':
                char* s = va_arg(args, char *);
                while (*s != '\0')
                {
                    *str++ = *s++;
                }
            break;

            // ... 其他后面有遇到再去实现
        }
    }
    return str - buf;;
}



void printk(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int str_len = vsprintf(buf, format, args);
    va_end(args);
    console_write(buf, str_len);
}