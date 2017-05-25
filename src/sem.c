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

#include "gb/sem.h"

gb_inline void gb_yield_thread(void) {
#if defined(GB_SYSTEM_WINDOWS)
  _mm_pause();
#elif defined(GB_SYSTEM_OSX)
  __asm__ volatile ("" : : : "memory");
#elif defined(GB_CPU_X86)
  _mm_pause();
#else
#error Unknown architecture
#endif
}

gb_inline void gb_mfence(void) {
#if defined(GB_SYSTEM_WINDOWS)
  _ReadWriteBarrier();
#elif defined(GB_SYSTEM_OSX)
  __sync_synchronize();
#elif defined(GB_CPU_X86)
  _mm_mfence();
#else
#error Unknown architecture
#endif
}

gb_inline void gb_sfence(void) {
#if defined(GB_SYSTEM_WINDOWS)
  _WriteBarrier();
#elif defined(GB_SYSTEM_OSX)
  __asm__ volatile ("" : : : "memory");
#elif defined(GB_CPU_X86)
  _mm_sfence();
#else
#error Unknown architecture
#endif
}

gb_inline void gb_lfence(void) {
#if defined(GB_SYSTEM_WINDOWS)
  _ReadBarrier();
#elif defined(GB_SYSTEM_OSX)
  __asm__ volatile ("" : : : "memory");
#elif defined(GB_CPU_X86)
  _mm_lfence();
#else
#error Unknown architecture
#endif
}


gb_inline void gb_semaphore_release(gbSemaphore *s) { gb_semaphore_post(s, 1); }

#if defined(GB_SYSTEM_WINDOWS)
gb_inline void gb_semaphore_init   (gbSemaphore *s)            { s->win32_handle = CreateSemaphoreA(NULL, 0, I32_MAX, NULL); }
	gb_inline void gb_semaphore_destroy(gbSemaphore *s)            { CloseHandle(s->win32_handle); }
	gb_inline void gb_semaphore_post   (gbSemaphore *s, i32 count) { ReleaseSemaphore(s->win32_handle, count, NULL); }
	gb_inline void gb_semaphore_wait   (gbSemaphore *s)            { WaitForSingleObject(s->win32_handle, INFINITE); }

#elif defined(GB_SYSTEM_OSX)
gb_inline void gb_semaphore_init   (gbSemaphore *s)            { semaphore_create(mach_task_self(), &s->osx_handle, SYNC_POLICY_FIFO, 0); }
	gb_inline void gb_semaphore_destroy(gbSemaphore *s)            { semaphore_destroy(mach_task_self(), s->osx_handle); }
	gb_inline void gb_semaphore_post   (gbSemaphore *s, i32 count) { while (count --> 0) semaphore_signal(s->osx_handle); }
	gb_inline void gb_semaphore_wait   (gbSemaphore *s)            { semaphore_wait(s->osx_handle); }

#elif defined(GB_SYSTEM_UNIX)
gb_inline void gb_semaphore_init   (gbSemaphore *s)            { sem_init(&s->unix_handle, 0, 0); }
gb_inline void gb_semaphore_destroy(gbSemaphore *s)            { sem_destroy(&s->unix_handle); }
gb_inline void gb_semaphore_post   (gbSemaphore *s, i32 count) { while (count --> 0) sem_post(&s->unix_handle); }
gb_inline void gb_semaphore_wait   (gbSemaphore *s)            { int i; do { i = sem_wait(&s->unix_handle); } while (i == -1 && errno == EINTR); }

#else
#error
#endif
