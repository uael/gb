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

#ifndef  GB_HASH_H__
# define GB_HASH_H__

#include "gb/vector.h"

GB_EXTERN uint32_t gb_adler32(void const *data, ssize_t len);

GB_EXTERN uint32_t gb_crc32(void const *data, ssize_t len);

GB_EXTERN uint64_t gb_crc64(void const *data, ssize_t len);

GB_EXTERN uint32_t gb_fnv32(void const *data, ssize_t len);

GB_EXTERN uint64_t gb_fnv64(void const *data, ssize_t len);

GB_EXTERN uint32_t gb_fnv32a(void const *data, ssize_t len);

GB_EXTERN uint64_t gb_fnv64a(void const *data, ssize_t len);

// NOTE(bill): Default seed of 0x9747b28c
// NOTE(bill): I prefer using murmur64 for most hashes
GB_EXTERN uint32_t gb_murmur32(void const *data, ssize_t len);

GB_EXTERN uint64_t gb_murmur64(void const *data, ssize_t len);

GB_EXTERN uint32_t gb_murmur32_seed(void const *data, ssize_t len, uint32_t seed);

GB_EXTERN uint64_t gb_murmur64_seed(void const *data, ssize_t len, uint64_t seed);

#endif /* GB_HASH_H__ */
