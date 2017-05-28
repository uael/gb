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

#include "gb/string.h"
#include "gb/io.h"

int main(void) {
  char *str = gb_string_make(gb_heap_allocator(), "Hello");
  char *other_str = gb_string_make_length(gb_heap_allocator(), ", ", 2);
  str = gb_string_append(str, other_str);
  str = gb_string_appendc(str, "world!");

  gb_printf("%s\n", str); // Hello, world!

  gb_printf("str length = %zu\n", gb_string_length(str));

  str = gb_string_set(str, "Potato soup");
  gb_printf("%s\n", str); // Potato soup

  str = gb_string_set(str, "Hello");
  other_str = gb_string_set(other_str, "Pizza");
  if (gb_strings_are_equal(str, other_str))
    gb_printf("Not called\n");
  else
    gb_printf("Called\n");

  str = gb_string_set(str, "Ab.;!...AHello World       ??");
  str = gb_string_trim(str, "Ab.;!. ?");
  gb_printf("%s\n", str); // "Hello World"

  gb_string_free(str);
  gb_string_free(other_str);

  return EXIT_SUCCESS;
}
