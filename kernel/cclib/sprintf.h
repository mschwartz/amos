#ifndef VSPRINTF_H
#define VSPRINTF_H

#include <stdarg.h>

extern int vsprintf(char* buffer, const char* format, va_list parameters) ;
extern int snprintf(char* buffer, size_t n, const char* fmt, ...);
extern int sprintf(char* buffer, const char* format, ...) ;
extern int asprintf(char **buffer, const char *fmt, ...);

#endif
