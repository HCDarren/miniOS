#ifndef MINIOS_STDARG_H
#define MINIOS_STDARG_H

typedef char *va_list;
#define va_start(arg_p, arg_start) (arg_p = (va_list)&arg_start + sizeof(char *))
#define va_arg(arg_p, type) (*(type *)((arg_p += sizeof(type)) - sizeof(type)))
#define va_end(arg_p) (arg_p = (va_list)0)

#endif