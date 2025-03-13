#ifndef MINIOS_ASSERT_H
#define MINIOS_ASSERT_H

void assert_failure(const char *file, const char *func, int line, const char *expr);

#ifdef DEBUG // debug 版本

#define assert(expr) \
    if(!(expr)) assert_failure(__FILE__, __func__, __LINE__, #expr)
#else // 正式版本
    #define  assert(expr) ;
#endif

#endif