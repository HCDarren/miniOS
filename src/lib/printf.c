#include <lib/include/printf.h>
#include <base/stdarg.h>
#include <lib/include/syscall.h>
#include <os.h>
static char buf[1024];

static u32_t write(fd_t fd, char *buf, u32_t len)
{
    return syscall_3(sys_write, fd, (u32_t)buf, len);
}

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

int printf(const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);

    i = vsprintf(buf, fmt, args);

    va_end(args);

    write(stdout, buf, i);

    return i;
}