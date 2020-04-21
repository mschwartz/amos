#include <stdarg.h>
#include <limits.h>

#include <posix.h>
#include <posix/string.h>
#include <posix/itoa.h>
#include <x86/kprint.h>
#include <x86/bochs.h>

#include <posix/sprintf.h>

#define SMALL_PRINTF
//#undef SMALL_PRINTF

#ifdef SMALL_PRINTF
// small printf
// https://www.menie.org/georges/embedded/small_printf_source_code.html

/*
  Copyright 2001-2019 Georges Menie
  https://www.menie.org/georges/embedded/small_printf_source_code.html
  stdarg version contributed by Christian Ettinger

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
	putchar is the only external dependency for this file,
	if you have a working putchar, leave it commented out.
	If not, uncomment the define below and
	replace outbyte(c) by your own function call.

#define putchar(c) outbyte(c)
*/

#include <stdarg.h>

static int putchar(int ic) {
  char c = (char)ic;
  dputc(c);
  //terminal_write(&c, sizeof(c));
  return ic;
}

static void printchar(char **str, int c) {
  extern int putchar(int c);

  if (str) {
    **str = c;
    ++(*str);
  }
  else
    (void)putchar(c);
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

static int prints(char **out, const char *string, int width, int pad) {
  register int pc = 0, padchar = ' ';

  if (width > 0) {
    register int len = 0;
    register const char *ptr;
    for (ptr = string; *ptr; ++ptr)
      ++len;
    if (len >= width)
      width = 0;
    else
      width -= len;
    if (pad & PAD_ZERO)
      padchar = '0';
  }
  if (!(pad & PAD_RIGHT)) {
    for (; width > 0; --width) {
      printchar(out, padchar);
      ++pc;
    }
  }
  for (; *string; ++string) {
    printchar(out, *string);
    ++pc;
  }
  for (; width > 0; --width) {
    printchar(out, padchar);
    ++pc;
  }

  return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

static int printi(char **out, int i, int b, int sg, int width, int pad, int letbase) {
  char print_buf[PRINT_BUF_LEN];
  register char *s;
  register int t, neg = 0, pc = 0;
  register unsigned int u = i;

  if (i == 0) {
    print_buf[0] = '0';
    print_buf[1] = '\0';
    return prints(out, print_buf, width, pad);
  }

  if (sg && b == 10 && i < 0) {
    neg = 1;
    u = -i;
  }

  s = print_buf + PRINT_BUF_LEN - 1;
  *s = '\0';

  while (u) {
    t = u % b;
    if (t >= 10)
      t += letbase - '0' - 10;
    *--s = t + '0';
    u /= b;
  }

  if (neg) {
    if (width && (pad & PAD_ZERO)) {
      printchar(out, '-');
      ++pc;
      --width;
    }
    else {
      *--s = '-';
    }
  }

  return pc + prints(out, s, width, pad);
}

static int print(char **out, const char *format, va_list args) {
  register int width, pad;
  register int pc = 0;
  char scr[2];

  for (; *format != 0; ++format) {
    if (*format == '%') {
      ++format;
      width = pad = 0;
      if (*format == '\0')
        break;
      if (*format == '%')
        goto out;
      if (*format == '-') {
        ++format;
        pad = PAD_RIGHT;
      }
      while (*format == '0') {
        ++format;
        pad |= PAD_ZERO;
      }
      for (; *format >= '0' && *format <= '9'; ++format) {
        width *= 10;
        width += *format - '0';
      }
      if (*format == 's') {
        register char *s = (char *)va_arg(args, int);
        pc += prints(out, s ? s : "(null)", width, pad);
        continue;
      }
      if (*format == 'd') {
        pc += printi(out, va_arg(args, int), 10, 1, width, pad, 'a');
        continue;
      }
      if (*format == 'x') {
        pc += printi(out, va_arg(args, int), 16, 0, width, pad, 'a');
        continue;
      }
      if (*format == 'X') {
        pc += printi(out, va_arg(args, int), 16, 0, width, pad, 'A');
        continue;
      }
      if (*format == 'u') {
        pc += printi(out, va_arg(args, int), 10, 0, width, pad, 'a');
        continue;
      }
      if (*format == 'c') {
        /* char are converted to int then pushed on the stack */
        scr[0] = (char)va_arg(args, int);
        scr[1] = '\0';
        pc += prints(out, scr, width, pad);
        continue;
      }
    }
    else {
    out:
      printchar(out, *format);
      ++pc;
    }
  }
  if (out)
    **out = '\0';
  va_end(args);
  return pc;
}

int printf(const char *format, ...) {
  va_list args;

  va_start(args, format);
  return print(0, format, args);
}

int sprintf(char *out, const char *format, ...) {
  va_list args;

  va_start(args, format);
  return print(&out, format, args);
}

int vsprintf(char *buffer, const char *format, va_list parameters) {
  return print(0, format, parameters);
}

#if 0
#ifdef TEST_PRINTF
int main(void) {
  char *ptr = "Hello world!";
  char *np = 0;
  int i = 5;
  unsigned int bs = sizeof(int) * 8;
  int mi;
  char buf[80];

  mi = (1 << (bs - 1)) + 1;
  printf("%s\n", ptr);
  printf("printf test\n");
  printf("%s is null pointer\n", np);
  printf("%d = 5\n", i);
  printf("%d = - max int\n", mi);
  printf("char %c = 'a'\n", 'a');
  printf("hex %x = ff\n", 0xff);
  printf("hex %02x = 00\n", 0);
  printf("signed %d = unsigned %u = hex %x\n", -3, -3, -3);
  printf("%d %s(s)%", 0, "message");
  printf("\n");
  printf("%d %s(s) with %%\n", 0, "message");
  sprintf(buf, "justif: \"%-10s\"\n", "left");
  printf("%s", buf);
  sprintf(buf, "justif: \"%10s\"\n", "right");
  printf("%s", buf);
  sprintf(buf, " 3: %04d zero padded\n", 3);
  printf("%s", buf);
  sprintf(buf, " 3: %-4d left justif.\n", 3);
  printf("%s", buf);
  sprintf(buf, " 3: %4d right justif.\n", 3);
  printf("%s", buf);
  sprintf(buf, "-3: %04d zero padded\n", -3);
  printf("%s", buf);
  sprintf(buf, "-3: %-4d left justif.\n", -3);
  printf("%s", buf);
  sprintf(buf, "-3: %4d right justif.\n", -3);
  printf("%s", buf);

  return 0;
}

/*
 * if you compile this file with
 *   gcc -Wall $(YOUR_C_OPTIONS) -DTEST_PRINTF -c printf.c
 * you will get a normal warning:
 *   printf.c:214: warning: spurious trailing `%' in format
 * this line is testing an invalid % at the end of the format string.
 *
 * this should display (on 32bit int machine) :
 *
 * Hello world!
 * printf test
 * (null) is null pointer
 * 5 = 5
 * -2147483647 = - max int
 * char a = 'a'
 * hex ff = ff
 * hex 00 = 00
 * signed -3 = unsigned 4294967293 = hex fffffffd
 * 0 message(s)
 * 0 message(s) with %
 * justif: "left      "
 * justif: "     right"
 *  3: 0003 zero padded
 *  3: 3    left justif.
 *  3:    3 right justif.
 * -3: -003 zero padded
 * -3: -3   left justif.
 * -3:   -3 right justif.
 */

#endif
#endif
#else
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

int sprintf(char *buffer, const char *format, ...) {
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
#endif
