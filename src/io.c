/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org>
 */

#include "gb/io.h"

ssize_t gb_printf(char const *fmt, ...) {
  ssize_t res;
  va_list va;
  va_start(va, fmt);
  res = gb_printf_va(fmt, va);
  va_end(va);
  return res;
}

ssize_t gb_printf_err(char const *fmt, ...) {
  ssize_t res;
  va_list va;
  va_start(va, fmt);
  res = gb_printf_err_va(fmt, va);
  va_end(va);
  return res;
}

ssize_t gb_fprintf(struct gbFile *f, char const *fmt, ...) {
  ssize_t res;
  va_list va;
  va_start(va, fmt);
  res = gb_fprintf_va(f, fmt, va);
  va_end(va);
  return res;
}

char *gb_bprintf(char const *fmt, ...) {
  va_list va;
  char *str;
  va_start(va, fmt);
  str = gb_bprintf_va(fmt, va);
  va_end(va);
  return str;
}

ssize_t gb_snprintf(char *str, ssize_t n, char const *fmt, ...) {
  ssize_t res;
  va_list va;
  va_start(va, fmt);
  res = gb_snprintf_va(str, n, fmt, va);
  va_end(va);
  return res;
}

gb_inline ssize_t gb_printf_va(char const *fmt, va_list va) {
  return gb_fprintf_va(gb_file_get_standard(gbFileStandard_Output), fmt, va);
}

gb_inline ssize_t gb_printf_err_va(char const *fmt, va_list va) {
  return gb_fprintf_va(gb_file_get_standard(gbFileStandard_Error), fmt, va);
}

gb_inline ssize_t gb_fprintf_va(struct gbFile *f, char const *fmt, va_list va) {
  gb_local_persist char buf[4096];
  ssize_t len = gb_snprintf_va(buf, gb_size_of(buf), fmt, va);
  gb_file_write(f, buf, len - 1); // NOTE(bill): prevent extra whitespace
  return len;
}

gb_inline char *gb_bprintf_va(char const *fmt, va_list va) {
  gb_local_persist char buffer[4096];
  gb_snprintf_va(buffer, gb_size_of(buffer), fmt, va);
  return buffer;
}

enum {
  gbFmt_Minus = GB_BIT(0),
  gbFmt_Plus = GB_BIT(1),
  gbFmt_Alt = GB_BIT(2),
  gbFmt_Space = GB_BIT(3),
  gbFmt_Zero = GB_BIT(4),

  gbFmt_Char = GB_BIT(5),
  gbFmt_Short = GB_BIT(6),
  gbFmt_Int = GB_BIT(7),
  gbFmt_Long = GB_BIT(8),
  gbFmt_Llong = GB_BIT(9),
  gbFmt_Size = GB_BIT(10),
  gbFmt_Intptr = GB_BIT(11),

  gbFmt_Unsigned = GB_BIT(12),
  gbFmt_Lower = GB_BIT(13),
  gbFmt_Upper = GB_BIT(14),

  gbFmt_Done = GB_BIT(30),

  gbFmt_Ints = gbFmt_Char | gbFmt_Short | gbFmt_Int | gbFmt_Long | gbFmt_Llong | gbFmt_Size | gbFmt_Intptr
};

typedef struct {
  int32_t base;
  int32_t flags;
  int32_t width;
  int32_t precision;
} gbprivFmtInfo;

gb_internal ssize_t gb__print_string(char *text, ssize_t max_len, gbprivFmtInfo *info, char const *str) {
  // TODO(bill): Get precision and width to work correctly. How does it actually work?!
  // TODO(bill): This looks very buggy indeed.
  ssize_t res = 0, len;
  ssize_t remaining = max_len;

  if (info && info->precision >= 0)
    len = gb_strnlen(str, info->precision);
  else
    len = gb_strlen(str);

  if (info && (info->width == 0 || info->flags & gbFmt_Minus)) {
    if (info->precision > 0)
      len = info->precision < len ? info->precision : len;

    res += gb_strlcpy(text, str, len);

    if (info->width > res) {
      ssize_t padding = info->width - len;
      char pad = (info->flags & gbFmt_Zero) ? '0' : ' ';
      while (padding-- > 0 && remaining-- > 0)
        *text++ = pad, res++;
    }
  } else {
    if (info && (info->width > res)) {
      ssize_t padding = info->width - len;
      char pad = (info->flags & gbFmt_Zero) ? '0' : ' ';
      while (padding-- > 0 && remaining-- > 0)
        *text++ = pad, res++;
    }

    res += gb_strlcpy(text, str, len);
  }

  if (info) {
    if (info->flags & gbFmt_Upper)
      gb_str_to_upper(text);
    else if (info->flags & gbFmt_Lower)
      gb_str_to_lower(text);
  }

  return res;
}

