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

#define GB__ONES        (cast(usize)-1/U8_MAX)
#define GB__HIGHS       (GB__ONES * (U8_MAX/2+1))
#define GB__HAS_ZERO(x) (((x)-GB__ONES) & ~(x) & GB__HIGHS)

b32 gb_is_power_of_two(isize x) {
  if (x <= 0)
    return false;
  return !(x & (x - 1));
}

gb_inline void *gb_align_forward(void *ptr, isize alignment) {
  uintptr p;
  isize modulo;

  GB_ASSERT(gb_is_power_of_two(alignment));

  p = cast(uintptr) ptr;
  modulo = p & (alignment - 1);
  if (modulo) p += (alignment - modulo);
  return cast(void *) p;
}

gb_inline void *gb_pointer_add(void *ptr, isize bytes) { return cast(void *) (cast(u8 *) ptr + bytes); }

gb_inline void *gb_pointer_sub(void *ptr, isize bytes) { return cast(void *) (cast(u8 *) ptr - bytes); }

gb_inline void const *gb_pointer_add_const(void const *ptr, isize bytes) {
  return cast(void const *) (cast(u8 const *) ptr + bytes);
}

gb_inline void const *gb_pointer_sub_const(void const *ptr, isize bytes) {
  return cast(void const *) (cast(u8 const *) ptr - bytes);
}

gb_inline isize gb_pointer_diff(void const *begin, void const *end) {
  return cast(isize) (cast(u8 const *) end - cast(u8 const *) begin);
}

gb_inline void gb_zero_size(void *ptr, isize size) { gb_memset(ptr, 0, size); }

#if defined(_MSC_VER)
#pragma intrinsic(__movsb)
#endif

