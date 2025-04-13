#include <lib/include/execve.h>
#include <lib/include/syscall.h>
/**
 * pathname：可执行文件路径
 * argv：参数列表（以NULL结尾）
 * envp：环境变量（以NULL结尾）
 */
u32_t execve(const char *pathname, char *const argv[], char *const envp[]) {
    syscall_3(sys_execve, (u32_t)pathname, (u32_t)argv, (u32_t)envp);
}