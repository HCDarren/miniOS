#include <shell/include/main.h>
#include <lib/include/printf.h>
#include <lib/include/sleep.h>

#define SHELL_PROMPT "darrenzeng@DARRENZENG-MB4 ~ %"

int main(int argc, char** argv) {
    // 输出一些基本信息
    printf("Welcome to the miniOS......\r\n");
    printf("version: 0.1\r\n");
    printf("date: 2025/04/13\r\n");
    // 不断的执行命令操作
    for (;;)
    {
       printf(SHELL_PROMPT);
       sleep(50000);
    }
    return 0;
}