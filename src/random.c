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

#include "gb/random.h"

gb_global gbAtomic32 gb__random_shared_counter = {0};

gb_internal u32 gb__get_noise_from_time(void) {
  u32 accum = 0;
  f64 start, remaining, end, curr = 0;
  u64 interval = 100000ll;

  start = gb_time_now();
  remaining = (interval - cast(u64) (interval * start) % interval) / cast(f64) interval;
  end = start + remaining;

  do {
    curr = gb_time_now();
    accum += cast(u32) curr;
  } while (curr >= end);
  return accum;
}

// NOTE(bill): Partly from http://preshing.com/20121224/how-to-generate-a-sequence-of-unique-random-integers/
// But the generation is even more random-er-est

gb_internal gb_inline u32 gb__permute_qpr(u32 x) {
  gb_local_persist u32 const prime = 4294967291; // 2^32 - 5
  if (x >= prime) {
    return x;
  } else {
    u32 residue = cast(u32) (cast(u64) x * x) % prime;
    if (x <= prime / 2)
      return residue;
    else
      return prime - residue;
  }
}

gb_internal gb_inline u32 gb__permute_with_offset(u32 x, u32 offset) {
  return (gb__permute_qpr(x) + offset) ^ 0x5bf03635;
}

void gb_random_init(gbRandom *r) {
  u64 time, tick;
  isize i, j;
  u32 x = 0;
  r->value = 0;

  r->offsets[0] = gb__get_noise_from_time();
  r->offsets[1] = gb_atomic32_fetch_add(&gb__random_shared_counter, 1);
  r->offsets[2] = gb_thread_current_id();
  r->offsets[3] = gb_thread_current_id() * 3 + 1;
  time = gb_utc_time_now();
  r->offsets[4] = cast(u32) (time >> 32);
  r->offsets[5] = cast(u32) time;
  r->offsets[6] = gb__get_noise_from_time();
  tick = gb_rdtsc();
  r->offsets[7] = cast(u32) (tick ^ (tick >> 32));

  for (j = 0; j < 4; j++) {
    for (i = 0; i < gb_count_of(r->offsets); i++) {
      r->offsets[i] = x = gb__permute_with_offset(x, r->offsets[i]);
    }
  }
}

u32 gb_random_gen_u32(gbRandom *r) {
  u32 x = r->value;
  u32 carry = 1;
  isize i;
  for (i = 0; i < gb_count_of(r->offsets); i++) {
    x = gb__permute_with_offset(x, r->offsets[i]);
    if (carry > 0) {
      carry = ++r->offsets[i] ? 0 : 1;
    }
  }

  r->value = x;
  return x;
}

u32 gb_random_gen_u32_unique(gbRandom *r) {
  u32 x = r->value;
  isize i;
  r->value++;
  for (i = 0; i < gb_count_of(r->offsets); i++) {
    x = gb__permute_with_offset(x, r->offsets[i]);
  }

  return x;
}

u64 gb_random_gen_u64(gbRandom *r) {
  return ((cast(u64) gb_random_gen_u32(r)) << 32) | gb_random_gen_u32(r);
}

isize gb_random_gen_isize(gbRandom *r) {
  u64 u = gb_random_gen_u64(r);
  return *cast(isize *) &u;
}

i64 gb_random_range_i64(gbRandom *r, i64 lower_inc, i64 higher_inc) {
  u64 u = gb_random_gen_u64(r);
  i64 i = *cast(i64 *) &u;
  i64 diff = higher_inc - lower_inc + 1;
  i %= diff;
  i += lower_inc;
  return i;
}

isize gb_random_range_isize(gbRandom *r, isize lower_inc, isize higher_inc) {
  u64 u = gb_random_gen_u64(r);
  isize i = *cast(isize *) &u;
  isize diff = higher_inc - lower_inc + 1;
  i %= diff;
  i += lower_inc;
  return i;
}

// NOTE(bill): Semi-cc'ed from gb_math to remove need for fmod and math.h
f64 gb__copy_sign64(f64 x, f64 y) {
  i64 ix, iy;
  ix = *(i64 *) &x;
  iy = *(i64 *) &y;

  ix &= 0x7fffffffffffffff;
  ix |= iy & 0x8000000000000000;
  return *cast(f64 *) &ix;
}

f64 gb__floor64(f64 x) { return cast(f64) ((x >= 0.0) ? cast(i64) x : cast(i64) (x - 0.9999999999999999)); }

f64 gb__ceil64(f64 x) { return cast(f64) ((x < 0) ? cast(i64) x : (cast(i64) x) + 1); }

f64 gb__round64(f64 x) { return cast(f64) ((x >= 0.0) ? gb__floor64(x + 0.5) : gb__ceil64(x - 0.5)); }

f64 gb__remainder64(f64 x, f64 y) { return x - (gb__round64(x / y) * y); }

f64 gb__abs64(f64 x) { return x < 0 ? -x : x; }

f64 gb__sign64(f64 x) { return x < 0 ? -1.0 : +1.0; }

f64 gb__mod64(f64 x, f64 y) {
  f64 result;
  y = gb__abs64(y);
  result = gb__remainder64(gb__abs64(x), y);
  if (gb__sign64(result)) result += y;
  return gb__copy_sign64(result, x);
}

f64 gb_random_range_f64(gbRandom *r, f64 lower_inc, f64 higher_inc) {
  u64 u = gb_random_gen_u64(r);
  f64 f = *cast(f64 *) &u;
  f64 diff = higher_inc - lower_inc + 1.0;
  f = gb__mod64(f, diff);
  f += lower_inc;
  return f;
}
