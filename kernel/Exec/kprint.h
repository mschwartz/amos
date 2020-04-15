#ifndef KPRINT_H
#define KPRINT_H

extern "C" void kputc(char c);
extern "C" void kputs(const char *s);
extern "C" void kprint(const char *fmt, ...);

#endif
