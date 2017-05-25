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

#ifndef  GB_BUFFER_H__
# define GB_BUFFER_H__

#include "gb/string.h"

//
// Fixed Capacity Buffer (POD Types)
//
//
// gbBuffer(Type) works like gbString or gbArray where the actual type is just a pointer to the first
// element.
//

typedef struct gbBufferHeader {
  isize count;
  isize capacity;
} gbBufferHeader;

#define gbBuffer(Type) Type *

#define GB_BUFFER_HEADER(x)   (cast(gbBufferHeader *)(x) - 1)
#define gb_buffer_count(x)    (GB_BUFFER_HEADER(x)->count)
#define gb_buffer_capacity(x) (GB_BUFFER_HEADER(x)->capacity)

#define gb_buffer_init(x, allocator, cap) do { \
  void **nx = cast(void **)&(x); \
  gbBufferHeader *gb__bh = cast(gbBufferHeader *)gb_alloc((allocator), (cap)*gb_size_of(*(x))); \
  gb__bh->count = 0; \
  gb__bh->capacity = cap; \
  *nx = cast(void *)(gb__bh+1); \
} while (0)

#define gb_buffer_free(x, allocator) (gb_free(allocator, GB_BUFFER_HEADER(x)))

#define gb_buffer_append(x, item) do { (x)[gb_buffer_count(x)++] = (item); } while (0)

#define gb_buffer_appendv(x, items, item_count) do { \
  GB_ASSERT(gb_size_of(*(items)) == gb_size_of(*(x))); \
  GB_ASSERT(gb_buffer_count(x)+item_count <= gb_buffer_capacity(x)); \
  gb_memcopy(&(x)[gb_buffer_count(x)], (items), gb_size_of(*(x))*(item_count)); \
  gb_buffer_count(x) += (item_count); \
} while (0)

#define gb_buffer_pop(x)   do { GB_ASSERT(gb_buffer_count(x) > 0); gb_buffer_count(x)--; } while (0)
#define gb_buffer_clear(x) do { gb_buffer_count(x) = 0; } while (0)

#endif /* GB_BUFFER_H__ */
