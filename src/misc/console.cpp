/*
 *
 */

#include <stdarg.h>

#include "ch.h"
#include "hal.h"
#include "chsprintf.h"
#include "console.hpp"

#define MAX_FILLER 11
#define FLOAT_PRECISION 100

/*
 * @brief   console ...
 */
Console::Console(void) {
}

/*
 * @brief   console ...
 */
void Console::init(SerialDriver *drv) {
    this->_drv = drv;
    chMtxInit(&this->_mutex);
    sdStart(this->_drv, &this->_cfg);
}

static char *long_to_string_with_divisor(char *p,
                                         long num,
                                         unsigned radix,
                                         long divisor) {
  int i;
  char *q;
  long l, ll;

  l = num;
  if (divisor == 0) {
    ll = num;
  } else {
    ll = divisor;
  }

  q = p + MAX_FILLER;
  do {
    i = (int)(l % radix);
    i += '0';
    if (i > '9')
      i += 'A' - '0' - 10;
    *--q = i;
    l /= radix;
  } while ((ll /= radix) != 0);

  i = (int)(p + MAX_FILLER - q);
  do
    *p++ = *q++;
  while (--i);

  return p;
}

static char *ltoa(char *p, long num, unsigned radix) {

  return long_to_string_with_divisor(p, num, radix, 0);
}

#if CHPRINTF_USE_FLOAT
static char *ftoa(char *p, double num) {
  long l;
  unsigned long precision = FLOAT_PRECISION;

  l = num;
  p = long_to_string_with_divisor(p, l, 10, 0);
  *p++ = '.';
  l = (num - l) * precision;
  return long_to_string_with_divisor(p, l, 10, precision / 10);
}
#endif
/*
 * @brief   console ...
 */
void Console::write(char *fmt, ...) {
    chMtxLock(&this->_mutex);
    char buffer[this->_buffer_size];
    char *str = buffer;
    va_list ap;
    char *p, *s, c, filler;
    int i, precision, width;
    bool_t is_long, left_align;
    long l;
#if CHPRINTF_USE_FLOAT
    float f;
    char tmpbuf[2 * MAX_FILLER + 1];
#else
    char tmpbuf[MAX_FILLER + 1];
#endif
    // print timestamp
    uint8_t len = 0;
    len = chsprintf(buffer, "[%d] ", chTimeNow());
    sdWrite(this->_drv, (uint8_t *)buffer, len);
    //
    va_start(ap, fmt);
    while (TRUE) {
        c = *fmt++;
        if (c == 0) {
            va_end(ap);
            break;
        }
        if (c != '%') {
            *str++ = (uint8_t)c;
            continue;
        }
        p = tmpbuf;
        s = tmpbuf;
        left_align = FALSE;
        if (*fmt == '-') {
            fmt++;
            left_align = TRUE;
        }
        filler = ' ';
        if (*fmt == '.') {
            fmt++;
            filler = '0';
        }
        width = 0;
        while (TRUE) {
            c = *fmt++;
            if (c >= '0' && c <= '9')
                c -= '0';
            else if (c == '*')
                c = va_arg(ap, int);
            else
                break;
            width = width * 10 + c;
        }
        precision = 0;
        if (c == '.') {
            while (TRUE) {
                c = *fmt++;
                if (c >= '0' && c <= '9')
                    c -= '0';
                else if (c == '*')
                    c = va_arg(ap, int);
                else
                    break;
                precision *= 10;
                precision += c;
            }
        }
        /* Long modifier.*/
        if (c == 'l' || c == 'L') {
            is_long = TRUE;
            if (*fmt)
                c = *fmt++;
        } else
            is_long = (c >= 'A') && (c <= 'Z');

    /* Command decoding.*/
        switch (c) {
            case 'c':
                filler = ' ';
                *p++ = va_arg(ap, int);
                break;
            case 's':
                filler = ' ';
                if ((s = va_arg(ap, char *)) == 0)
                    s = "(null)";
                if (precision == 0)
                    precision = 32767;
                for (p = s; *p && (--precision >= 0); p++)
                ;
                break;
            case 'D':
            case 'd':
                if (is_long)
                    l = va_arg(ap, long);
                else
                    l = va_arg(ap, int);
                if (l < 0) {
                    *p++ = '-';
                    l = -l;
                }
                p = ltoa(p, l, 10);
                break;
#if CHPRINTF_USE_FLOAT
            case 'f':
                f = (float) va_arg(ap, double);
                if (f < 0) {
                    *p++ = '-';
                    f = -f;
                }
                p = ftoa(p, f);
                break;
#endif
            case 'X':
            case 'x':
                c = 16;
                goto unsigned_common;
            case 'U':
            case 'u':
                c = 10;
                goto unsigned_common;
            case 'O':
            case 'o':
                c = 8;
unsigned_common:
                if (is_long)
                    l = va_arg(ap, long);
                else
                    l = va_arg(ap, int);
                p = ltoa(p, l, c);
                break;
            default:
                *p++ = c;
            break;
        }
        i = (int)(p - s);
        if ((width -= i) < 0)
            width = 0;
        if (left_align == FALSE)
            width = -width;
        if (width < 0) {
            if (*s == '-' && filler == '0') {
                *str++ = (uint8_t)*s++;
                i--;
            }
            do
                *str++ = (uint8_t)filler;
            while (++width != 0);
        }
        while (--i >= 0)
            *str++ = (uint8_t)*s++;
    
        while (width) {
            *str++ = (uint8_t)filler;
            width--;
        }
    }
    //va_list args;
    //va_start(args, fmt);
    //chsprintf(buffer, fmt, args);
    //va_end(args);
    sdWrite(this->_drv, (uint8_t *)buffer, str - buffer);
    chMtxUnlock();
}

/*
 * @brief   console ...
 */
Console console;

