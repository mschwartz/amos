#ifndef POSIX_SPRINTF_H
#define POSIX_SPRINTF_H

#include <posix/stdint.h>
#include <posix/stddef.h>
#include <stdarg.h>

extern int vsprintf(char* buffer, const char* format, va_list parameters) ;
extern int snprintf(char* buffer, size_t n, const char* fmt, ...);
extern int asprintf(char **buffer, const char *fmt, ...);

extern int sprintf(char* buffer, const char* format, ...) ;

#endif
