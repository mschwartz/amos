#include <stdarg.h>
#include <limits.h>

#include <posix.h>
#include <posix/string.h>
#include <posix/itoa.h>
#include <x86/kprint.h>
#include <x86/bochs.h>

#include <posix/sprintf.h>

static int putchar(int ic) {
  char c = (char)ic;
  dputc(c);
  //terminal_write(&c, sizeof(c));
  return ic;
}

static bool print(char *data, size_t length) {
  const unsigned char *bytes = (const unsigned char *)data;
  for (size_t i = 0; i < length; i++)
    if (putchar(bytes[i]) == -1) {
      return false;
    }
  return true;
}

int snprintf(char *buffer, size_t n, const char *fmt, ...) {
  va_list parameters;
  char temp_buffer[n];

  va_start(parameters, fmt);
//  dprintf("xparameters: %x\n", parameters);
  vsprintf(temp_buffer, fmt, parameters);
  va_end(parameters);

  memcpy(buffer, &temp_buffer[0], n * sizeof(char));

  for (int i = n - 1; i > 0; i--) {
    if ((int)temp_buffer[i] != 0) {
      return i;
    }
  }

  return -1;
}

#if 0
int asprintf(char **buffer, const char *fmt, ...) {
  va_list parameters;
  char temp_buffer[128] = { 0 };

  va_start(parameters, fmt);
  vsprintf(temp_buffer, fmt, parameters);
  va_end(parameters);

  size_t s = 0;
  for (int i = 127; i > 0; i--) {
    if ((int)temp_buffer[i] != 0) {
      s = i;
      break;
    }
  }

  char *rs = new char[s];
  memcpy(rs, &temp_buffer[0], s * sizeof(char));
  buffer = &rs;

  return s;
}
#endif

int sprintf(char* buffer, const char* format, ...) {
	va_list parameters;
	va_start(parameters, format);
  long value;
  int d;
  char fmt_buffer[16]; //  = { 0 };
  // set the buffer to zero before any writing
  int i = 0;
  while (buffer[i] != 0) {
    buffer[i] = 0;
    i++;
  }

  char *out = buffer, *src, *s, c, cc;
  int written = 0;

  while (*format != '\0') {
    switch ((c = *format++)) {
      case '%':
        switch ((cc = *format++)) {
          case '\0':
            return written;
          case 'd':
          case 'u':
          case 'i':
            d = va_arg(parameters, int);
            itoa(d, fmt_buffer, 10);
            src = fmt_buffer;
            while (*src) {
              written++;
              *out++ = *src++;
            }
            break;
          case 'x':
            d = va_arg(parameters, int);
            ltoa(d, fmt_buffer, 16);
            src = fmt_buffer;
            while (*src) {
              written++;
              *out++ = *src++;
            }
            break;
          case 's':
            s = va_arg(parameters, char *);
            src = fmt_buffer;
            while (*src) {
              written++;
              *out++ = *src++;
            }
            break;
          case 'c':
            c = (char)va_arg(parameters, int);
            written++;
            *out++ = c;
            break;
          default:
            *out++ = cc;
            written++;
            break;
        }
        break;
      default:
        *out++ = *src++;
        break;
    }
  }
  return written;
}

int vsprintf(char *buffer, const char *format, va_list parameters) {
  long value;
  unsigned long d;
  char fmt_buffer[16]; //  = { 0 };
  // set the buffer to zero before any writing
  int i = 0;
  while (buffer[i] != 0) {
    buffer[i] = 0;
    i++;
  }

  char *out = buffer, *src, *s, c, cc;
  int written = 0;

  while (*format != '\0') {
    switch ((c = *format++)) {
      case '%':
        switch ((cc = *format++)) {
          case '\0':
            return written;
          case 'd':
          case 'u':
          case 'i':
            d = va_arg(parameters, unsigned long);
            itoa(d, fmt_buffer, 10);
            src = fmt_buffer;
            while (*src) {
              written++;
              *out++ = *src++;
            }
            break;
          case 'x':
            d = va_arg(parameters, unsigned long);
            ltoa(d, fmt_buffer, 16);
            src = fmt_buffer;
            while (*src) {
              written++;
              *out++ = *src++;
            }
            break;
          case 's':
            s = va_arg(parameters, char *);
            src = fmt_buffer;
            while (*src) {
              written++;
              *out++ = *src++;
            }
            break;
          case 'c':
            c = (char)va_arg(parameters, int);
            written++;
            *out++ = c;
            break;
          default:
            *out++ = cc;
            written++;
            break;
        }
        break;
      default:
        *out++ = c;
        break;
    }
  }
  return written;
}
