#include <lib/printf.h>
#include <base/stdarg.h>
#include <printk.h>
#include <sys_call.h>
#include <os.h>
static char buf[1024];

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