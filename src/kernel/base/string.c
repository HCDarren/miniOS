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

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

void strcpy(char dst[], char* src) {
    assert(dst != NULL && src != NULL);
    char *ptr = dst;
    while (*src != '\0')
    {
        *ptr++ = *((char *)(src++));
    }
    *ptr = '\0';
}

void memset(void *dst, const size_t data, size_t len) {
    assert(dst != NULL);
    char *ptr = dst;
    while (len--)
    {
        *ptr++ = data;
    }
}