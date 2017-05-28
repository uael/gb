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

#include "gb/string.h"

gb_inline void gb_str_to_lower(char *str) {
  if (!str) return;
  while (*str) {
    *str = gb_char_to_lower(*str);
    str++;
  }
}

gb_inline void gb_str_to_upper(char *str) {
  if (!str) return;
  while (*str) {
    *str = gb_char_to_upper(*str);
    str++;
  }
}

#define GB__ONES        (cast(size_t)-1/UINT8_MAX)
#define GB__HIGHS       (GB__ONES * (UINT8_MAX/2+1))
#define GB__HAS_ZERO(x) (((x)-GB__ONES) & ~(x) & GB__HIGHS)

gb_inline ssize_t gb_strlen(char const *str) {
  char const *begin = str;
  ssize_t const *w;
  while (cast(uintptr_t) str % sizeof(size_t)) {
    if (!*str)
      return str - begin;
    str++;
  }
  w = cast(ssize_t const *) str;
  while (!GB__HAS_ZERO(*w))
    w++;
  str = cast(char const *) w;
  while (*str)
    str++;
  return str - begin;
}

gb_inline ssize_t gb_strnlen(char const *str, ssize_t max_len) {
  char const *end = cast(char const *) gb_memchr(str, 0, max_len);
  if (end)
    return end - str;
  return max_len;
}

gb_inline ssize_t gb_utf8_strlen(uint8_t const *str) {
  ssize_t count = 0;
  for (; *str; count++) {
    uint8_t c = *str;
    ssize_t inc = 0;
    if (c < 0x80) inc = 1;
    else if ((c & 0xe0) == 0xc0) inc = 2;
    else if ((c & 0xf0) == 0xe0) inc = 3;
    else if ((c & 0xf8) == 0xf0) inc = 4;
    else return -1;

    str += inc;
  }
  return count;
}

gb_inline ssize_t gb_utf8_strnlen(uint8_t const *str, ssize_t max_len) {
  ssize_t count = 0;
  for (; *str && max_len > 0; count++) {
    uint8_t c = *str;
    ssize_t inc = 0;
    if (c < 0x80) inc = 1;
    else if ((c & 0xe0) == 0xc0) inc = 2;
    else if ((c & 0xf0) == 0xe0) inc = 3;
    else if ((c & 0xf8) == 0xf0) inc = 4;
    else return -1;

    str += inc;
    max_len -= inc;
  }
  return count;
}

gb_inline int32_t gb_strcmp(char const *s1, char const *s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++, s2++;
  }
  return *(uint8_t *) s1 - *(uint8_t *) s2;
}

gb_inline char *gb_strcpy(char *dest, char const *source) {
  GB_ASSERT_NOT_NULL(dest);
  if (source) {
    char *str = dest;
    while (*source) *str++ = *source++;
  }
  return dest;
}

gb_inline char *gb_strncpy(char *dest, char const *source, ssize_t len) {
  GB_ASSERT_NOT_NULL(dest);
  if (source) {
    char *str = dest;
    while (len > 0 && *source) {
      *str++ = *source++;
      len--;
    }
    while (len > 0) {
      *str++ = '\0';
      len--;
    }
  }
  return dest;
}

gb_inline ssize_t gb_strlcpy(char *dest, char const *source, ssize_t len) {
  ssize_t result = 0;
  GB_ASSERT_NOT_NULL(dest);
  if (source) {
    char const *source_start = source;
    char *str = dest;
    while (len > 0 && *source) {
      *str++ = *source++;
      len--;
    }
    while (len > 0) {
      *str++ = '\0';
      len--;
    }

    result = source - source_start;
  }
  return result;
}

gb_inline char *gb_strrev(char *str) {
  ssize_t len = gb_strlen(str);
  char *a = str + 0;
  char *b = str + len - 1;
  len /= 2;
  while (len--) {
    gb_swap(char, *a, *b);
    a++, b--;
  }
  return str;
}

gb_inline int32_t gb_strncmp(char const *s1, char const *s2, ssize_t len) {
  for (; len > 0;
         s1++, s2++, len--) {
    if (*s1 != *s2)
      return ((s1 < s2) ? -1 : +1);
    else if (*s1 == '\0')
      return 0;
  }
  return 0;
}

