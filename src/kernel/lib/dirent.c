#include <lib/dirent.h>
#include <sys_call.h>

int closedir(DIR dir) {
    return 0;
}

DIR opendir(const char * path) {
    DIR dir = {0};

    syscall_2(sys_opendir, (u32_t)path, (u32_t)&dir);

    return dir;
}

dirent readdir(DIR dir) {
    dirent dirent = {0};
   return dirent;
}