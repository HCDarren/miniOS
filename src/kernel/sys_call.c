#include <sys_call.h>
#include <interrupt.h>
#include <printk.h>
#include <base/assert.h>
#include <memory/memory_manager.h>
#include <drivers/console.h>
#include <task/task_manager.h>

// 系统调用方法表的大小
#define SYS_CALL_SIZE 64
// 系统调用方法表
sys_call_method_t sys_call_table[SYS_CALL_SIZE];

static void sys_call_default(exception_frame_t* exception_frame) {
    assert(false);
}

static void do_sys_write(exception_frame_t* exception_frame){
    console_write((char*)exception_frame->ecx, exception_frame->edx);
}

// fork 进程
static void do_sys_fork(exception_frame_t* exception_frame){
    exception_frame->eax = task_fork();
}

// 待完善
static void do_sys_execve(exception_frame_t* exception_frame) {
    
}

// 获取进程id
static void do_sys_getpid(exception_frame_t* exception_frame){
    exception_frame->eax = current_running_task()->pid;
}

// 获取进程的pid
static void do_sys_getppid(exception_frame_t* exception_frame){
    exception_frame->eax = current_running_task()->ppid;
}

static void do_sys_sleep(exception_frame_t* exception_frame) {
    task_sleep(exception_frame->ebx);
    exception_frame->eax = 0;
}

static void do_sys_exit(exception_frame_t* exception_frame) {
    task_exit();
}

extern void interrupt_handler_syscall();

// 系统调用处理函数
void do_interrupt_handler_syscall(exception_frame_t* exception_frame) {
    assert(exception_frame->eax >= 0 && exception_frame->eax < SYS_CALL_SIZE);
    typedef void (*sys_call_handler_t)(exception_frame_t*);
    // 声明一个 `sys_call` 方法指针变量
    sys_call_handler_t sys_call = sys_call_table[exception_frame->eax];
    sys_call(exception_frame);
}

// 初始化系统调用表
static inline void init_sys_table() {
    for (size_t i = 0; i < SYS_CALL_SIZE; i++)
    {
        sys_call_table[i] = sys_call_default;
    }
    sys_call_table[sys_write] = do_sys_write;
    sys_call_table[sys_fork] = do_sys_fork;
    sys_call_table[sys_getpid] = do_sys_getpid;
    sys_call_table[sys_getppid] = do_sys_getppid;
    sys_call_table[sys_sleep] = do_sys_sleep;
    sys_call_table[sys_exit] = do_sys_exit;
    sys_call_table[sys_execve] = do_sys_execve;
}

void syscall_init() { 
    // 注册 0x80 系统调用号
    register_interrupt_handler(INTERRUPT_SYS_CALL_NUMBER, interrupt_handler_syscall);
    // 初始化系统调用表
    init_sys_table();
}

// 三个参数的系统调用
u32_t syscall_3(u32_t nr, u32_t arg1, u32_t arg2, u32_t arg3)
{
    u32_t ret;
    asm volatile(
        "int $0x80\n\t"
        : "=a"(ret)
        : "a"(nr), "b"(arg1), "c"(arg2), "d"(arg3));
    return ret;
}

// 两个参数的系统调用
u32_t syscall_2(u32_t nr, u32_t arg1, u32_t arg2)
{
    u32_t ret;
    asm volatile(
        "int $0x80\n\t"
        : "=a"(ret)
        : "a"(nr), "b"(arg1), "c"(arg2));
    return ret;
}

// 一个参数的系统调用
u32_t syscall_1(u32_t nr, u32_t arg1)
{
    u32_t ret;
    asm volatile(
        "int $0x80\n\t"
        : "=a"(ret)
        : "a"(nr), "b"(arg1));
    return ret;
}

// 0个参数的系统调用
u32_t syscall_0(u32_t nr)
{
    u32_t ret;
    asm volatile(
        "int $0x80\n\t"
        : "=a"(ret)
        : "a"(nr));
    return ret;
}