gb_inline char const *gb_strtok(char *output, char const *src, char const *delimit) {
  while (*src && gb_char_first_occurence(delimit, *src) != NULL)
    *output++ = *src++;

  *output = 0;
  return *src ? src + 1 : src;
}

gb_inline byte32_t gb_str_has_prefix(char const *str, char const *prefix) {
  while (*prefix) {
    if (*str++ != *prefix++)
      return false;
  }
  return true;
}

gb_inline byte32_t gb_str_has_suffix(char const *str, char const *suffix) {
  ssize_t i = gb_strlen(str);
  ssize_t j = gb_strlen(suffix);
  if (j <= i)
    return gb_strcmp(str + i - j, suffix) == 0;
  return false;
}

gb_inline char const *gb_char_first_occurence(char const *s, char c) {
  char ch = c;
  for (; *s != ch; s++) {
    if (*s == '\0')
      return NULL;
  }
  return s;
}

gb_inline char const *gb_char_last_occurence(char const *s, char c) {
  char const *result = NULL;
  do {
    if (*s == c)
      result = s;
  } while (*s++);

  return result;
}

gb_inline void gb_str_concat(char *dest, ssize_t dest_len,
                             char const *src_a, ssize_t src_a_len,
                             char const *src_b, ssize_t src_b_len) {
  GB_ASSERT(dest_len >= src_a_len + src_b_len + 1);
  if (dest) {
    gb_memcopy(dest, src_a, src_a_len);
    gb_memcopy(dest + src_a_len, src_b, src_b_len);
    dest[src_a_len + src_b_len] = '\0';
  }
}

gb_internal ssize_t gb__scan_i64(char const *text, int32_t base, int64_t *value) {
  char const *text_begin = text;
  int64_t result = 0;
  byte32_t negative = false;

  if (*text == '-') {
    negative = true;
    text++;
  }

  if (base == 16 && gb_strncmp(text, "0x", 2) == 0)
    text += 2;

  for (;;) {
    int64_t v;
    if (gb_char_is_digit(*text))
      v = *text - '0';
    else if (base == 16 && gb_char_is_hex_digit(*text))
      v = gb_hex_digit_to_int(*text);
    else
      break;

    result *= base;
    result += v;
    text++;
  }

  if (value) {
    if (negative) result = -result;
    *value = result;
  }

  return (text - text_begin);
}

gb_internal ssize_t gb__scan_u64(char const *text, int32_t base, uint64_t *value) {
  char const *text_begin = text;
  uint64_t result = 0;

  if (base == 16 && gb_strncmp(text, "0x", 2) == 0)
    text += 2;

  for (;;) {
    uint64_t v;
    if (gb_char_is_digit(*text))
      v = *text - '0';
    else if (base == 16 && gb_char_is_hex_digit(*text))
      v = gb_hex_digit_to_int(*text);
    else {
      break;
    }

    result *= base;
    result += v;
    text++;
  }

  if (value)
    *value = result;

  return (text - text_begin);
}

// TODO(bill): Make better
uint64_t gb_str_to_u64(char const *str, char **end_ptr, int32_t base) {
  ssize_t len;
  uint64_t value = 0;

  if (!base) {
    if ((gb_strlen(str) > 2) && (gb_strncmp(str, "0x", 2) == 0))
      base = 16;
    else
      base = 10;
  }

  len = gb__scan_u64(str, base, &value);
  if (end_ptr)
    *end_ptr = (char *) str + len;
  return value;
}

int64_t gb_str_to_i64(char const *str, char **end_ptr, int32_t base) {
  ssize_t len;
  int64_t value;

  if (!base) {
    if ((gb_strlen(str) > 2) && (gb_strncmp(str, "0x", 2) == 0))
      base = 16;
    else
      base = 10;
  }

  len = gb__scan_i64(str, base, &value);
  if (end_ptr)
    *end_ptr = (char *) str + len;
  return value;
}

// TODO(bill): Are these good enough for characters?
gb_global char const gb__num_to_char_table[] =
  "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "@$";

