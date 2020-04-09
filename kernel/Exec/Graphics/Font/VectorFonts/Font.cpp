#define DEBUGME

/**
 * Some of this code lifted from Arduino serial Print class and modified.
 * See: https://github.com/arduino/Arduino/blob/master/hardware/arduino/avr/cores/arduino/Print.cpp
 */

#include "Font.h"
#include "charset.h"

const TInt16 *gVectorCharset[] = {
  ENull, // space
  font_emark,
#ifdef FULL_CHARSET
  font_dquote,
#else
  ENull,
#endif
#ifdef FULL_CHARSET
  font_pound, // #
#else
  ENull,
#endif
#ifdef FULL_CHARSET
  font_dollar, // $
#else
  ENull,
#endif
#ifdef FULL_CHARSET
  font_percent, // %
#else
  ENull,
#endif
#ifdef FULL_CHARSET
  font_amp, // &
#else
  ENull,
#endif
#ifdef FULL_CHARSET
  font_squote, // '
#else
  ENull,
#endif
#ifdef FULL_CHARSET
  font_lparen, // (
#else
  ENull,
#endif
#ifdef FULL_CHARSET
  font_rparen, // )
#else
  ENull,
#endif
#ifdef FULL_CHARSET
  font_asterisk, // *
#else
  ENull,
#endif
#ifdef FULL_CHARSET
  font_plus,
#else
  ENull,
#endif
#ifdef FULL_CHARSET
  font_comma,
#else
  ENull,
#endif
#ifdef FULL_CHARSET
  font_minus,
#else
  ENull,
#endif
  font_period,
  font_fslash,
  font_0,
  font_1,
  font_2,
  font_3,
  font_4,
  font_5,
  font_6,
  font_7,
  font_8,
  font_9,
  font_colon,
#ifdef full_charset
  font_semicolon,
#else
  ENull,
#endif
#ifdef full_charset
  font_lt, // <
#else
  ENull,
#endif
#ifdef full_charset
  font_eq, // =
#else
  ENull,
#endif
#ifdef full_charset
  font_gt, // >
#else
  ENull,
#endif
#ifdef full_charset
  font_qmark,
#else
  ENull,
#endif
#ifdef full_charset
  font_at, // @
#else
  ENull,
#endif
  font_a,
  font_b,
  font_c,
  font_d,
  font_e,
  font_f,
  font_g,
  font_h,
  font_i,
  font_j,
  font_k,
  font_l,
  font_m,
  font_n,
  font_o,
  font_p,
  font_q,
  font_r,
  font_s,
  font_t,
  font_u,
  font_v,
  font_w,
  font_x,
  font_y,
  font_z,
#ifdef full_charset
  font_lt, // [
#else
  ENull,
#endif
#ifdef full_charset
  font_bslash, // '\'
#else
  ENull,
#endif
#ifdef full_charset
  font_gt, // ]
#else
  ENull,
#endif
#ifdef full_charset
  font_caret, // ^
#else
  ENull,
#endif
#ifdef full_charset
  font_uscore, // _
#else
  ENull,
#endif
  ENull, // ``
};

#if 0
WORD Font::scale = 0x100;

#ifdef ENABLE_ROTATING_TEXT
TInt16 Font::print_string_rotatedx(TInt16 x, TInt16 y, FLOAT theta, const __FlashStringHelper *ifsh) {
  theta = float(theta) * 3.1415926 / 180;
  FLOAT cost = cos(theta),
        sint = sin(theta);
  PGM_P p = reinterpret_cast<PGM_P>(ifsh);

  FLOAT fscale = FLOAT(scale >> 8) + FLOAT(scale & 0xff) / 256.0;

  const TInt16 size = 9;

  TInt16 xo = x;
  while (char c = pgm_read_byte(p++)) {
    PGM_P glyph = (PGM_P)pgm_read_word(&charset[toupper(c) - 32]);
    if (glyph) {
      TInt16 lines = pgm_read_byte(glyph++);

      for (TInt16 i = 0; i < lines; i++) {
        FLOAT x0 = (TInt16)pgm_read_byte(glyph++) * fscale + x,
              y0 = (TInt16)pgm_read_byte(glyph++) * fscale + y,
              x1 = (TInt16)pgm_read_byte(glyph++) * fscale + x,
              y1 = (TInt16)pgm_read_byte(glyph++) * fscale + y;

        Graphics::drawLine(
            x0,
            ((y0 - y) * sint + cost + y),
            x1,
            ((y1 - y) * sint + cost + y));
      }
      x += size * fscale;
    }
    else {
      x += 6 * fscale;
    }
  }
  return x - xo;
}

