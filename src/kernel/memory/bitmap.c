#include "memory/bitmap.h"
#include <base/string.h>
#include <base/assert.h>

// 位图初始化
void bitmap_init(bitmap_t* bitmap, u8_t* bits, u32_t size) {
    assert(size % 8 == 0);
    bitmap->bits = bits;
    bitmap->size = size;
    memset(bitmap->bits, 0, size / 8);
}

// 返回位图的某个位是否可用
bool bitmap_search_available(bitmap_t* bitmap, u32_t index) {
    // 断言异常
    assert(index < bitmap->size);
    // 在哪个字节上？
    u32_t bytes = index / 8;
    // 在字节中的哪一位？
    u8_t bits = index % 8;
    // 判断是不是可用
    return !(bitmap->bits[bytes] & (1 << bits));
}

// 设置 bitmap 某位的值
// value: true 为占用；false 为闲置
void bitmap_set_state(bitmap_t* bitmap, u32_t index, bool value) {
    // 断言异常
    assert(index < bitmap->size);
    // 在哪个字节上？
    u32_t bytes = index / 8;
    // 在字节中的哪一位？
    u8_t bits = index % 8;
    if (value)
    {
        // 置为 1
        bitmap->bits[bytes] |= (1 << bits);
    }
    else
    {
        // 置为 0 注意这里不要写错了
        bitmap->bits[bytes] &= ~(1 << bits);
    }
}

// 设置 bitmap 某位的值
void bitmap_free(bitmap_t* bitmap, u32_t index, const u32_t free_count) {
    // 断言异常
    assert((index - free_count) < bitmap->size);
    for (size_t i = 0; i < free_count; i++)
    {
        bitmap_set_state(bitmap, i + index, false);
    }
}

// 扫描分配，连续的标记分配
int bitmap_scan_alloc(bitmap_t* bitmap, const u32_t count) {
    int bitmap_size = bitmap->size;
    // 最开始扫一直扫到最后
    int matched_count = 0;
    int matched_index = EOF;
    for (size_t i = 0; i < bitmap_size; i++)
    {
        if (bitmap_search_available(bitmap, i)) {
            if(++matched_count == count) {
                matched_index = i + 1 - count;
                break;
            } 
        } else {
            matched_count = 0;
        }
    }
    // 这个要记得加，不然跑起来要去找 bug 了
    if (matched_index == EOF) {
        return EOF;
    }
    // 标记为已经分配，不可用
    for (size_t i = 0; i < count; i++)
    {
        bitmap_set_state(bitmap, i + matched_index, true);
    }

    return matched_index;
}