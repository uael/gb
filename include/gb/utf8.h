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

#ifndef  GB_UTF8_H__
# define GB_UTF8_H__

#include "gb/ctype.h"

// NOTE(bill): Does not check if utf-8 string is valid
GB_DEF isize gb_utf8_strlen(u8 const *str);

GB_DEF isize gb_utf8_strnlen(u8 const *str, isize max_len);

// NOTE(bill): Windows doesn't handle 8 bit filenames well ('cause Micro$hit)
GB_DEF u16 *gb_utf8_to_ucs2(u16 *buffer, isize len, u8 const *str);

GB_DEF u8 *gb_ucs2_to_utf8(u8 *buffer, isize len, u16 const *str);

GB_DEF u16 *gb_utf8_to_ucs2_buf(u8 const *str);   // NOTE(bill): Uses locally persisting buffer
GB_DEF u8 *gb_ucs2_to_utf8_buf(u16 const *str); // NOTE(bill): Uses locally persisting buffer

// NOTE(bill): Returns size of codepoint in bytes
GB_DEF isize gb_utf8_decode(u8 const *str, isize str_len, Rune *codepoint);

GB_DEF isize gb_utf8_codepoint_size(u8 const *str, isize str_len);

GB_DEF isize gb_utf8_encode_rune(u8 buf[4], Rune r);

#endif /* GB_UTF8_H__ */