gb_inline void *gb_memcopy(void *dest, void const *source, isize n) {
#if defined(_MSC_VER)
  // TODO(bill): Is this good enough?
  __movsb(cast(u8 *)dest, cast(u8 *)source, n);
#elif GB_ARCH_X86 || GB_ARCH_X86_64
  __asm__ __volatile__("rep movsb" : "+D"(dest), "+S"(source), "+c"(n) : : "memory");
#else
  u8 *d = cast(u8 *)dest;
  u8 const *s = cast(u8 const *)source;
  u32 w, x;

  for (; cast(uintptr)s % 4 && n; n--)
  *d++ = *s++;

  if (cast(uintptr)d % 4 == 0) {
    for (; n >= 16;
           s += 16, d += 16, n -= 16) {
      *cast(u32 *)(d+ 0) = *cast(u32 *)(s+ 0);
      *cast(u32 *)(d+ 4) = *cast(u32 *)(s+ 4);
      *cast(u32 *)(d+ 8) = *cast(u32 *)(s+ 8);
      *cast(u32 *)(d+12) = *cast(u32 *)(s+12);
    }
    if (n & 8) {
      *cast(u32 *)(d+0) = *cast(u32 *)(s+0);
      *cast(u32 *)(d+4) = *cast(u32 *)(s+4);
      d += 8;
      s += 8;
    }
    if (n&4) {
      *cast(u32 *)(d+0) = *cast(u32 *)(s+0);
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
    switch (cast(uintptr)d % 4) {
      case 1: {
        w = *cast(u32 *)s;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        n -= 3;
        while (n > 16) {
          x = *cast(u32 *)(s+1);
          *cast(u32 *)(d+0)  = (w LS 24) | (x RS 8);
          w = *cast(u32 *)(s+5);
          *cast(u32 *)(d+4)  = (x LS 24) | (w RS 8);
          x = *cast(u32 *)(s+9);
          *cast(u32 *)(d+8)  = (w LS 24) | (x RS 8);
          w = *cast(u32 *)(s+13);
          *cast(u32 *)(d+12) = (x LS 24) | (w RS 8);

          s += 16;
          d += 16;
          n -= 16;
        }
      } break;
      case 2: {
        w = *cast(u32 *)s;
        *d++ = *s++;
        *d++ = *s++;
        n -= 2;
        while (n > 17) {
          x = *cast(u32 *)(s+2);
          *cast(u32 *)(d+0)  = (w LS 16) | (x RS 16);
          w = *cast(u32 *)(s+6);
          *cast(u32 *)(d+4)  = (x LS 16) | (w RS 16);
          x = *cast(u32 *)(s+10);
          *cast(u32 *)(d+8)  = (w LS 16) | (x RS 16);
          w = *cast(u32 *)(s+14);
          *cast(u32 *)(d+12) = (x LS 16) | (w RS 16);

          s += 16;
          d += 16;
          n -= 16;
        }
      } break;
      case 3: {
        w = *cast(u32 *)s;
        *d++ = *s++;
        n -= 1;
        while (n > 18) {
          x = *cast(u32 *)(s+3);
          *cast(u32 *)(d+0)  = (w LS 8) | (x RS 24);
          w = *cast(u32 *)(s+7);
          *cast(u32 *)(d+4)  = (x LS 8) | (w RS 24);
          x = *cast(u32 *)(s+11);
          *cast(u32 *)(d+8)  = (w LS 8) | (x RS 24);
          w = *cast(u32 *)(s+15);
          *cast(u32 *)(d+12) = (x LS 8) | (w RS 24);

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

gb_inline void *gb_memmove(void *dest, void const *source, isize n) {
  u8 *d = cast(u8 *) dest;
  u8 const *s = cast(u8 const *) source;

  if (d == s)
    return d;
  if (s + n <= d || d + n <= s) // NOTE(bill): Non-overlapping
    return gb_memcopy(d, s, n);

  if (d < s) {
    if (cast(uintptr) s % gb_size_of(isize) == cast(uintptr) d % gb_size_of(isize)) {
      while (cast(uintptr) d % gb_size_of(isize)) {
        if (!n--) return dest;
        *d++ = *s++;
      }
      while (n >= gb_size_of(isize)) {
        *cast(isize *) d = *cast(isize *) s;
        n -= gb_size_of(isize);
        d += gb_size_of(isize);
        s += gb_size_of(isize);
      }
    }
    for (; n; n--) *d++ = *s++;
  } else {
    if ((cast(uintptr) s % gb_size_of(isize)) == (cast(uintptr) d % gb_size_of(isize))) {
      while (cast(uintptr) (d + n) % gb_size_of(isize)) {
        if (!n--)
          return dest;
        d[n] = s[n];
      }
      while (n >= gb_size_of(isize)) {
        n -= gb_size_of(isize);
        *cast(isize *) (d + n) = *cast(isize *) (s + n);
      }
    }
    while (n) n--, d[n] = s[n];
  }

  return dest;
}

gb_inline void *gb_memset(void *dest, u8 c, isize n) {
  u8 *s = cast(u8 *) dest;
  isize k;
  u32 c32 = ((u32) -1) / 255 * c;

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

  k = -cast(intptr) s & 3;
  s += k;
  n -= k;
  n &= -4;

  *cast(u32 *) (s + 0) = c32;
  *cast(u32 *) (s + n - 4) = c32;
  if (n < 9)
    return dest;
  *cast(u32 *) (s + 4) = c32;
  *cast(u32 *) (s + 8) = c32;
  *cast(u32 *) (s + n - 12) = c32;
  *cast(u32 *) (s + n - 8) = c32;
  if (n < 25)
    return dest;
  *cast(u32 *) (s + 12) = c32;
  *cast(u32 *) (s + 16) = c32;
  *cast(u32 *) (s + 20) = c32;
  *cast(u32 *) (s + 24) = c32;
  *cast(u32 *) (s + n - 28) = c32;
  *cast(u32 *) (s + n - 24) = c32;
  *cast(u32 *) (s + n - 20) = c32;
  *cast(u32 *) (s + n - 16) = c32;

  k = 24 + (cast(uintptr) s & 4);
  s += k;
  n -= k;

  {
    u64 c64 = (cast(u64) c32 << 32) | c32;
    while (n > 31) {
      *cast(u64 *) (s + 0) = c64;
      *cast(u64 *) (s + 8) = c64;
      *cast(u64 *) (s + 16) = c64;
      *cast(u64 *) (s + 24) = c64;

      n -= 32;
      s += 32;
    }
  }

  return dest;
}

gb_inline i32 gb_memcompare(void const *s1, void const *s2, isize size) {
  // TODO(bill): Heavily optimize

  u8 const *s1p8 = cast(u8 const *) s1;
  u8 const *s2p8 = cast(u8 const *) s2;
  while (size--) {
    if (*s1p8 != *s2p8)
      return (*s1p8 - *s2p8);
    s1p8++, s2p8++;
  }
  return 0;
}

void gb_memswap(void *i, void *j, isize size) {
  if (i == j) return;

  if (size == 4) {
    gb_swap(u32, *cast(u32 * )
      i, *cast(u32 * )
              j);
  } else if (size == 8) {
    gb_swap(u64, *cast(u64 * )
      i, *cast(u64 * )
              j);
  } else if (size < 8) {
    u8 *a = cast(u8 *) i;
    u8 *b = cast(u8 *) j;
    if (a != b) {
      while (size--) {
        gb_swap(u8, *a, *b);
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

void const *gb_memchr(void const *data, u8 c, isize n) {
  u8 const *s = cast(u8 const *) data;
  while ((cast(uintptr) s & (sizeof(usize) - 1)) &&
         n && *s != c) {
    s++;
    n--;
  }
  if (n && *s != c) {
    isize const *w;
    isize k = GB__ONES * c;
    w = cast(isize const *) s;
    while (n >= gb_size_of(isize) && !GB__HAS_ZERO(*w ^ k)) {
      w++;
      n -= gb_size_of(isize);
    }
    s = cast(u8 const *) w;
    while (n && *s != c) {
      s++;
      n--;
    }
  }

  return n ? cast(void const *) s : NULL;
}

void const *gb_memrchr(void const *data, u8 c, isize n) {
  u8 const *s = cast(u8 const *) data;
  while (n--) {
    if (s[n] == c)
      return cast(void const *) (s + n);
  }
  return NULL;
}
