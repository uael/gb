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

#ifndef  GB_THREAD_H__
# define GB_THREAD_H__

#include "gb/mutex.h"

#define GB_THREAD_PROC(name) void name(void *data)

typedef GB_THREAD_PROC(gbThreadProc);

typedef struct gbThread {
#if defined(GB_SYSTEM_WINDOWS)
  void *        win32_handle;
#else
  pthread_t posix_handle;
#endif

  gbThreadProc *proc;
  void *data;

  gbSemaphore semaphore;
  isize stack_size;
  b32 is_running;
} gbThread;

GB_DEF void gb_thread_init(gbThread *t);

GB_DEF void gb_thread_destory(gbThread *t);

GB_DEF void gb_thread_start(gbThread *t, gbThreadProc *proc, void *data);

GB_DEF void gb_thread_start_with_stack(gbThread *t, gbThreadProc *proc, void *data, isize stack_size);

GB_DEF void gb_thread_join(gbThread *t);

GB_DEF b32 gb_thread_is_running(gbThread const *t);

GB_DEF u32 gb_thread_current_id(void);

GB_DEF void gb_thread_set_name(gbThread *t, char const *name);

// NOTE(bill): Thread Merge Operation
// Based on Sean Barrett's stb_sync
typedef struct gbSync {
  i32 target;  // Target Number of threads
  i32 current; // Threads to hit
  i32 waiting; // Threads waiting

  gbMutex start;
  gbMutex mutex;
  gbSemaphore release;
} gbSync;

GB_DEF void gb_sync_init(gbSync *s);

GB_DEF void gb_sync_destroy(gbSync *s);

GB_DEF void gb_sync_set_target(gbSync *s, i32 count);

GB_DEF void gb_sync_release(gbSync *s);

GB_DEF i32 gb_sync_reach(gbSync *s);

GB_DEF void gb_sync_reach_and_wait(gbSync *s);

#endif /* GB_THREAD_H__ */
