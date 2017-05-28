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

#include <cute.h>

#include "gb/array.h"
#include "gb/io.h"

int main(void) {
  ssize_t i;
  int test_values[] = {4, 2, 1, 7};
  gb_allocator_t a = gb_heap_allocator();
  int *items = {0};

  gb_array_init(items, a);

  gb_array_append(items, 1);
  gb_array_append(items, 4);
  gb_array_append(items, 9);
  gb_array_append(items, 16);

  items[1] = 3; // Manually set value
  // NOTE: No array bounds checking

  for (i = 0; i < gb_array_count(items); i++)
    gb_printf("%d\n", items[i]);
  // 1
  // 3
  // 9
  // 16

  gb_array_clear(items);

  gb_array_appendv(items, test_values, gb_count_of(test_values));
  for (i = 0; i < gb_array_count(items); i++)
    gb_printf("%d\n", items[i]);
  // 4
  // 2
  // 1
  // 7

  gb_array_free(items);
  return EXIT_SUCCESS;
}
