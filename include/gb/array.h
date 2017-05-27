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

#ifndef  GB_ARRAY_H__
# define GB_ARRAY_H__

#include "gb/buffer.h"

//
// Dynamic Array (POD Types)
//
// NOTE(bill): I know this is a macro hell but C is an old (and shit) language with no proper arrays
// Also why the fuck not?! It fucking works! And it has custom allocation, which is already better than C++!
//
// gbArray(Type) works like gbString or gbBuffer where the actual type is just a pointer to the first
// element.
//



// Available Procedures for gbArray(Type)
// gb_array_init
// gb_array_free
// gb_array_set_capacity
// gb_array_grow
// gb_array_append
// gb_array_appendv
// gb_array_pop
// gb_array_clear
// gb_array_resize
// gb_array_reserve
//

#if 0 // Example
void foo(void) {
  isize i;
  int test_values[] = {4, 2, 1, 7};
  gbAllocator a = gb_heap_allocator();
  gbArray(int) items;

  gb_array_init(items, a);

  gb_array_append(items, 1);
  gb_array_append(items, 4);
  gb_array_append(items, 9);
  gb_array_append(items, 16);

  items[1] = 3; // Manually set value
                // NOTE: No array bounds checking

  for (i = 0; i < items.count; i++)
    gb_printf("%d\n", items[i]);
  // 1
  // 3
  // 9
  // 16

  gb_array_clear(items);

  gb_array_appendv(items, test_values, gb_count_of(test_values));
  for (i = 0; i < items.count; i++)
    gb_printf("%d\n", items[i]);
  // 4
  // 2
  // 1
  // 7

  gb_array_free(items);
}
#endif

typedef struct gbArrayHeader {
  gbAllocator allocator;
  isize count;
  isize capacity;
} gbArrayHeader;

// NOTE(bill): This thing is magic!
#define gbArray(Type) Type *

#ifndef GB_ARRAY_GROW_FORMULA
#define GB_ARRAY_GROW_FORMULA(x) (2*(x) + 8)
#endif

GB_STATIC_ASSERT(GB_ARRAY_GROW_FORMULA(0) > 0);

#define GB_ARRAY_HEADER(x)    (cast(gbArrayHeader *)(x) - 1)
#define gb_array_allocator(x) (GB_ARRAY_HEADER(x)->allocator)
#define gb_array_count(x)     (GB_ARRAY_HEADER(x)->count)
#define gb_array_capacity(x)  (GB_ARRAY_HEADER(x)->capacity)

// TODO(bill): Have proper alignment!
#define gb_array_init_reserve(x, allocator_, cap) do { \
  void **gb__array_ = cast(void **)&(x); \
  gbArrayHeader *gb__ah = cast(gbArrayHeader *)gb_alloc(allocator_, gb_size_of(gbArrayHeader)+gb_size_of(*(x))*(cap)); \
  gb__ah->allocator = allocator_; \
  gb__ah->count = 0; \
  gb__ah->capacity = cap; \
  *gb__array_ = cast(void *)(gb__ah+1); \
} while (0)

// NOTE(bill): Give it an initial default capacity
#define gb_array_init(x, allocator) gb_array_init_reserve(x, allocator, GB_ARRAY_GROW_FORMULA(0))

#define gb_array_free(x) do { \
  gbArrayHeader *gb__ah = GB_ARRAY_HEADER(x); \
  gb_free(gb__ah->allocator, gb__ah); \
} while (0)

#define gb_array_set_capacity(x, capacity) do { \
  if (x) { \
    void **gb__array_ = cast(void **)&(x); \
    *gb__array_ = gb__array_set_capacity((x), (capacity), gb_size_of(*(x))); \
  } \
} while (0)

// NOTE(bill): Do not use the thing below directly, use the macro
GB_API void *gb__array_set_capacity(void *array, isize capacity, isize element_size);


// TODO(bill): Decide on a decent growing formula for gbArray
#define gb_array_grow(x, min_capacity) do { \
  isize new_capacity = GB_ARRAY_GROW_FORMULA(gb_array_capacity(x)); \
  if (new_capacity < (min_capacity)) \
    new_capacity = (min_capacity); \
  gb_array_set_capacity(x, new_capacity); \
} while (0)

#define gb_array_append(x, item) do { \
  if (gb_array_capacity(x) < gb_array_count(x)+1) \
    gb_array_grow(x, 0); \
  (x)[gb_array_count(x)++] = (item); \
} while (0)

#define gb_array_appendv(x, items, item_count) do { \
  gbArrayHeader *gb__ah = GB_ARRAY_HEADER(x); \
  GB_ASSERT(gb_size_of((items)[0]) == gb_size_of((x)[0])); \
  if (gb__ah->capacity < gb__ah->count+(item_count)) \
    gb_array_grow(x, gb__ah->count+(item_count)); \
  gb_memcopy(&(x)[gb__ah->count], (items), gb_size_of((x)[0])*(item_count));\
  gb__ah->count += (item_count); \
} while (0)

#define gb_array_pop(x)   do { GB_ASSERT(GB_ARRAY_HEADER(x)->count > 0); GB_ARRAY_HEADER(x)->count--; } while (0)
#define gb_array_clear(x) do { GB_ARRAY_HEADER(x)->count = 0; } while (0)

#define gb_array_resize(x, new_count) do { \
  if (GB_ARRAY_HEADER(x)->capacity < (new_count)) \
    gb_array_grow(x, (new_count)); \
  GB_ARRAY_HEADER(x)->count = (new_count); \
} while (0)

#define gb_array_reserve(x, new_capacity) do { \
  if (GB_ARRAY_HEADER(x)->capacity < (new_capacity)) \
    gb_array_set_capacity(x, new_capacity); \
} while (0)

#endif /* GB_ARRAY_H__ */
