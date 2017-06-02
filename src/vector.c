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

#include "gb/vector.h"

#ifndef GB_VECTOR_MIN_CAP
# define GB_VECTOR_MIN_CAP 8
#endif

size_t gb_vector_pgrowth(gb_vector_t *self, const ssize_t nmin, const size_t isize) {
  if (nmin > 0) {
    size_t unmin = (size_t) nmin;

    if (self->capacity) {
      if (self->capacity < unmin) {
        if (GB_ISPOW2(unmin)) {
          self->capacity = unmin;
        } else {
          do self->capacity *= 2; while (self->capacity < unmin);
        }
        self->ptr = realloc(self->ptr, isize * self->capacity);
      }
    } else {
      if (unmin == GB_VECTOR_MIN_CAP || (unmin > GB_VECTOR_MIN_CAP && GB_ISPOW2(unmin))) {
        self->capacity = unmin;
      } else {
        self->capacity = GB_VECTOR_MIN_CAP;
        while (self->capacity < unmin) self->capacity *= 2;
      }
      self->ptr = malloc(isize * self->capacity);
    }
    return unmin;
  }
  return 0;
}

size_t gb_vector_pdecay(gb_vector_t *self, const ssize_t nmax, const size_t isize) {
  size_t nearest_pow2;

  if (nmax >= 0) {
    size_t unmax = (size_t) nmax;

    nearest_pow2 = gb_roundup32((size_t) unmax);
    if (self->capacity > nearest_pow2) {
      self->capacity = nearest_pow2;
      self->ptr = realloc(self->ptr, isize * self->capacity);
    }
    if (self->size > unmax) {
      memset((char *) self->ptr + unmax * isize, 0, (self->size - unmax) * isize);
    }
    return unmax;
  }
  return 0;
}
