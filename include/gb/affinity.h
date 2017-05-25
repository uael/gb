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

#ifndef  GB_AFFINITY_H__
# define GB_AFFINITY_H__

#include "gb/thread.h"

#if defined(GB_SYSTEM_WINDOWS)

typedef struct gbAffinity {
  b32   is_accurate;
  isize core_count;
  isize thread_count;
#define GB_WIN32_MAX_THREADS (8 * gb_size_of(usize))
  usize core_masks[GB_WIN32_MAX_THREADS];

} gbAffinity;

#elif defined(GB_SYSTEM_OSX)
typedef struct gbAffinity {
  b32   is_accurate;
  isize core_count;
  isize thread_count;
  isize threads_per_core;
} gbAffinity;

#elif defined(GB_SYSTEM_LINUX)
typedef struct gbAffinity {
  b32 is_accurate;
  isize core_count;
  isize thread_count;
  isize threads_per_core;
} gbAffinity;
#else
#error TODO(bill): Unknown system
#endif

GB_DEF void gb_affinity_init(gbAffinity *a);

GB_DEF void gb_affinity_destroy(gbAffinity *a);

GB_DEF b32 gb_affinity_set(gbAffinity *a, isize core, isize thread);

GB_DEF isize gb_affinity_thread_count_for_core(gbAffinity *a, isize core);

#endif /* GB_AFFINITY_H__ */
