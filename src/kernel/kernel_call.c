#include <kernel_call.h>
#include <interrupt.h>
#include <printk.h>
#include <base/assert.h>
#include <memory/memory_manager.h>
#include <drivers/device.h>
#include <task/task.h>
#include <fs/fs.h>
#include <lib/include/syscall.h>

// 系统调用方法表的大小
#define SYS_CALL_SIZE 64
// 系统调用方法表
sys_call_method_t sys_call_table[SYS_CALL_SIZE];

static void sys_call_default(exception_frame_t* exception_frame) {
    assert(false);
}

static void do_sys_open(exception_frame_t* exception_frame){
    exception_frame->eax = fs_open((const char*)exception_frame->ebx, exception_frame->ecx);
}

static void do_sys_dup(exception_frame_t* exception_frame){
    task_t* task = current_running_task();
    file_t* file = task_get_file(exception_frame->ebx);
    assert(file != NULL);
    fd_t new_fd = task_add_file(file);
    exception_frame->eax = new_fd;
}

static void do_sys_write(exception_frame_t* exception_frame){
    fs_write(exception_frame->ebx, (char*)exception_frame->ecx, exception_frame->edx);
}

// fork 进程
static void do_sys_fork(exception_frame_t* exception_frame){
    exception_frame->eax = task_fork();
}

// 待完善
static void do_sys_execve(exception_frame_t* exception_frame) {
    char* elf_name = (char*)exception_frame->ebx;
    // 跳到程序入口执行
    int res = task_execve(elf_name);
    exception_frame->eax = res;
}

// 获取进程id
static void do_sys_getpid(exception_frame_t* exception_frame){
    exception_frame->eax = current_running_task()->pid;
}

// 获取进程的pid
static void do_sys_getppid(exception_frame_t* exception_frame){
    exception_frame->eax = current_running_task()->ppid;
}

static void do_sys_sbrk(exception_frame_t* exception_frame){
    u32_t sbrk = current_running_task()->sbrk;
    current_running_task()->sbrk = sbrk + exception_frame->ebx;
    exception_frame->eax = sbrk;
}

static void do_sys_sleep(exception_frame_t* exception_frame) {
    task_sleep(exception_frame->ebx);
    exception_frame->eax = 0;
}

static void do_sys_exit(exception_frame_t* exception_frame) {
    task_exit();
}

static void do_sys_opendir(exception_frame_t* exception_frame) {
    
}

static void do_sys_read(exception_frame_t* exception_frame) {
    u32_t read_size = fs_read(exception_frame->ebx, (void*)exception_frame->ecx, exception_frame->edx);
    exception_frame->eax = read_size;
}

static void do_sys_close(exception_frame_t* exception_frame) {
    fs_close(exception_frame->ebx);
}

static void do_sys_readdir(exception_frame_t* exception_frame) {
    
}

static void do_sys_closedir(exception_frame_t* exception_frame) {
    
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
    sys_call_table[sys_open] = do_sys_open;
    sys_call_table[sys_write] = do_sys_write;
    sys_call_table[sys_fork] = do_sys_fork;
    sys_call_table[sys_getpid] = do_sys_getpid;
    sys_call_table[sys_getppid] = do_sys_getppid;
    sys_call_table[sys_sleep] = do_sys_sleep;
    sys_call_table[sys_exit] = do_sys_exit;
    sys_call_table[sys_execve] = do_sys_execve;
    sys_call_table[sys_dup] = do_sys_dup;
    sys_call_table[sys_opendir] = do_sys_opendir;
    sys_call_table[sys_readdir] = do_sys_readdir;
    sys_call_table[sys_closedir] = do_sys_closedir;
    sys_call_table[sys_read] = do_sys_read;
    sys_call_table[sys_close] = do_sys_close;
    sys_call_table[sys_sbrk] = do_sys_sbrk;
}

void syscall_init() { 
    // 注册 0x80 系统调用号
    register_interrupt_handler(INTERRUPT_SYS_CALL_NUMBER, interrupt_handler_syscall);
    // 初始化系统调用表
    init_sys_table();
}