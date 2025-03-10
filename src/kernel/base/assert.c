#include<base/assert.h>
#include<os.h>
#include<printk.h>

void assert_failure(const char *file, const char *func, int line, const char *expr){
    printk("[%s:%s:%d] assert(%s) failed!!!\r\n", file, func, line, expr);
    while (true) {
        // 停机，不消耗 cpu 了
        asm volatile("hlt\n");
    }  
}