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

#ifndef  GB_SORT_H__
# define GB_SORT_H__

#include "gb/alloc.h"

#define GB_COMPARE_PROC(name) int name(void const *a, void const *b)

typedef GB_COMPARE_PROC(gbCompareProc);

#define GB_COMPARE_PROC_PTR(def) GB_COMPARE_PROC((*def))

// Producure pointers
// NOTE(bill): The offset parameter specifies the offset in the structure
// e.g. gb_i32_cmp(gb_offset_of(Thing, value))
// Use 0 if it's just the type instead.

GB_DEF GB_COMPARE_PROC_PTR(gb_i16_cmp(ssize_t
                             offset));

GB_DEF GB_COMPARE_PROC_PTR(gb_i32_cmp(ssize_t
                             offset));

GB_DEF GB_COMPARE_PROC_PTR(gb_i64_cmp(ssize_t
                             offset));

GB_DEF GB_COMPARE_PROC_PTR(gb_isize_cmp(ssize_t
                             offset));

GB_DEF GB_COMPARE_PROC_PTR(gb_str_cmp(ssize_t
                             offset));

GB_DEF GB_COMPARE_PROC_PTR(gb_f32_cmp(ssize_t
                             offset));

GB_DEF GB_COMPARE_PROC_PTR(gb_f64_cmp(ssize_t
                             offset));

GB_DEF GB_COMPARE_PROC_PTR(gb_char_cmp(ssize_t
                             offset));

// TODO(bill): Better sorting algorithms
// NOTE(bill): Uses quick sort for large arrays but insertion sort for small
#define gb_sort_array(array, count, compare_proc) gb_sort(array, count, gb_size_of(*(array)), compare_proc)

GB_DEF void gb_sort(void *base, ssize_t count, ssize_t size, gbCompareProc compare_proc);

// NOTE(bill): the count of temp == count of items
#define gb_radix_sort(Type) gb_radix_sort_##Type
#define GB_RADIX_SORT_PROC(Type) void gb_radix_sort(Type)(Type *items, Type *temp, ssize_t count)

GB_DEF GB_RADIX_SORT_PROC(uint8_t);

GB_DEF GB_RADIX_SORT_PROC(uint16_t);

GB_DEF GB_RADIX_SORT_PROC(uint32_t);

GB_DEF GB_RADIX_SORT_PROC(uint64_t);


// NOTE(bill): Returns index or -1 if not found
#define gb_binary_search_array(array, count, key, compare_proc) gb_binary_search(array, count, gb_size_of(*(array)), key, compare_proc)

GB_DEF ssize_t gb_binary_search(void const *base, ssize_t count, ssize_t size, void const *key, gbCompareProc compare_proc);

#define gb_shuffle_array(array, count) gb_shuffle(array, count, gb_size_of(*(array)))

GB_DEF void gb_shuffle(void *base, ssize_t count, ssize_t size);

#define gb_reverse_array(array, count) gb_reverse(array, count, gb_size_of(*(array)))

GB_DEF void gb_reverse(void *base, ssize_t count, ssize_t size);

#endif /* GB_SORT_H__ */
