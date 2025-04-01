#include <fs/file.h>
#include <ipc/mutex.h>
// 整个系统可打开的文件个数
#define FILE_TABLE_COUNT 4096

static file_t file_table[FILE_TABLE_COUNT] = {0};

static mutex_t mutex;

file_t* file_create() {
    file_t* file = NULL;
    mutex_lock(&mutex);
    for (size_t i = 0; i < FILE_TABLE_COUNT; i++)
    {
        if (file_table[i].reference_count == 0) {
            file = &file_table[i];
            file->reference_count = 1;
            break;
        }
    }
    mutex_unlock(&mutex);
    assert(file != NULL);
    return file;
}

void file_free(file_t* file) {
    mutex_lock(&mutex);
    assert(file != nullptr && file->reference_count != 0);
    file->reference_count--;
    mutex_unlock(&mutex);
}

void file_init() {
    mutex_init(&mutex);
}