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

#include "gb/memory.h"

#define GB__ONES        (cast(size_t)-1/UINT8_MAX)
#define GB__HIGHS       (GB__ONES * (UINT8_MAX/2+1))
#define GB__HAS_ZERO(x) (((x)-GB__ONES) & ~(x) & GB__HIGHS)

byte32_t gb_is_power_of_two(ssize_t x) {
  if (x <= 0)
    return false;
  return !(x & (x - 1));
}

gb_inline void *gb_align_forward(void *ptr, ssize_t alignment) {
  uintptr_t p;
  ssize_t modulo;

  GB_ASSERT(gb_is_power_of_two(alignment));

  p = cast(uintptr_t) ptr;
  modulo = p & (alignment - 1);
  if (modulo) p += (alignment - modulo);
  return cast(void *) p;
}

gb_inline void *gb_pointer_add(void *ptr, ssize_t bytes) { return cast(void *) (cast(uint8_t *) ptr + bytes); }

gb_inline void *gb_pointer_sub(void *ptr, ssize_t bytes) { return cast(void *) (cast(uint8_t *) ptr - bytes); }

gb_inline void const *gb_pointer_add_const(void const *ptr, ssize_t bytes) {
  return cast(void const *) (cast(uint8_t const *) ptr + bytes);
}

gb_inline void const *gb_pointer_sub_const(void const *ptr, ssize_t bytes) {
  return cast(void const *) (cast(uint8_t const *) ptr - bytes);
}

gb_inline ssize_t gb_pointer_diff(void const *begin, void const *end) {
  return cast(ssize_t) (cast(uint8_t const *) end - cast(uint8_t const *) begin);
}

gb_inline void gb_zero_size(void *ptr, ssize_t size) { gb_memset(ptr, 0, size); }

#if defined(_MSC_VER)
#pragma intrinsic(__movsb)
#endif

gb_inline void *gb_memcopy(void *dest, void const *source, ssize_t n) {
#if defined(_MSC_VER)
  // TODO(bill): Is this good enough?
  __movsb(cast(uint8_t *)dest, cast(uint8_t *)source, n);
#elif defined(GB_CPU_X86)
  __asm__ __volatile__("rep movsb" : "+D"(dest), "+S"(source), "+c"(n) : : "memory");
#else
  uint8_t *d = cast(uint8_t *)dest;
  uint8_t const *s = cast(uint8_t const *)source;
  uint32_t w, x;

  for (; cast(uintptr_t)s % 4 && n; n--)
  *d++ = *s++;

  if (cast(uintptr_t)d % 4 == 0) {
    for (; n >= 16;
           s += 16, d += 16, n -= 16) {
      *cast(uint32_t *)(d+ 0) = *cast(uint32_t *)(s+ 0);
      *cast(uint32_t *)(d+ 4) = *cast(uint32_t *)(s+ 4);
      *cast(uint32_t *)(d+ 8) = *cast(uint32_t *)(s+ 8);
      *cast(uint32_t *)(d+12) = *cast(uint32_t *)(s+12);
    }
    if (n & 8) {
      *cast(uint32_t *)(d+0) = *cast(uint32_t *)(s+0);
      *cast(uint32_t *)(d+4) = *cast(uint32_t *)(s+4);
      d += 8;
      s += 8;
    }
    if (n&4) {
      *cast(uint32_t *)(d+0) = *cast(uint32_t *)(s+0);
      d += 4;
      s += 4;
    }
    if (n&2) {
      *d++ = *s++; *d++ = *s++;
    }
    if (n&1) {
      *d = *s;
    }
    return dest;
  }

  if (n >= 32) {
#if __BYTE_ORDER == __BIG_ENDIAN
#define LS <<
#define RS >>
#else
#define LS >>
#define RS <<
#endif
    switch (cast(uintptr_t)d % 4) {
      case 1: {
        w = *cast(uint32_t *)s;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        n -= 3;
        while (n > 16) {
          x = *cast(uint32_t *)(s+1);
          *cast(uint32_t *)(d+0)  = (w LS 24) | (x RS 8);
          w = *cast(uint32_t *)(s+5);
          *cast(uint32_t *)(d+4)  = (x LS 24) | (w RS 8);
          x = *cast(uint32_t *)(s+9);
          *cast(uint32_t *)(d+8)  = (w LS 24) | (x RS 8);
          w = *cast(uint32_t *)(s+13);
          *cast(uint32_t *)(d+12) = (x LS 24) | (w RS 8);

          s += 16;
          d += 16;
          n -= 16;
        }
      } break;
      case 2: {
        w = *cast(uint32_t *)s;
        *d++ = *s++;
        *d++ = *s++;
        n -= 2;
        while (n > 17) {
          x = *cast(uint32_t *)(s+2);
          *cast(uint32_t *)(d+0)  = (w LS 16) | (x RS 16);
          w = *cast(uint32_t *)(s+6);
          *cast(uint32_t *)(d+4)  = (x LS 16) | (w RS 16);
          x = *cast(uint32_t *)(s+10);
          *cast(uint32_t *)(d+8)  = (w LS 16) | (x RS 16);
          w = *cast(uint32_t *)(s+14);
          *cast(uint32_t *)(d+12) = (x LS 16) | (w RS 16);

          s += 16;
          d += 16;
          n -= 16;
        }
      } break;
      case 3: {
        w = *cast(uint32_t *)s;
        *d++ = *s++;
        n -= 1;
        while (n > 18) {
          x = *cast(uint32_t *)(s+3);
          *cast(uint32_t *)(d+0)  = (w LS 8) | (x RS 24);
          w = *cast(uint32_t *)(s+7);
          *cast(uint32_t *)(d+4)  = (x LS 8) | (w RS 24);
          x = *cast(uint32_t *)(s+11);
          *cast(uint32_t *)(d+8)  = (w LS 8) | (x RS 24);
          w = *cast(uint32_t *)(s+15);
          *cast(uint32_t *)(d+12) = (x LS 8) | (w RS 24);

          s += 16;
          d += 16;
          n -= 16;
        }
      } break;
      default: break; // NOTE(bill): Do nowt!
    }
#undef LS
#undef RS
    if (n & 16) {
      *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
      *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
      *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
      *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
    }
    if (n & 8) {
      *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
      *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
    }
    if (n & 4)
      *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
    if (n & 2)
      *d++ = *s++; *d++ = *s++;
    if (n & 1)
      *d = *s;
  }

#endif
  return dest;
}

