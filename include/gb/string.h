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

#ifndef  GB_STRING_H__
# define GB_STRING_H__

#include "gb/utf8.h"

// NOTE(bill): ASCII only
GB_DEF void gb_str_to_lower(char *str);

GB_DEF void gb_str_to_upper(char *str);

GB_DEF isize gb_strlen(char const *str);

GB_DEF isize gb_strnlen(char const *str, isize max_len);

GB_DEF i32 gb_strcmp(char const *s1, char const *s2);

GB_DEF i32 gb_strncmp(char const *s1, char const *s2, isize len);

GB_DEF char *gb_strcpy(char *dest, char const *source);

GB_DEF char *gb_strncpy(char *dest, char const *source, isize len);

GB_DEF isize gb_strlcpy(char *dest, char const *source, isize len);

GB_DEF char *gb_strrev(char *str); // NOTE(bill): ASCII only

// NOTE(bill): A less fucking crazy strtok!
GB_DEF char const *gb_strtok(char *output, char const *src, char const *delimit);

GB_DEF b32 gb_str_has_prefix(char const *str, char const *prefix);

GB_DEF b32 gb_str_has_suffix(char const *str, char const *suffix);

GB_DEF char const *gb_char_first_occurence(char const *str, char c);

GB_DEF char const *gb_char_last_occurence(char const *str, char c);

GB_DEF void gb_str_concat(char *dest, isize dest_len,
                          char const *src_a, isize src_a_len,
                          char const *src_b, isize src_b_len);

GB_DEF u64
gb_str_to_u64(char const *str, char **end_ptr, i32 base); // TODO(bill): Support more than just decimal and hexadecimal
GB_DEF i64
gb_str_to_i64(char const *str, char **end_ptr, i32 base); // TODO(bill): Support more than just decimal and hexadecimal
GB_DEF f32 gb_str_to_f32(char const *str, char **end_ptr);

GB_DEF f64 gb_str_to_f64(char const *str, char **end_ptr);

GB_DEF void gb_i64_to_str(i64 value, char *string, i32 base);

GB_DEF void gb_u64_to_str(u64 value, char *string, i32 base);

////////////////////////////////////////////////////////////////
//
// gbString - C Read-Only-Compatible
//
//
/*
Reasoning:

	By default, strings in C are null terminated which means you have to count
	the number of character up to the null character to calculate the length.
	Many "better" C string libraries will create a struct for a string.
	i.e.

	    struct String {
	    	Allocator allocator;
	        size_t    length;
	        size_t    capacity;
	        char *    cstring;
	    };

	This library tries to augment normal C strings in a better way that is still
	compatible with C-style strings.

	+--------+-----------------------+-----------------+
	| Header | Binary C-style String | Null Terminator |
	+--------+-----------------------+-----------------+
	         |
	         +-> Pointer returned by functions

	Due to the meta-data being stored before the string pointer and every gb string
	having an implicit null terminator, gb strings are full compatible with c-style
	strings and read-only functions.

Advantages:

    * gb strings can be passed to C-style string functions without accessing a struct
      member of calling a function, i.e.

          gb_printf("%s\n", gb_str);

      Many other libraries do either of these:

          gb_printf("%s\n", string->cstr);
          gb_printf("%s\n", get_cstring(string));

    * You can access each character just like a C-style string:

          gb_printf("%c %c\n", str[0], str[13]);

    * gb strings are singularly allocated. The meta-data is next to the character
      array which is better for the cache.

Disadvantages:

    * In the C version of these functions, many return the new string. i.e.
          str = gb_string_appendc(str, "another string");
      This could be changed to gb_string_appendc(&str, "another string"); but I'm still not sure.

	* This is incompatible with "gb_string.h" strings
*/

#if 0
#define GB_IMPLEMENTATION
#include "gb.h"
int main(int argc, char **argv) {
  gbString str = gb_string_make("Hello");
  gbString other_str = gb_string_make_length(", ", 2);
  str = gb_string_append(str, other_str);
  str = gb_string_appendc(str, "world!");

  gb_printf("%s\n", str); // Hello, world!

  gb_printf("str length = %d\n", gb_string_length(str));

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

  return 0;
}
#endif

// TODO(bill): Should this be a wrapper to gbArray(char) or this extra type safety better?
typedef char *gbString;

// NOTE(bill): If you only need a small string, just use a standard c string or change the size from isize to u16, etc.
typedef struct gbStringHeader {
  gbAllocator allocator;
  isize length;
  isize capacity;
} gbStringHeader;

#define GB_STRING_HEADER(str) (cast(gbStringHeader *)(str) - 1)

GB_DEF gbString gb_string_make(gbAllocator a, char const *str);

GB_DEF gbString gb_string_make_length(gbAllocator a, void const *str, isize num_bytes);

GB_DEF void gb_string_free(gbString str);

GB_DEF gbString gb_string_duplicate(gbAllocator a, gbString const str);

GB_DEF isize gb_string_length(gbString const str);

GB_DEF isize gb_string_capacity(gbString const str);

GB_DEF isize gb_string_available_space(gbString const str);

GB_DEF void gb_string_clear(gbString str);

GB_DEF gbString gb_string_append(gbString str, gbString const other);

GB_DEF gbString gb_string_append_length(gbString str, void const *other, isize num_bytes);

GB_DEF gbString gb_string_appendc(gbString str, char const *other);

GB_DEF gbString gb_string_set(gbString str, char const *cstr);

GB_DEF gbString gb_string_make_space_for(gbString str, isize add_len);

GB_DEF isize gb_string_allocation_size(gbString const str);

GB_DEF b32 gb_string_are_equal(gbString const lhs, gbString const rhs);

GB_DEF gbString gb_string_trim(gbString str, char const *cut_set);

GB_DEF gbString gb_string_trim_space(gbString str); // Whitespace ` \t\r\n\v\f`

#endif /* GB_STRING_H__ */
