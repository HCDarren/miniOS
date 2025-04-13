extern main
extern exit
global _start
_start:
    ; 这里应该还有一些 c 的环境代码
    call main ; 主函数
    call exit ; 程序退出