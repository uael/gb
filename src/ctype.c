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

#include "gb/ctype.h"

gb_inline char gb_char_to_lower(char c) {
  if (c >= 'A' && c <= 'Z')
    return 'a' + (c - 'A');
  return c;
}

gb_inline char gb_char_to_upper(char c) {
  if (c >= 'a' && c <= 'z')
    return 'A' + (c - 'a');
  return c;
}

gb_inline byte32_t gb_char_is_space(char c) {
  if (c == ' ' ||
      c == '\t' ||
      c == '\n' ||
      c == '\r' ||
      c == '\f' ||
      c == '\v')
    return true;
  return false;
}

gb_inline byte32_t gb_char_is_digit(char c) {
  if (c >= '0' && c <= '9')
    return true;
  return false;
}

gb_inline byte32_t gb_char_is_hex_digit(char c) {
  if (gb_char_is_digit(c) ||
      (c >= 'a' && c <= 'f') ||
      (c >= 'A' && c <= 'F'))
    return true;
  return false;
}

gb_inline byte32_t gb_char_is_alpha(char c) {
  if ((c >= 'A' && c <= 'Z') ||
      (c >= 'a' && c <= 'z'))
    return true;
  return false;
}

gb_inline byte32_t gb_char_is_alphanumeric(char c) {
  return gb_char_is_alpha(c) || gb_char_is_digit(c);
}

gb_inline int32_t gb_digit_to_int(char c) {
  return gb_char_is_digit(c) ? c - '0' : c - 'W';
}

gb_inline int32_t gb_hex_digit_to_int(char c) {
  if (gb_char_is_digit(c))
    return gb_digit_to_int(c);
  else if (gb_is_between(c, 'a', 'f'))
    return c - 'a' + 10;
  else if (gb_is_between(c, 'A', 'F'))
    return c - 'A' + 10;
  return -1;
}
