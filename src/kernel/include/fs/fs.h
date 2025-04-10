#ifndef MINIOS_FS_H
#define MINIOS_FS_H

#include <os.h>
#include <lib/dirent.h>

void fs_init();

int fs_open(const char* file_name, const u32_t flags);

int fs_write(const u32_t fd, const char* buf, const u32_t len);

int fs_closedir(DIR* dir);

DIR* fs_opendir(const char * path);

dirent* fs_readdir(DIR* dir);

#endif