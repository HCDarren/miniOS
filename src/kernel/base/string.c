#include <base/string.h>
#include <base/assert.h>

void memcpy(void *dst, const void *src, size_t count) {
    assert(dst != NULL && src != NULL);
    char *ptr = dst;
    while (count--)
    {
        *ptr++ = *((char *)(src++));
    }
}

void memset(void *dst, const size_t data, size_t len) {
    assert(dst != NULL);
    char *ptr = dst;
    while (len--)
    {
        *ptr++ = data;
    }
}