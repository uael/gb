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
  u32 offsets[8];
  u32 value;
} gbRandom;

// NOTE(bill): Generates from numerous sources to produce a decent pseudo-random seed
GB_DEF void  gb_random_init          (gbRandom *r);
GB_DEF u32   gb_random_gen_u32       (gbRandom *r);
GB_DEF u32   gb_random_gen_u32_unique(gbRandom *r);
GB_DEF u64   gb_random_gen_u64       (gbRandom *r); // NOTE(bill): (gb_random_gen_u32() << 32) | gb_random_gen_u32()
GB_DEF isize gb_random_gen_isize     (gbRandom *r);
GB_DEF i64   gb_random_range_i64     (gbRandom *r, i64 lower_inc, i64 higher_inc);
GB_DEF isize gb_random_range_isize   (gbRandom *r, isize lower_inc, isize higher_inc);
GB_DEF f64   gb_random_range_f64     (gbRandom *r, f64 lower_inc, f64 higher_inc);

#endif /* GB_RANDOM_H__ */
