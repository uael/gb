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

#ifndef  GB_RANDOM_H__
# define GB_RANDOM_H__

#include "gb/time.h"

typedef struct gbRandom {
  uint32_t offsets[8];
  uint32_t value;
} gbRandom;

// NOTE(bill): Generates from numerous sources to produce a decent pseudo-random seed
GB_DEF void gb_random_init(gbRandom *r);

GB_DEF uint32_t gb_random_gen_u32(gbRandom *r);

GB_DEF uint32_t gb_random_gen_u32_unique(gbRandom *r);

GB_DEF uint64_t gb_random_gen_u64(gbRandom *r); // NOTE(bill): (gb_random_gen_u32() << 32) | gb_random_gen_u32()
GB_DEF ssize_t gb_random_gen_isize(gbRandom *r);

GB_DEF int64_t gb_random_range_i64(gbRandom *r, int64_t lower_inc, int64_t higher_inc);

GB_DEF ssize_t gb_random_range_isize(gbRandom *r, ssize_t lower_inc, ssize_t higher_inc);

GB_DEF float64_t gb_random_range_f64(gbRandom *r, float64_t lower_inc, float64_t higher_inc);

#endif /* GB_RANDOM_H__ */