gb_inline void gb_i64_to_str(int64_t value, char *string, int32_t base) {
  char *buf = string;
  byte32_t negative = false;
  if (value < 0) {
    negative = true;
    value = -value;
  }
  if (value) {
    while (value > 0) {
      *buf++ = gb__num_to_char_table[value % base];
      value /= base;
    }
  } else {
    *buf++ = '0';
  }
  if (negative)
    *buf++ = '-';
  *buf = '\0';
  gb_strrev(string);
}

gb_inline void gb_u64_to_str(uint64_t value, char *string, int32_t base) {
  char *buf = string;

  if (value) {
    while (value > 0) {
      *buf++ = gb__num_to_char_table[value % base];
      value /= base;
    }
  } else {
    *buf++ = '0';
  }
  *buf = '\0';

  gb_strrev(string);
}

gb_inline float32_t gb_str_to_f32(char const *str, char **end_ptr) {
  float64_t f = gb_str_to_f64(str, end_ptr);
  float32_t r = cast(float32_t) f;
  return r;
}

gb_inline float64_t gb_str_to_f64(char const *str, char **end_ptr) {
  float64_t result, value, sign, scale;
  int32_t frac;

  while (gb_char_is_space(*str)) {
    str++;
  }

  sign = 1.0;
  if (*str == '-') {
    sign = -1.0;
    str++;
  } else if (*str == '+') {
    str++;
  }

  for (value = 0.0; gb_char_is_digit(*str); str++) {
    value = value * 10.0 + (*str - '0');
  }

  if (*str == '.') {
    float64_t pow10 = 10.0;
    str++;
    while (gb_char_is_digit(*str)) {
      value += (*str - '0') / pow10;
      pow10 *= 10.0;
      str++;
    }
  }

  frac = 0;
  scale = 1.0;
  if ((*str == 'e') || (*str == 'E')) {
    uint32_t exp;

    str++;
    if (*str == '-') {
      frac = 1;
      str++;
    } else if (*str == '+') {
      str++;
    }

    for (exp = 0; gb_char_is_digit(*str); str++) {
      exp = exp * 10 + (*str - '0');
    }
    if (exp > 308) exp = 308;

    while (exp >= 50) {
      scale *= 1e50;
      exp -= 50;
    }
    while (exp >= 8) {
      scale *= 1e8;
      exp -= 8;
    }
    while (exp > 0) {
      scale *= 10.0;
      exp -= 1;
    }
  }

  result = sign * (frac ? (value / scale) : (value * scale));

  if (end_ptr) *end_ptr = cast(char *) str;

  return result;
}

gb_inline void gb__set_string_length(gbString str, ssize_t len) { GB_STRING_HEADER(str)->length = len; }

gb_inline void gb__set_string_capacity(gbString str, ssize_t cap) { GB_STRING_HEADER(str)->capacity = cap; }

gb_inline gbString gb_string_make(gb_allocator_t a, char const *str) {
  ssize_t len = str ? gb_strlen(str) : 0;
  return gb_string_make_length(a, str, len);
}

gbString gb_string_make_length(gb_allocator_t a, void const *init_str, ssize_t num_bytes) {
  ssize_t header_size = gb_size_of(gbStringHeader);
  void *ptr = gb_alloc(a, header_size + num_bytes + 1);

  gbString str;
  gbStringHeader *header;

  if (!init_str) gb_zero_size(ptr, header_size + num_bytes + 1);
  if (ptr == NULL) return NULL;

  str = cast(char *) ptr + header_size;
  header = GB_STRING_HEADER(str);
  header->allocator = a;
  header->length = num_bytes;
  header->capacity = num_bytes;
  if (num_bytes && init_str)
    gb_memcopy(str, init_str, num_bytes);
  str[num_bytes] = '\0';

  return str;
}

gb_inline void gb_string_free(gbString str) {
  if (str) {
    gbStringHeader *header = GB_STRING_HEADER(str);
    gb_free(header->allocator, header);
  }

}

gb_inline gbString gb_string_duplicate(gb_allocator_t a, gbString const str) {
  return gb_string_make_length(a, str, gb_string_length(str));
}