gb_internal ssize_t gb__print_char(char *text, ssize_t max_len, gbprivFmtInfo *info, char arg) {
  char str[2] = "";
  str[0] = arg;
  return gb__print_string(text, max_len, info, str);
}

gb_internal ssize_t gb__print_i64(char *text, ssize_t max_len, gbprivFmtInfo *info, int64_t value) {
  char num[130];
  gb_i64_to_str(value, num, info ? info->base : 10);
  return gb__print_string(text, max_len, info, num);
}

gb_internal ssize_t gb__print_u64(char *text, ssize_t max_len, gbprivFmtInfo *info, uint64_t value) {
  char num[130];
  gb_u64_to_str(value, num, info ? info->base : 10);
  return gb__print_string(text, max_len, info, num);
}

gb_internal ssize_t gb__print_f64(char *text, ssize_t max_len, gbprivFmtInfo *info, float64_t arg) {
  // TODO(bill): Handle exponent notation
  ssize_t width, len, remaining = max_len;
  char *text_begin = text;

  if (arg) {
    uint64_t value;
    if (arg < 0) {
      if (remaining > 1)
        *text = '-', remaining--;
      text++;
      arg = -arg;
    } else if (info->flags & gbFmt_Minus) {
      if (remaining > 1)
        *text = '+', remaining--;
      text++;
    }

    value = cast(uint64_t) arg;
    len = gb__print_u64(text, remaining, NULL, value);
    text += len;

    if (len >= remaining)
      remaining = gb_min(remaining, 1);
    else
      remaining -= len;
    arg -= value;

    if (info->precision < 0)
      info->precision = 6;

    if ((info->flags & gbFmt_Alt) || info->precision > 0) {
      int64_t mult = 10;
      if (remaining > 1)
        *text = '.', remaining--;
      text++;
      while (info->precision-- > 0) {
        value = cast(uint64_t) (arg * mult);
        len = gb__print_u64(text, remaining, NULL, value);
        text += len;
        if (len >= remaining)
          remaining = gb_min(remaining, 1);
        else
          remaining -= len;
        arg -= cast(float64_t) value / mult;
        mult *= 10;
      }
    }
  } else {
    if (remaining > 1)
      *text = '0', remaining--;
    text++;
    if (info->flags & gbFmt_Alt) {
      if (remaining > 1)
        *text = '.', remaining--;
      text++;
    }
  }

  width = info->width - (text - text_begin);
  if (width > 0) {
    char fill = (info->flags & gbFmt_Zero) ? '0' : ' ';
    char *end = text + remaining - 1;
    len = (text - text_begin);

    for (len = (text - text_begin); len--;) {
      if ((text_begin + len + width) < end)
        *(text_begin + len + width) = *(text_begin + len);
    }

    len = width;
    text += len;
    if (len >= remaining)
      remaining = gb_min(remaining, 1);
    else
      remaining -= len;

    while (len--) {
      if (text_begin + len < end)
        text_begin[len] = fill;
    }
  }

  return (text - text_begin);
}

