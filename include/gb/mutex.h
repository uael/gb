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

#ifndef  GB_MUTEX_H__
# define GB_MUTEX_H__

#include "gb/sem.h"

typedef struct gbMutex {
  gbSemaphore semaphore;
  gbAtomic32 counter;
  gbAtomic32 owner;
  i32 recursion;
} gbMutex;

GB_API void gb_mutex_init(gbMutex *m);

GB_API void gb_mutex_destroy(gbMutex *m);

GB_API void gb_mutex_lock(gbMutex *m);

GB_API b32 gb_mutex_try_lock(gbMutex *m);

GB_API void gb_mutex_unlock(gbMutex *m);

// NOTE(bill): If you wanted a Scoped Mutex in C++, why not use the defer() construct?
// No need for a silly wrapper class and it's clear!
#if 0
gbMutex m = {0};
gb_mutex_init(&m);
{
  gb_mutex_lock(&m);
  defer (gb_mutex_unlock(&m));

  // Do whatever as the mutex is now scoped based!
}
#endif

#endif /* GB_MUTEX_H__ */
