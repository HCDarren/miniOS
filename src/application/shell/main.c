#include <shell/include/main.h>
#include <lib/include/printf.h>
#include <lib/include/sleep.h>
#include <lib/include/stdio.h>
#include <lib/include/string.h>
#include <lib/include/malloc.h>

#define SHELL_PROMPT "darrenzeng@DARRENZENG-MB4: "

// 从tty设备读取缓冲区大小
char shell_data_buf[1024];

static int do_help(int argc, char** argv){
    printf("\r\nls: List the detailed information of the file.\r\n");
    printf("cat: Read the content of the file.");
}

static int do_cat(int argc, char** argv){
    printf("\r\ncat: do_cat");
}

static int do_ls(int argc, char** argv){
    printf("\r\ndo_ls: do_ls");
}

commond_t commond_list[COMMOND_INPUT_SIZE] = {
    {
        .name="help", 
        .do_execute = do_help,
    },
    {
        .name="cat", 
        .do_execute = do_cat,
    },
    {
        .name="ls", 
        .do_execute = do_ls,
    },
};

static void do_execute_commond() {
    if (shell_data_buf[0] == '\0') {
        return;
    }
    int argc = 0;
    // 最多 10 个，每个长度 128
    char argv[10][128] = {0};
    char* start = shell_data_buf;
    char* argv_s = argv[0];
    while (*start != '\0')
    {
        if (*start == ' ') {
            argc++;
            argv_s = argv[argc];
            start++;
        } else {
            *argv_s++ = *start++;
        }
    }
    // 匹配
    for (u32_t i = 0; i < COMMOND_INPUT_SIZE; i++)
    {
        commond_t commond = commond_list[i];
        if (strcmp(argv[0], commond.name) == 0)
        {
            commond.do_execute(argc, (char**)argv[1]);
            return;
        }
    }
    // 不识别的命令
    printf("\r\nzsh: command not found:%s", argv[0]);
}

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
       // 解析参数执行命令
       do_execute_commond();
       printf("\r\n");
       memset(shell_data_buf, 0, sizeof(shell_data_buf));
    }
    return 0;
}
