#include <cclib.h>
#include <itoa.h>
#include <string.h>
#include <bochs.h>

typedef struct {
  long quot, rem;
} ldiv_t;

static const char *digits = "0123456789abcdef";

char *itoa(int value, char *result, int base) {
  // check that the base if valid

  //  screen->puts("itoa ");
  //  screen->hexlong(value);
  //  screen->puts(" ");
  //  screen->hexlong(base);
  //  screen->newline();

  if (base < 2 || base > 16) {
    *result = 0;
    return result;
  }

  if (value == 0) {
    result[0] = '0';
    result[1] = '\0';
    return result;
  }
  else {
    char *out = result;
    int quotient = abs(value);
    //    screen->puts("quotient ");
    //    screen->hexlong(quotient);
    //    screen->newline();

    do {
      const int tmp = quotient / base;
      //      screen->puts(" tmp ");
      //      screen->hexlong(tmp);
      int ndx = quotient - (tmp * base);
      char c = digits[ndx];
      *out++ = c;
      *out = 0;
      quotient = tmp;
      //      screen->puts(" quotient ");
      //      screen->hexlong(quotient);
      //      screen->puts(" ndx ");
      //      screen->hexlong(ndx);
      //      screen->puts(" c ");
      //      screen->putc(c);
      //      screen->puts(" out ");
      //      screen->puts(result);
      //      screen->newline();
    } while (quotient);

    // Apply negative sign
    if (value < 0) {
      *out++ = '-';
    }

    *out = 0;
    //    screen->puts("out ");
    //    screen->puts(out);
    //    screen->newline();
    reverse(result);
  }
  return result;
}

char *ltoa(long value, char *result, int base) {
  // check that the base if valid

  //  screen->puts("itoa ");
  //  screen->hexlong(value);
  //  screen->puts(" ");
  //  screen->hexlong(base);
  //  screen->newline();

  if (base < 2 || base > 16) {
    *result = 0;
    return result;
  }

  if (value == 0) {
    result[0] = '0';
    result[1] = '\0';
    return result;
  }
  else {
    char *out = result;
    long quotient = abs(value);
    //    screen->puts("quotient ");
    //    screen->hexlong(quotient);
    //    screen->newline();

    do {
      const long tmp = quotient / base;
      //      screen->puts(" tmp ");
      //      screen->hexlong(tmp);
      int ndx = quotient - (tmp * base);
      char c = digits[ndx];
      *out++ = c;
      *out = 0;
      quotient = tmp;
      //      screen->puts(" quotient ");
      //      screen->hexlong(quotient);
      //      screen->puts(" ndx ");
      //      screen->hexlong(ndx);
      //      screen->puts(" c ");
      //      screen->putc(c);
      //      screen->puts(" out ");
      //      screen->puts(result);
      //      screen->newline();
    } while (quotient);

    // Apply negative sign
    if (value < 0) {
      *out++ = '-';
    }

    *out = 0;
    //    screen->puts("out ");
    //    screen->puts(out);
    //    screen->newline();
    reverse(result);
  }
  return result;
}

#if 0
/* Return the `ldiv_t' representation of NUMER over DENOM.  */
ldiv_t ldiv(unsigned long int numer, unsigned long int denom) {
  ldiv_t result;

  result.quot = numer / denom;
  result.rem = numer % denom;

  /* The ANSI standard says that |QUOT| <= |NUMER / DENOM|, where
     NUMER / DENOM is to be computed in infinite precision.  In
     other words, we should always truncate the quotient towards
     zero, never -infinity.  Machine division and remainer may
     work either way when one or both of NUMER or DENOM is
     negative.  If only one is negative and QUOT has been
     truncated towards -infinity, REM will have the same sign as
     DENOM and the opposite sign of NUMER; if both are negative
     and QUOT has been truncated towards -infinity, REM will be
     positive (will have the opposite sign of NUMER).  These are
     considered `wrong'.  If both are NUM and DENOM are positive,
     RESULT will always be positive.  This all boils down to: if
     NUMER >= 0, but REM < 0, we got the wrong answer.  In that
     case, to get the right answer, add 1 to QUOT and subtract
     DENOM from REM.  */

  if (numer && result.rem < 0) {
    ++result.quot;
    result.rem -= denom;
  }

  return result;
}

#define BUFSIZE (sizeof(unsigned long long) * 8 + 1)

char *ltoa(unsigned long long N, char *str, int base) {
  register int i = 2;
  unsigned long long uarg;
  char *tail, *head = str, buf[BUFSIZE] = { 0 };

  if (36 < base || 2 > base) {
    base = 10; /* can only use 0-9, A-Z        */
  }
  tail = &buf[BUFSIZE - 1]; /* last character position      */
  *tail-- = '\0';

  if (10 == base) {
    *head++ = '-';
    uarg = -N;
  }
  else {
    uarg = N;
  }

  if (uarg) {
    for (i = 1; uarg; ++i) {
      ldiv_t res;

      res = ldiv(uarg, base);
      *tail-- = (char)(res.rem + ((9L < res.rem) ? ('A' - 10L) : '0'));
      uarg = res.quot;
    }
  }
  else {
    *tail-- = '0';
  }

  memcpy(head, ++tail, i);
  return str;
}
#endif