TInt16 Font::write(TInt16 x, TInt16 y, char c) {
  PGM_P glyph;
  const TInt16 width = 9;

  FLOAT fscale = FLOAT(scale >> 8) + FLOAT(scale & 0xff) / 256.0;
  glyph = (PGM_P)pgm_read_word(&charset[toupper(c) - 32]);
  if (glyph) {
    TInt16 lines = pgm_read_byte(glyph++);

    for (TInt16 i = 0; i < lines; i++) {
      TInt16 x0 = pgm_read_byte(glyph++),
           y0 = pgm_read_byte(glyph++),
           x1 = pgm_read_byte(glyph++),
           y1 = pgm_read_byte(glyph++);

      Graphics::drawLine(x + x0 * fscale, y + y0 * fscale, x + x1 * fscale, y + y1 * fscale);
    }
  }
  return width * fscale;
}

TInt16 Font::print_string(TInt16 x, TInt16 y, char *s) {
  TInt16 xx = x;
  while (char c = *s++) {
    TInt16 width = Font::write(x, y, c);
    x += width;
  }
  return x - xx; // width of string printed
}

TInt16 Font::print_long(TInt16 x, TInt16 y, LONG n, TInt16 base) {
  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
  char *str = &buf[sizeof(buf) - 1];

  *str = '\0';

  // prevent crash if called with base == 1
  if (base < 2)
    base = 10;

  do {
    char c = n % base;
    n /= base;

    *--str = c < 10 ? c + '0' : c + 'A' - 10;
  } while (n);

  return print_string(x, y, str);
}

#ifdef PRINTF_FLOAT
TInt16 Font::print_float(TInt16 x, TInt16 y, double number, TInt16 digits) {
  TInt16 xx = x;
  if (isnan(number)) {
    x += write(x, y, 'n');
    x += write(x, y, 'a');
    x += write(x, y, 'n');
    return x;
  }
  if (isinf(number)) {
    x += write(x, y, 'i');
    x += write(x, y, 'n');
    x += write(x, y, 'f');
    return x;
  }
  if (number > 4294967040.0 || number < -4294967040.0) {
    x += write(x, y, 'o');
    x += write(x, y, 'v');
    x += write(x, y, 'f');
    return x;
  }

  // Handle negative numbers
  if (number < 0.0) {
    x += write(x, y, '-');
    number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i = 0; i < digits; ++i)
    rounding /= 10.0;

  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  x += print_long(x, y, int_part, 10);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0) {
    x += write(x, y, '.');
  }

  // Extract digits from the remainder one at a time
  while (digits-- > 0) {
    remainder *= 10.0;
    unsigned int toPrint = (unsigned int)(remainder);
    x += print_long(x, y, toPrint, 10);
    remainder -= toPrint;
  }

  return x - xx;
}
#endif

TInt16 Font::_printf(TInt16 x, TInt16 y, const __FlashStringHelper *ifsh, ...) {
  va_list ap;
  TInt16 xx = x;
  char c;
  PGM_P p = reinterpret_cast<PGM_P>(ifsh);
  va_start(ap, ifsh);

  while (c = pgm_read_byte(p++)) {
    if (c == '%') {
      c = pgm_read_byte(p++);
      switch (c) {
        case '\0':
          va_end(ap);
          return x - xx;
#ifdef PRINTF_FLOAT
        case '%':
          x += Font::write(x, y, '%');
          break;
        case 'f':
          x += print_float(x, y, va_arg(ap, double));
          break;
#endif
        case 'd':
          x += print_long(x, y, (unsigned long)va_arg(ap, int));
          break;
#ifdef PRINTF_FLOAT
        case 'x':
          x += print_long(x, y, (unsigned long)va_arg(ap, int) & 0xffff, 16);
          break;
        case 'l':
          x += print_long(x, y, va_arg(ap, long));
          break;
#endif
        default:
          x += Font::write(x, y, c);
          break;
      }
    }
    else {
      x += Font::write(x, y, c);
    }
  }
  va_end(ap);
  return xx - x;
}

#endif
#endif

