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

#include "gb/math.h"

gb_inline bool gb_ispow2(const long n) {
  return n > 0 && GB_ISPOW2(n);
}

gb_inline size_t gb_roundup32(const ssize_t n) {
  ssize_t j, m;

  return n <= 0 ? 2 : (size_t) (
      GB_ISPOW2(n) ? n : (
          ((j = n & 0xFFFF0000) || (j = n)),
              ((m = j & 0xFF00FF00) || (m = j)),
              ((j = m & 0xF0F0F0F0) || (j = m)),
              ((m = j & 0xCCCCCCCC) || (m = j)),
              ((j = m & 0xAAAAAAAA) || (j = m)),
              j << 1
      )
  );
}
