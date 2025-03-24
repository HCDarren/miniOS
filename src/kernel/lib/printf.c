#include <lib/printf.h>
#include <base/stdarg.h>
#include <printk.h>
#include <sys_call.h>
#include <os.h>
static char buf[1024];

static u32_t write(fd_t fd, char *buf, u32_t len)
{
    return syscall3(SYS_NR_WRITE, fd, (u32_t)buf, len);
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