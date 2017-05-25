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

#ifndef  GB_SEM_H__
# define GB_SEM_H__

#include "gb/atomic.h"

// Fences
GB_DEF void gb_yield_thread(void);

GB_DEF void gb_mfence(void);

GB_DEF void gb_sfence(void);

GB_DEF void gb_lfence(void);

#if defined(GB_SYSTEM_WINDOWS)
typedef struct gbSemaphore { void *win32_handle; }     gbSemaphore;
#elif defined(GB_SYSTEM_OSX)
typedef struct gbSemaphore { semaphore_t osx_handle; } gbSemaphore;
#elif defined(GB_SYSTEM_UNIX)
typedef struct gbSemaphore {
  sem_t unix_handle;
} gbSemaphore;
#else
#error
#endif

GB_DEF void gb_semaphore_init(gbSemaphore *s);

GB_DEF void gb_semaphore_destroy(gbSemaphore *s);

GB_DEF void gb_semaphore_post(gbSemaphore *s, i32 count);

GB_DEF void gb_semaphore_release(gbSemaphore *s); // NOTE(bill): gb_semaphore_post(s, 1)
GB_DEF void gb_semaphore_wait(gbSemaphore *s);

#endif /* GB_SEM_H__ */
