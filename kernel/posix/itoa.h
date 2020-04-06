#ifndef POSIX_ITOA_H
#define POSIX_ITOA_H

#include <stdint.h>
#include <stddef.h>

extern char *itoa(int value, char *result, int base);
extern char *ltoa( int64_t N, char *str, int base);

#endif