gb_inline void *gb_memmove(void *dest, void const *source, ssize_t n) {
  uint8_t *d = cast(uint8_t *) dest;
  uint8_t const *s = cast(uint8_t const *) source;

  if (d == s)
    return d;
  if (s + n <= d || d + n <= s) // NOTE(bill): Non-overlapping
    return gb_memcopy(d, s, n);

  if (d < s) {
    if (cast(uintptr_t) s % gb_size_of(ssize_t) == cast(uintptr_t) d % gb_size_of(ssize_t)) {
      while (cast(uintptr_t) d % gb_size_of(ssize_t)) {
        if (!n--) return dest;
        *d++ = *s++;
      }
      while (n >= gb_size_of(ssize_t)) {
        *cast(ssize_t *) d = *cast(ssize_t *) s;
        n -= gb_size_of(ssize_t);
        d += gb_size_of(ssize_t);
        s += gb_size_of(ssize_t);
      }
    }
    for (; n; n--) *d++ = *s++;
  } else {
    if ((cast(uintptr_t) s % gb_size_of(ssize_t)) == (cast(uintptr_t) d % gb_size_of(ssize_t))) {
      while (cast(uintptr_t) (d + n) % gb_size_of(ssize_t)) {
        if (!n--)
          return dest;
        d[n] = s[n];
      }
      while (n >= gb_size_of(ssize_t)) {
        n -= gb_size_of(ssize_t);
        *cast(ssize_t *) (d + n) = *cast(ssize_t *) (s + n);
      }
    }
    while (n) n--, d[n] = s[n];
  }

  return dest;
}

