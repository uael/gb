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

#ifndef  GB_MEMORY_H__
# define GB_MEMORY_H__

#include "gb/assert.h"

GB_DEF byte32_t gb_is_power_of_two(ssize_t x);

GB_DEF void *gb_align_forward(void *ptr, ssize_t alignment);

GB_DEF void *gb_pointer_add(void *ptr, ssize_t bytes);

GB_DEF void *gb_pointer_sub(void *ptr, ssize_t bytes);

GB_DEF void const *gb_pointer_add_const(void const *ptr, ssize_t bytes);

GB_DEF void const *gb_pointer_sub_const(void const *ptr, ssize_t bytes);

GB_DEF ssize_t gb_pointer_diff(void const *begin, void const *end);

GB_DEF void gb_zero_size(void *ptr, ssize_t size);

#ifndef     gb_zero_item
#define     gb_zero_item(t)         gb_zero_size((t), gb_size_of(*(t))) // NOTE(bill): Pass pointer of struct
#define     gb_zero_array(a, count) gb_zero_size((a), gb_size_of(*(a))*count)
#endif

GB_DEF void *gb_memcopy(void *dest, void const *source, ssize_t size);

GB_DEF void *gb_memmove(void *dest, void const *source, ssize_t size);

GB_DEF void *gb_memset(void *data, uint8_t byte_value, ssize_t size);

GB_DEF int32_t gb_memcompare(void const *s1, void const *s2, ssize_t size);

GB_DEF void gb_memswap(void *i, void *j, ssize_t size);

GB_DEF void const *gb_memchr(void const *data, uint8_t byte_value, ssize_t size);

GB_DEF void const *gb_memrchr(void const *data, uint8_t byte_value, ssize_t size);


// NOTE(bill): Very similar to doing `*cast(T *)(&u)`
#ifndef GB_BIT_CAST
#define GB_BIT_CAST(dest, source) do { \
  GB_STATIC_ASSERT(gb_size_of(*(dest)) <= gb_size_of(source)); \
  gb_memcopy((dest), &(source), gb_size_of(*dest)); \
} while (0)
#endif

#ifndef gb_kilobytes
#define gb_kilobytes(x) (            (x) * (int64_t)(1024))
#define gb_megabytes(x) (gb_kilobytes(x) * (int64_t)(1024))
#define gb_gigabytes(x) (gb_megabytes(x) * (int64_t)(1024))
#define gb_terabytes(x) (gb_gigabytes(x) * (int64_t)(1024))
#endif

#endif /* GB_MEMORY_H__ */
