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

#include "gb/mutex.h"
#include "gb/thread.h"

// NOTE(bill): THIS IS FUCKING AWESOME THAT THIS "MUTEX" IS FAST AND RECURSIVE TOO!
// NOTE(bill): WHO THE FUCK NEEDS A NORMAL MUTEX NOW?!?!?!?!
gb_inline void gb_mutex_init(gbMutex *m) {
  gb_atomic32_store(&m->counter, 0);
  gb_atomic32_store(&m->owner, gb_thread_current_id());
  gb_semaphore_init(&m->semaphore);
  m->recursion = 0;
}

gb_inline void gb_mutex_destroy(gbMutex *m) { gb_semaphore_destroy(&m->semaphore); }

gb_inline void gb_mutex_lock(gbMutex *m) {
  i32 thread_id = cast(i32)gb_thread_current_id();
  if (gb_atomic32_fetch_add(&m->counter, 1) > 0) {
    if (thread_id != gb_atomic32_load(&m->owner))
      gb_semaphore_wait(&m->semaphore);
  }

  gb_atomic32_store(&m->owner, thread_id);
  m->recursion++;
}

gb_inline b32 gb_mutex_try_lock(gbMutex *m) {
  i32 thread_id = cast(i32)gb_thread_current_id();
  if (gb_atomic32_load(&m->owner) == thread_id) {
    gb_atomic32_fetch_add(&m->counter, 1);
  } else {
    i32 expected = 0;
    if (gb_atomic32_load(&m->counter) != 0)
      return false;
    if (!gb_atomic32_compare_exchange(&m->counter, expected, 1))
      return false;
    gb_atomic32_store(&m->owner, thread_id);
  }

  m->recursion++;
  return true;
}

gb_inline void gb_mutex_unlock(gbMutex *m) {
  i32 recursion;
  i32 thread_id = cast(i32)gb_thread_current_id();

  GB_ASSERT(thread_id == gb_atomic32_load(&m->owner));

  recursion = --m->recursion;
  if (recursion == 0)
    gb_atomic32_store(&m->owner, thread_id);

  if (gb_atomic32_fetch_add(&m->counter, -1) > 1) {
    if (recursion == 0)
      gb_semaphore_release(&m->semaphore);
  }
}