gb_inline ssize_t gb_string_length(gbString const str) { return GB_STRING_HEADER(str)->length; }

gb_inline ssize_t gb_string_capacity(gbString const str) { return GB_STRING_HEADER(str)->capacity; }

gb_inline ssize_t gb_string_available_space(gbString const str) {
  gbStringHeader *h = GB_STRING_HEADER(str);
  if (h->capacity > h->length)
    return h->capacity - h->length;
  return 0;
}

gb_inline void gb_string_clear(gbString str) {
  gb__set_string_length(str, 0);
  str[0] = '\0';
}

gb_inline gbString gb_string_append(gbString str, gbString const other) {
  return gb_string_append_length(str, other, gb_string_length(other));
}

gbString gb_string_append_length(gbString str, void const *other, ssize_t other_len) {
  if (other_len > 0) {
    ssize_t curr_len = gb_string_length(str);

    str = gb_string_make_space_for(str, other_len);
    if (str == NULL)
      return NULL;

    gb_memcopy(str + curr_len, other, other_len);
    str[curr_len + other_len] = '\0';
    gb__set_string_length(str, curr_len + other_len);
  }
  return str;
}

gb_inline gbString gb_string_appendc(gbString str, char const *other) {
  return gb_string_append_length(str, other, gb_strlen(other));
}

gbString gb_string_set(gbString str, char const *cstr) {
  ssize_t len = gb_strlen(cstr);
  if (gb_string_capacity(str) < len) {
    str = gb_string_make_space_for(str, len - gb_string_length(str));
    if (str == NULL)
      return NULL;
  }

  gb_memcopy(str, cstr, len);
  str[len] = '\0';
  gb__set_string_length(str, len);

  return str;
}

gbString gb_string_make_space_for(gbString str, ssize_t add_len) {
  ssize_t available = gb_string_available_space(str);

// NOTE(bill): Return if there is enough space left
  if (available >= add_len) {
    return str;
  } else {
    ssize_t new_len, old_size, new_size;
    void *ptr, *new_ptr;
    gb_allocator_t a = GB_STRING_HEADER(str)->allocator;
    gbStringHeader *header;

    new_len = gb_string_length(str) + add_len;
    ptr = GB_STRING_HEADER(str);
    old_size = gb_size_of(gbStringHeader) + gb_string_length(str) + 1;
    new_size = gb_size_of(gbStringHeader) + new_len + 1;

    new_ptr = gb_resize(a, ptr, old_size, new_size);
    if (new_ptr == NULL) return NULL;

    header = cast(gbStringHeader *) new_ptr;
    header->allocator = a;

    str = cast(gbString) (header + 1);
    gb__set_string_capacity(str, new_len);

    return str;
  }
}

gb_inline ssize_t gb_string_allocation_size(gbString const str) {
  ssize_t cap = gb_string_capacity(str);
  return gb_size_of(gbStringHeader) + cap;
}

gb_inline byte32_t gb_string_are_equal(gbString const lhs, gbString const rhs) {
  ssize_t lhs_len, rhs_len, i;
  lhs_len = gb_string_length(lhs);
  rhs_len = gb_string_length(rhs);
  if (lhs_len != rhs_len)
    return false;

  for (i = 0; i < lhs_len; i++) {
    if (lhs[i] != rhs[i])
      return false;
  }

  return true;
}

gbString gb_string_trim(gbString str, char const *cut_set) {
  char *start, *end, *start_pos, *end_pos;
  ssize_t len;

  start_pos = start = str;
  end_pos = end = str + gb_string_length(str) - 1;

  while (start_pos <= end && gb_char_first_occurence(cut_set, *start_pos))
    start_pos++;
  while (end_pos > start_pos && gb_char_first_occurence(cut_set, *end_pos))
    end_pos--;

  len = cast(ssize_t) ((start_pos > end_pos) ? 0 : ((end_pos - start_pos) + 1));

  if (str != start_pos)
    gb_memmove(str, start_pos, len);
  str[len] = '\0';

  gb__set_string_length(str, len);

  return str;
}

gb_inline gbString gb_string_trim_space(gbString str) { return gb_string_trim(str, " \t\r\n\v\f"); }
