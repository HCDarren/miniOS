#ifndef APPLICATION_SHELL_MAIN_H
#define APPLICATION_SHELL_MAIN_H

// 命令行的输入大小
#define COMMOND_INPUT_SIZE 2048

typedef struct 
{
    // 命令的名称
    char* name;
    // 命令的执行函数，带有两个参数
    int (*do_execute)(int argc, char** argv);
} commond_t;
 

#endif