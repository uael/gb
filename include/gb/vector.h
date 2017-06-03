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

#define vector_of(T) struct { \
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

#define vector_push(v, x) \
  (gb_vector_pgrowth((gb_vector_t *) &(v), (v).size+1, sizeof(*(v).items)), *((v).items + (v).size++) = (x))

#define vector_it(v) (v).it
#define vector_begin(v) (v).items
#define vector_end(v) (v).items+(v).size

#define foreach_(value, it, begin, end) \
  for ( \
    (value) = *((it) = (begin)); \
    (it) < (end); \
    (value) = *(++(it)) \
  )

#define rforeach_(value, it, begin, end) \
  for ( \
    (value) = *((it) = (end) - 1); \
    (it) >= (begin); \
    (value) = *(--(it)) \
  )

// PP_VA_NUM_ARGS() is a very nifty macro to retrieve the number of arguments handed to a variable-argument macro
// unfortunately, VS 2010 still has this compiler bug which treats a __VA_ARGS__ argument as being one single parameter:
// https://connect.microsoft.com/VisualStudio/feedback/details/521844/variadic-macro-treating-va-args-as-a-single-parameter-for-other-macros#details
#if _MSC_VER <= 1400
#    define PP_VA_NUM_ARGS_HELPER(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...)    N
#    define PP_VA_NUM_ARGS_REVERSE_SEQUENCE            10, 9, 8, 7, 6, 5, 4, 3, 2, 1
#    define PP_LEFT_PARENTHESIS (
#    define PP_RIGHT_PARENTHESIS )
#    define PP_VA_NUM_ARGS(...)                        PP_VA_NUM_ARGS_HELPER PP_LEFT_PARENTHESIS __VA_ARGS__, PP_VA_NUM_ARGS_REVERSE_SEQUENCE PP_RIGHT_PARENTHESIS
#else
#    define PP_VA_NUM_ARGS(...)                        PP_VA_NUM_ARGS_HELPER(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#    define PP_VA_NUM_ARGS_HELPER(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...)    N
#endif

// PP_PASS_VA passes __VA_ARGS__ as multiple parameters to another macro, working around the above-mentioned bug
#if _MSC_VER <= 1400
#    define PP_PASS_VA(...)                            PP_LEFT_PARENTHESIS __VA_ARGS__ PP_RIGHT_PARENTHESIS
#else
#    define PP_PASS_VA(...)                            (__VA_ARGS__)
#endif

#define PP_JOIN(x, y)                    PP_JOIN_HELPER(x, y)
#define PP_JOIN_HELPER(x, y)             PP_JOIN_HELPER_HELPER(x, y)
#define PP_JOIN_HELPER_HELPER(x, y)      x##y

#define PP_EVAL(...)  PP_EVAL__(PP_EVAL__(PP_EVAL__(__VA_ARGS__)))
#define PP_EVAL__(...) __VA_ARGS__

#define DS_VECTOR vector

#define in(T_ds, ds) \
  PP_EVAL(PP_JOIN(PP_EVAL(T_ds), _it)(PP_EVAL(ds))), \
  PP_EVAL(PP_JOIN(PP_EVAL(T_ds), _begin)(PP_EVAL(ds))), \
  PP_EVAL(PP_JOIN(PP_EVAL(T_ds), _end)(PP_EVAL(ds)))
#define foreach(val, in) \
  PP_EVAL(foreach_ PP_PASS_VA(PP_EVAL(val), PP_EVAL(in)))
#define rforeach(val, in) \
  PP_EVAL(rforeach_ PP_PASS_VA(PP_EVAL(val), PP_EVAL(in)))

#endif /* GB_VECTOR_H */