gb_no_inline ssize_t gb_snprintf_va(char *text, ssize_t max_len, char const *fmt, va_list va) {
  char const *text_begin = text;
  ssize_t remaining = max_len, res;

  while (*fmt) {
    gbprivFmtInfo info = {0};
    ssize_t len = 0;
    info.precision = -1;

    while (*fmt && *fmt != '%' && remaining)
      *text++ = *fmt++;

    if (*fmt == '%') {
      do {
        switch (*++fmt) {
          case '-':
            info.flags |= gbFmt_Minus;
            break;
          case '+':
            info.flags |= gbFmt_Plus;
            break;
          case '#':
            info.flags |= gbFmt_Alt;
            break;
          case ' ':
            info.flags |= gbFmt_Space;
            break;
          case '0':
            info.flags |= gbFmt_Zero;
            break;
          default:
            info.flags |= gbFmt_Done;
            break;
        }
      } while (!(info.flags & gbFmt_Done));
    }

    // NOTE(bill): Optional Width
    if (*fmt == '*') {
      int width = va_arg(va, int);
      if (width < 0) {
        info.flags |= gbFmt_Minus;
        info.width = -info.width;
      } else {
        info.width = -info.width;
      }
      fmt++;
    } else {
      info.width = cast(int32_t) gb_str_to_i64(fmt, cast(char **) &fmt, 10);
    }

    // NOTE(bill): Optional Precision
    if (*fmt == '.') {
      fmt++;
      if (*fmt == '*') {
        info.precision = va_arg(va, int);
        fmt++;
      } else {
        info.precision = cast(int32_t) gb_str_to_i64(fmt, cast(char **) &fmt, 10);
      }
      info.flags &= ~gbFmt_Zero;
    }

    switch (*fmt++) {
      case 'h':
        if (*fmt == 'h') { // hh => char
          info.flags |= gbFmt_Char;
          fmt++;
        } else { // h => short
          info.flags |= gbFmt_Short;
        }
        break;

      case 'l':
        if (*fmt == 'l') { // ll => long long
          info.flags |= gbFmt_Llong;
          fmt++;
        } else { // l => long
          info.flags |= gbFmt_Long;
        }
        break;

        break;

      case 'z': // NOTE(bill): size_t
        info.flags |= gbFmt_Unsigned;
        // fallthrough
      case 't': // NOTE(bill): ssize_t
        info.flags |= gbFmt_Size;
        break;

      default:
        fmt--;
        break;
    }

    switch (*fmt) {
      case 'u':
        info.flags |= gbFmt_Unsigned;
        // fallthrough
      case 'd':
      case 'i':
        info.base = 10;
        break;

      case 'o':
        info.base = 8;
        break;

      case 'x':
        info.base = 16;
        info.flags |= (gbFmt_Unsigned | gbFmt_Lower);
        break;

      case 'X':
        info.base = 16;
        info.flags |= (gbFmt_Unsigned | gbFmt_Upper);
        break;

      case 'f':
      case 'F':
      case 'g':
      case 'G':
        len = gb__print_f64(text, remaining, &info, va_arg(va, float64_t));
        break;

      case 'a':
      case 'A':
        // TODO(bill):
        break;

      case 'c':
        len = gb__print_char(text, remaining, &info, cast(char) va_arg(va, int));
        break;

      case 's':
        len = gb__print_string(text, remaining, &info, va_arg(va, char *));
        break;

      case 'p':
        info.base = 16;
        info.flags |= (gbFmt_Lower | gbFmt_Unsigned | gbFmt_Alt | gbFmt_Intptr);
        break;

      case '%':
        len = gb__print_char(text, remaining, &info, '%');
        break;

      default:
        fmt--;
        break;
    }

    fmt++;

    if (info.base != 0) {
      if (info.flags & gbFmt_Unsigned) {
        uint64_t value = 0;
        switch (info.flags & gbFmt_Ints) {
          case gbFmt_Char:
            value = cast(uint64_t) cast(uint8_t) va_arg(va, int);
            break;
          case gbFmt_Short:
            value = cast(uint64_t) cast(uint16_t) va_arg(va, int);
            break;
          case gbFmt_Long:
            value = cast(uint64_t) va_arg(va, unsigned long);
            break;
          case gbFmt_Llong:
            value = cast(uint64_t) va_arg(va, unsigned long long);
            break;
          case gbFmt_Size:
            value = cast(uint64_t) va_arg(va, size_t);
            break;
          case gbFmt_Intptr:
            value = cast(uint64_t) va_arg(va, uintptr_t);
            break;
          default:
            value = cast(uint64_t) va_arg(va, unsigned int);
            break;
        }

        len = gb__print_u64(text, remaining, &info, value);

      } else {
        int64_t value = 0;
        switch (info.flags & gbFmt_Ints) {
          case gbFmt_Char:
            value = cast(int64_t) cast(int8_t) va_arg(va, int);
            break;
          case gbFmt_Short:
            value = cast(int64_t) cast(int16_t) va_arg(va, int);
            break;
          case gbFmt_Long:
            value = cast(int64_t) va_arg(va, long);
            break;
          case gbFmt_Llong:
            value = cast(int64_t) va_arg(va, long long);
            break;
          case gbFmt_Size:
            value = cast(int64_t) va_arg(va, size_t);
            break;
          case gbFmt_Intptr:
            value = cast(int64_t) va_arg(va, uintptr_t);
            break;
          default:
            value = cast(int64_t) va_arg(va, int);
            break;
        }

        len = gb__print_i64(text, remaining, &info, value);
      }
    }

    text += len;
    if (len >= remaining)
      remaining = gb_min(remaining, 1);
    else
      remaining -= len;
  }

  *text++ = '\0';
  res = (text - text_begin);
  return (res >= max_len || res < 0) ? -1 : res;
}
