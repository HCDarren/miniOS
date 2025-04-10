#ifndef MINIOS_DIRENT_H
#define MINIOS_DIRENT_H

#include <os.h>

typedef struct {
	u32_t __fd;
	u32_t __size;
	u32_t __flags;
    char name[64];
} dirent;

typedef struct {
	u32_t __dd_fd;
	u32_t __dd_flags;
    char name[64];
} DIR;

int closedir(DIR);

DIR opendir(const char *);

dirent readdir(DIR);

#endif