gb_inline void *gb_memset(void *dest, uint8_t c, ssize_t n) {
  uint8_t *s = cast(uint8_t *) dest;
  ssize_t k;
  uint32_t c32 = ((uint32_t) -1) / 255 * c;

  if (n == 0)
    return dest;
  s[0] = s[n - 1] = c;
  if (n < 3)
    return dest;
  s[1] = s[n - 2] = c;
  s[2] = s[n - 3] = c;
  if (n < 7)
    return dest;
  s[3] = s[n - 4] = c;
  if (n < 9)
    return dest;

  k = -cast(intptr_t) s & 3;
  s += k;
  n -= k;
  n &= -4;

  *cast(uint32_t *) (s + 0) = c32;
  *cast(uint32_t *) (s + n - 4) = c32;
  if (n < 9)
    return dest;
  *cast(uint32_t *) (s + 4) = c32;
  *cast(uint32_t *) (s + 8) = c32;
  *cast(uint32_t *) (s + n - 12) = c32;
  *cast(uint32_t *) (s + n - 8) = c32;
  if (n < 25)
    return dest;
  *cast(uint32_t *) (s + 12) = c32;
  *cast(uint32_t *) (s + 16) = c32;
  *cast(uint32_t *) (s + 20) = c32;
  *cast(uint32_t *) (s + 24) = c32;
  *cast(uint32_t *) (s + n - 28) = c32;
  *cast(uint32_t *) (s + n - 24) = c32;
  *cast(uint32_t *) (s + n - 20) = c32;
  *cast(uint32_t *) (s + n - 16) = c32;

  k = 24 + (cast(uintptr_t) s & 4);
  s += k;
  n -= k;

  {
    uint64_t c64 = (cast(uint64_t) c32 << 32) | c32;
    while (n > 31) {
      *cast(uint64_t *) (s + 0) = c64;
      *cast(uint64_t *) (s + 8) = c64;
      *cast(uint64_t *) (s + 16) = c64;
      *cast(uint64_t *) (s + 24) = c64;

      n -= 32;
      s += 32;
    }
  }

  return dest;
}

gb_inline int32_t gb_memcompare(void const *s1, void const *s2, ssize_t size) {
  // TODO(bill): Heavily optimize

  uint8_t const *s1p8 = cast(uint8_t const *) s1;
  uint8_t const *s2p8 = cast(uint8_t const *) s2;
  while (size--) {
    if (*s1p8 != *s2p8)
      return (*s1p8 - *s2p8);
    s1p8++, s2p8++;
  }
  return 0;
}

void gb_memswap(void *i, void *j, ssize_t size) {
  if (i == j) return;

  if (size == 4) {
    gb_swap(uint32_t, *cast(uint32_t * )
      i, *cast(uint32_t * )
              j);
  } else if (size == 8) {
    gb_swap(uint64_t, *cast(uint64_t * )
      i, *cast(uint64_t * )
              j);
  } else if (size < 8) {
    uint8_t *a = cast(uint8_t *) i;
    uint8_t *b = cast(uint8_t *) j;
    if (a != b) {
      while (size--) {
        gb_swap(uint8_t, *a, *b);
        a++, b++;
      }
    }
  } else {
    char buffer[256];

    // TODO(bill): Is the recursion ever a problem?
    while (size > gb_size_of(buffer)) {
      gb_memswap(i, j, gb_size_of(buffer));
      i = gb_pointer_add(i, gb_size_of(buffer));
      j = gb_pointer_add(j, gb_size_of(buffer));
      size -= gb_size_of(buffer);
    }

    gb_memcopy(buffer, i, size);
    gb_memcopy(i, j, size);
    gb_memcopy(j, buffer, size);
  }
}

void const *gb_memchr(void const *data, uint8_t c, ssize_t n) {
  uint8_t const *s = cast(uint8_t const *) data;
  while ((cast(uintptr_t) s & (sizeof(size_t) - 1)) &&
         n && *s != c) {
    s++;
    n--;
  }
  if (n && *s != c) {
    ssize_t const *w;
    ssize_t k = GB__ONES * c;
    w = cast(ssize_t const *) s;
    while (n >= gb_size_of(ssize_t) && !GB__HAS_ZERO(*w ^ k)) {
      w++;
      n -= gb_size_of(ssize_t);
    }
    s = cast(uint8_t const *) w;
    while (n && *s != c) {
      s++;
      n--;
    }
  }

  return n ? cast(void const *) s : NULL;
}

void const *gb_memrchr(void const *data, uint8_t c, ssize_t n) {
  uint8_t const *s = cast(uint8_t const *) data;
  while (n--) {
    if (s[n] == c)
      return cast(void const *) (s + n);
  }
  return NULL;
}
