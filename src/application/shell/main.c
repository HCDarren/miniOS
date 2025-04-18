#include <shell/include/main.h>
#include <lib/include/printf.h>
#include <lib/include/sleep.h>
#include <lib/include/stdio.h>

#define SHELL_PROMPT "\r\ndarrenzeng@DARRENZENG-MB4: "

// 从tty设备读取
char shell_data_buf[1024];

int main(int argc, char** argv) {
    // 输出一些基本信息
    printf("Welcome to the miniOS......\r\n");
    printf("version: 0.1\r\n");
    printf("date: 2025/04/13\r\n");
    // 不断的执行命令操作
    for (;;)
    {
       printf(SHELL_PROMPT);
       fgets(stdin, shell_data_buf, sizeof(shell_data_buf));
       printf("read commond: %s", shell_data_buf);
       // 执行命令
    }
    return 0;
}
