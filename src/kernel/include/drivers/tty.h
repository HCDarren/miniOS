#ifndef MINIOS_TTY_H
#define MINIOS_TTY_H

// tty 设备字符的大小
#define TTY_CHAR_BUF_SIZE 1024

// tty 的字符队列
typedef struct 
{
    // 数据缓冲
    char buf[TTY_CHAR_BUF_SIZE];
    char* read;
    char* write;
    char* end;
} tty_char_queue_t;


void tty_init();

void tty_put(const char c);

#endif