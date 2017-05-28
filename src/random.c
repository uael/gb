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

gb_internal uint32_t gb__get_noise_from_time(void) {
  uint32_t accum = 0;
  float64_t start, remaining, end, curr = 0;
  uint64_t interval = 100000ll;

  start = gb_time_now();
  remaining = (interval - cast(uint64_t) (interval * start) % interval) / cast(float64_t) interval;
  end = start + remaining;

  do {
    curr = gb_time_now();
    accum += cast(uint32_t) curr;
  } while (curr >= end);
  return accum;
}

// NOTE(bill): Partly from http://preshing.com/20121224/how-to-generate-a-sequence-of-unique-random-integers/
// But the generation is even more random-er-est

gb_internal gb_inline uint32_t gb__permute_qpr(uint32_t x) {
  gb_local_persist uint32_t const prime = 4294967291; // 2^32 - 5
  if (x >= prime) {
    return x;
  } else {
    uint32_t residue = cast(uint32_t) (cast(uint64_t) x * x) % prime;
    if (x <= prime / 2)
      return residue;
    else
      return prime - residue;
  }
}

gb_internal gb_inline uint32_t gb__permute_with_offset(uint32_t x, uint32_t offset) {
  return (gb__permute_qpr(x) + offset) ^ 0x5bf03635;
}

void gb_random_init(gbRandom *r) {
  uint64_t time, tick;
  ssize_t i, j;
  uint32_t x = 0;
  r->value = 0;

  r->offsets[0] = gb__get_noise_from_time();
  r->offsets[1] = gb_atomic32_fetch_add(&gb__random_shared_counter, 1);
  r->offsets[2] = gb_thread_current_id();
  r->offsets[3] = gb_thread_current_id() * 3 + 1;
  time = gb_utc_time_now();
  r->offsets[4] = cast(uint32_t) (time >> 32);
  r->offsets[5] = cast(uint32_t) time;
  r->offsets[6] = gb__get_noise_from_time();
  tick = gb_rdtsc();
  r->offsets[7] = cast(uint32_t) (tick ^ (tick >> 32));

  for (j = 0; j < 4; j++) {
    for (i = 0; i < gb_count_of(r->offsets); i++) {
      r->offsets[i] = x = gb__permute_with_offset(x, r->offsets[i]);
    }
  }
}

uint32_t gb_random_gen_u32(gbRandom *r) {
  uint32_t x = r->value;
  uint32_t carry = 1;
  ssize_t i;
  for (i = 0; i < gb_count_of(r->offsets); i++) {
    x = gb__permute_with_offset(x, r->offsets[i]);
    if (carry > 0) {
      carry = ++r->offsets[i] ? 0 : 1;
    }
  }

  r->value = x;
  return x;
}

uint32_t gb_random_gen_u32_unique(gbRandom *r) {
  uint32_t x = r->value;
  ssize_t i;
  r->value++;
  for (i = 0; i < gb_count_of(r->offsets); i++) {
    x = gb__permute_with_offset(x, r->offsets[i]);
  }

  return x;
}

uint64_t gb_random_gen_u64(gbRandom *r) {
  return ((cast(uint64_t) gb_random_gen_u32(r)) << 32) | gb_random_gen_u32(r);
}

ssize_t gb_random_gen_isize(gbRandom *r) {
  uint64_t u = gb_random_gen_u64(r);
  return *cast(ssize_t *) &u;
}

int64_t gb_random_range_i64(gbRandom *r, int64_t lower_inc, int64_t higher_inc) {
  uint64_t u = gb_random_gen_u64(r);
  int64_t i = *cast(int64_t *) &u;
  int64_t diff = higher_inc - lower_inc + 1;
  i %= diff;
  i += lower_inc;
  return i;
}

ssize_t gb_random_range_isize(gbRandom *r, ssize_t lower_inc, ssize_t higher_inc) {
  uint64_t u = gb_random_gen_u64(r);
  ssize_t i = *cast(ssize_t *) &u;
  ssize_t diff = higher_inc - lower_inc + 1;
  i %= diff;
  i += lower_inc;
  return i;
}

// NOTE(bill): Semi-cc'ed from gb_math to remove need for fmod and math.h
float64_t gb__copy_sign64(float64_t x, float64_t y) {
  int64_t ix, iy;
  ix = *(int64_t *) &x;
  iy = *(int64_t *) &y;

  ix &= 0x7fffffffffffffff;
  ix |= iy & 0x8000000000000000;
  return *cast(float64_t *) &ix;
}

float64_t gb__floor64(float64_t x) { return cast(float64_t) ((x >= 0.0) ? cast(int64_t) x : cast(int64_t) (x - 0.9999999999999999)); }

float64_t gb__ceil64(float64_t x) { return cast(float64_t) ((x < 0) ? cast(int64_t) x : (cast(int64_t) x) + 1); }

float64_t gb__round64(float64_t x) { return cast(float64_t) ((x >= 0.0) ? gb__floor64(x + 0.5) : gb__ceil64(x - 0.5)); }

float64_t gb__remainder64(float64_t x, float64_t y) { return x - (gb__round64(x / y) * y); }

float64_t gb__abs64(float64_t x) { return x < 0 ? -x : x; }

float64_t gb__sign64(float64_t x) { return x < 0 ? -1.0 : +1.0; }

float64_t gb__mod64(float64_t x, float64_t y) {
  float64_t result;
  y = gb__abs64(y);
  result = gb__remainder64(gb__abs64(x), y);
  if (gb__sign64(result)) result += y;
  return gb__copy_sign64(result, x);
}

float64_t gb_random_range_f64(gbRandom *r, float64_t lower_inc, float64_t higher_inc) {
  uint64_t u = gb_random_gen_u64(r);
  float64_t f = *cast(float64_t *) &u;
  float64_t diff = higher_inc - lower_inc + 1.0;
  f = gb__mod64(f, diff);
  f += lower_inc;
  return f;
}
