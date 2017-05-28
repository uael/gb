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

#include "gb/sort.h"
#include "gb/string.h"
#include "gb/random.h"

// TODO(bill): Should I make all the macros local?

#define GB__COMPARE_PROC(Type) \
gb_global ssize_t gb__##Type##_cmp_offset; GB_COMPARE_PROC(gb__##Type##_cmp) { \
  Type const p = *cast(Type const *)gb_pointer_add_const(a, gb__##Type##_cmp_offset); \
  Type const q = *cast(Type const *)gb_pointer_add_const(b, gb__##Type##_cmp_offset); \
  return p < q ? -1 : p > q; \
} \
GB_COMPARE_PROC_PTR(gb_##Type##_cmp(ssize_t offset)) { \
  gb__##Type##_cmp_offset = offset; \
  return &gb__##Type##_cmp; \
}

GB__COMPARE_PROC(int16_t);
GB__COMPARE_PROC(int32_t);
GB__COMPARE_PROC(int64_t);
GB__COMPARE_PROC(ssize_t);
GB__COMPARE_PROC(float32_t);
GB__COMPARE_PROC(float64_t);
GB__COMPARE_PROC(char);

// NOTE(bill): str_cmp is special as it requires a funny type and funny comparison
gb_global ssize_t gb__str_cmp_offset;

GB_COMPARE_PROC(gb__str_cmp) {
  char const *p = *cast(char const **) gb_pointer_add_const(a, gb__str_cmp_offset);
  char const *q = *cast(char const **) gb_pointer_add_const(b, gb__str_cmp_offset);
  return gb_strcmp(p, q);
}

GB_COMPARE_PROC_PTR(gb_str_cmp(ssize_t
                      offset)) {
  gb__str_cmp_offset = offset;
  return &gb__str_cmp;
}

#undef GB__COMPARE_PROC




// TODO(bill): Make user definable?
#define GB__SORT_STACK_SIZE            64
#define GB__SORT_INSERT_SORT_THRESHOLD  8

#define GB__SORT_PUSH(_base, _limit) do { \
  stack_ptr[0] = (_base); \
  stack_ptr[1] = (_limit); \
  stack_ptr += 2; \
} while (0)

#define GB__SORT_POP(_base, _limit) do { \
  stack_ptr -= 2; \
  (_base)  = stack_ptr[0]; \
  (_limit) = stack_ptr[1]; \
} while (0)

void gb_sort(void *base_, ssize_t count, ssize_t size, gbCompareProc cmp) {
  uint8_t *i, *j;
  uint8_t *base = cast(uint8_t *) base_;
  uint8_t *limit = base + count * size;
  ssize_t threshold = GB__SORT_INSERT_SORT_THRESHOLD * size;

  // NOTE(bill): Prepare the stack
  uint8_t *stack[GB__SORT_STACK_SIZE] = {0};
  uint8_t **stack_ptr = stack;

  for (;;) {
    if ((limit - base) > threshold) {
      // NOTE(bill): Quick sort
      i = base + size;
      j = limit - size;

      gb_memswap(((limit - base) / size / 2) * size + base, base, size);
      if (cmp(i, j) > 0) gb_memswap(i, j, size);
      if (cmp(base, j) > 0) gb_memswap(base, j, size);
      if (cmp(i, base) > 0) gb_memswap(i, base, size);

      for (;;) {
        do i += size; while (cmp(i, base) < 0);
        do j -= size; while (cmp(j, base) > 0);
        if (i > j) break;
        gb_memswap(i, j, size);
      }

      gb_memswap(base, j, size);

      if (j - base > limit - i) {
        GB__SORT_PUSH(base, j);
        base = i;
      } else {
        GB__SORT_PUSH(i, limit);
        limit = j;
      }
    } else {
      // NOTE(bill): Insertion sort
      for (j = base, i = j + size;
           i < limit;
           j = i, i += size) {
        for (; cmp(j, j + size) > 0; j -= size) {
          gb_memswap(j, j + size, size);
          if (j == base) break;
        }
      }

      if (stack_ptr == stack) break; // NOTE(bill): Sorting is done!
      GB__SORT_POP(base, limit);
    }
  }
}

#undef GB__SORT_PUSH
#undef GB__SORT_POP

#define GB_RADIX_SORT_PROC_GEN(Type) GB_RADIX_SORT_PROC(Type) { \
  Type *source = items; \
  Type *dest   = temp; \
  ssize_t byte_index, i, byte_max = 8*gb_size_of(Type); \
  for (byte_index = 0; byte_index < byte_max; byte_index += 8) { \
    ssize_t offsets[256] = {0}; \
    ssize_t total = 0; \
    /* NOTE(bill): First pass - count how many of each key */ \
    for (i = 0; i < count; i++) { \
      Type radix_value = source[i]; \
      Type radix_piece = (radix_value >> byte_index) & 0xff; \
      offsets[radix_piece]++; \
    } \
    /* NOTE(bill): Change counts to offsets */ \
    for (i = 0; i < gb_count_of(offsets); i++) { \
      ssize_t skcount = offsets[i]; \
      offsets[i] = total; \
      total += skcount; \
    } \
    /* NOTE(bill): Second pass - place elements into the right location */ \
    for (i = 0; i < count; i++) { \
      Type radix_value = source[i]; \
      Type radix_piece = (radix_value >> byte_index) & 0xff; \
      dest[offsets[radix_piece]++] = source[i]; \
    } \
    gb_swap(Type *, source, dest); \
  } \
}

GB_RADIX_SORT_PROC_GEN(uint8_t);

GB_RADIX_SORT_PROC_GEN(uint16_t);

GB_RADIX_SORT_PROC_GEN(uint32_t);

GB_RADIX_SORT_PROC_GEN(uint64_t);

gb_inline ssize_t
gb_binary_search(void const *base, ssize_t count, ssize_t size, void const *key, gbCompareProc compare_proc) {
  ssize_t start = 0;
  ssize_t end = count;

  while (start < end) {
    ssize_t mid = start + (end - start) / 2;
    ssize_t result = compare_proc(key, cast(uint8_t *) base + mid * size);
    if (result < 0)
      end = mid;
    else if (result > 0)
      start = mid + 1;
    else
      return mid;
  }

  return -1;
}

void gb_shuffle(void *base, ssize_t count, ssize_t size) {
  uint8_t *a;
  ssize_t i, j;
  gbRandom random;
  gb_random_init(&random);

  a = cast(uint8_t *) base + (count - 1) * size;
  for (i = count; i > 1; i--) {
    j = gb_random_gen_isize(&random) % i;
    gb_memswap(a, cast(uint8_t *) base + j * size, size);
    a -= size;
  }
}

void gb_reverse(void *base, ssize_t count, ssize_t size) {
  ssize_t i, j = count - 1;
  for (i = 0; i < j; i++, j++)
    gb_memswap(cast(uint8_t *) base + i * size, cast(uint8_t *) base + j * size, size);
}
