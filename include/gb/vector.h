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

#ifndef  GB_VECTOR_H
# define GB_VECTOR_H

#include "gb/array.h"

#define gb_vector_of(T) struct { \
    size_t size, capacity; \
    union { \
      void *ptr; \
      T *items; \
    }; \
    T *it; \
  }

typedef struct gb_vector gb_vector_t;

struct gb_vector {
  size_t size, capacity;
  void *ptr;
};

GB_DEF size_t gb_vector_pgrowth(gb_vector_t *self, const ssize_t nmin, const size_t isize);
GB_DEF size_t gb_vector_pdecay(gb_vector_t *self, const ssize_t nmax, const size_t isize);

#define gb_vector_push(v, x) \
  (gb_vector_pgrowth((gb_vector_t *) &(v), (v).size+1, sizeof(*(v).items)), *((v).items + (v).size++) = (x))

#define foreach(iterator, value) foreach_(value, iterator)
#define foreach_(value, it, begin, end) \
  for ( \
    (value) = *((it) = (begin)); \
    (it) < (end); \
    (value) = *(++(it)) \
  )

#define rforeach(it, v) rforeach_(v, it)
#define rforeach_(value, it, begin, end) \
  for ( \
    (value) = *((it) = (end) - 1); \
    (it) >= (begin); \
    (value) = *(--(it)) \
  )

#define vector_it(v) (v).it, (v).items, (v).items+(v).size

#endif /* GB_VECTOR_H */
