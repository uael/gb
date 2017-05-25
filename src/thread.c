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

#include "gb/thread.h"

void gb_thread_init(gbThread *t) {
  gb_zero_item(t);
#if defined(GB_SYSTEM_WINDOWS)
  t->win32_handle = INVALID_HANDLE_VALUE;
#else
  t->posix_handle = 0;
#endif
  gb_semaphore_init(&t->semaphore);
}

void gb_thread_destory(gbThread *t) {
  if (t->is_running) gb_thread_join(t);
  gb_semaphore_destroy(&t->semaphore);
}

gb_inline void gb__thread_run(gbThread *t) {
  gb_semaphore_release(&t->semaphore);
  t->proc(t->data);
}

#if defined(GB_SYSTEM_WINDOWS)
gb_inline DWORD __stdcall gb__thread_proc(void *arg) { gb__thread_run(cast(gbThread *)arg); return 0; }
#else

gb_inline void *gb__thread_proc(void *arg) {
  gb__thread_run(cast(gbThread *) arg);
  return NULL;
}
#endif

gb_inline void gb_thread_start(gbThread *t, gbThreadProc *proc, void *data) {
  gb_thread_start_with_stack(t, proc, data, 0);
}

gb_inline void gb_thread_start_with_stack(gbThread *t, gbThreadProc *proc, void *data, isize stack_size) {
  GB_ASSERT(!t->is_running);
  GB_ASSERT(proc != NULL);
  t->proc = proc;
  t->data = data;
  t->stack_size = stack_size;

#if defined(GB_SYSTEM_WINDOWS)
  t->win32_handle = CreateThread(NULL, stack_size, gb__thread_proc, t, 0, NULL);
  GB_ASSERT_MSG(t->win32_handle != NULL, "CreateThread: GetLastError");
#else
  {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    if (stack_size != 0)
      pthread_attr_setstacksize(&attr, stack_size);
    pthread_create(&t->posix_handle, &attr, gb__thread_proc, t);
    pthread_attr_destroy(&attr);
  }
#endif

  t->is_running = true;
  gb_semaphore_wait(&t->semaphore);
}

gb_inline void gb_thread_join(gbThread *t) {
  if (!t->is_running) return;

#if defined(GB_SYSTEM_WINDOWS)
  WaitForSingleObject(t->win32_handle, INFINITE);
  CloseHandle(t->win32_handle);
  t->win32_handle = INVALID_HANDLE_VALUE;
#else
  pthread_join(t->posix_handle, NULL);
  t->posix_handle = 0;
#endif
  t->is_running = false;
}

gb_inline b32 gb_thread_is_running(gbThread const *t) { return t->is_running != 0; }

gb_inline u32 gb_thread_current_id(void) {
  u32 thread_id;
#if defined(GB_SYSTEM_WINDOWS)
#if defined(GB_ARCH_32_BIT) && defined(GB_CPU_X86)
  thread_id = (cast(u32 *)__readfsdword(24))[9];
#elif defined(GB_ARCH_64_BIT) && defined(GB_CPU_X86)
  thread_id = (cast(u32 *)__readgsqword(48))[18];
#else
  thread_id = GetCurrentThreadId();
#endif

#elif defined(GB_SYSTEM_OSX) && defined(GB_ARCH_64_BIT)
  thread_id = pthread_mach_thread_np(pthread_self());
#elif defined(GB_ARCH_32_BIT) && defined(GB_CPU_X86)
  __asm__("mov %%gs:0x08,%0" : "=r"(thread_id));
#elif defined(GB_ARCH_64_BIT) && defined(GB_CPU_X86)
  __asm__("mov %%gs:0x10,%0" : "=r"(thread_id));
#else
#error Unsupported architecture for gb_thread_current_id()
#endif

  return thread_id;
}

void gb_thread_set_name(gbThread *t, char const *name) {
#if defined(GB_COMPILER_MSVC)
#pragma pack(push, 8)
  typedef struct {
    DWORD       type;
    char const *name;
    DWORD       id;
    DWORD       flags;
  } gbprivThreadName;
#pragma pack(pop)
  gbprivThreadName tn;
  tn.type  = 0x1000;
  tn.name  = name;
  tn.id    = GetThreadId(cast(HANDLE)t->win32_handle);
  tn.flags = 0;

  __try {
    RaiseException(0x406d1388, 0, gb_size_of(tn)/4, cast(ULONG_PTR *)&tn);
  } __except(1 /*EXCEPTION_EXECUTE_HANDLER*/) {
  }

#elif defined(GB_SYSTEM_WINDOWS) && !defined(GB_COMPILER_MSVC)
  // IMPORTANT TODO(bill): Set thread name for GCC/Clang on windows
  return;
#elif defined(GB_SYSTEM_OSX)
  // TODO(bill): Test if this works
  pthread_setname_np(name);
#else
// TODO(bill): Test if this works
  pthread_setname_np(t->posix_handle, name);
#endif
}

void gb_sync_init(gbSync *s) {
  gb_zero_item(s);
  gb_mutex_init(&s->mutex);
  gb_mutex_init(&s->start);
  gb_semaphore_init(&s->release);
}

void gb_sync_destroy(gbSync *s) {
  if (s->waiting)
    GB_PANIC("Cannot destroy while threads are waiting!");

  gb_mutex_destroy(&s->mutex);
  gb_mutex_destroy(&s->start);
  gb_semaphore_destroy(&s->release);
}

void gb_sync_set_target(gbSync *s, i32 count) {
  gb_mutex_lock(&s->start);

  gb_mutex_lock(&s->mutex);
  GB_ASSERT(s->target == 0);
  s->target = count;
  s->current = 0;
  s->waiting = 0;
  gb_mutex_unlock(&s->mutex);
}

void gb_sync_release(gbSync *s) {
  if (s->waiting) {
    gb_semaphore_release(&s->release);
  } else {
    s->target = 0;
    gb_mutex_unlock(&s->start);
  }
}

i32 gb_sync_reach(gbSync *s) {
  i32 n;
  gb_mutex_lock(&s->mutex);
  GB_ASSERT(s->current < s->target);
  n = ++s->current; // NOTE(bill): Record this value to avoid possible race if `return s->current` was done
  if (s->current == s->target)
    gb_sync_release(s);
  gb_mutex_unlock(&s->mutex);
  return n;
}

void gb_sync_reach_and_wait(gbSync *s) {
  gb_mutex_lock(&s->mutex);
  GB_ASSERT(s->current < s->target);
  s->current++;
  if (s->current == s->target) {
    gb_sync_release(s);
    gb_mutex_unlock(&s->mutex);
  } else {
    s->waiting++;                   // NOTE(bill): Waiting, so one more waiter
    gb_mutex_unlock(&s->mutex);     // NOTE(bill): Release the mutex to other threads

    gb_semaphore_wait(&s->release); // NOTE(bill): Wait for merge completion

    gb_mutex_lock(&s->mutex);       // NOTE(bill): On merge completion, lock mutex
    s->waiting--;                   // NOTE(bill): Done waiting
    gb_sync_release(s);             // NOTE(bill): Restart the next waiter
    gb_mutex_unlock(&s->mutex);
  }
}
