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

#ifndef  GB_ATOMIC_H__
# define GB_ATOMIC_H__

#include "gb/memory.h"

// TODO(bill): Be specific with memory order?
// e.g. relaxed, acquire, release, acquire_release

#if defined(GB_COMPILER_MSVC)
typedef struct gbAtomic32  { int32_t   volatile value; } gbAtomic32;
typedef struct gbAtomic64  { int64_t   volatile value; } gbAtomic64;
typedef struct gbAtomicPtr { void *volatile value; } gbAtomicPtr;
#else
#if defined(GB_ARCH_32_BIT)
#define GB_ATOMIC_PTR_ALIGNMENT 4
#elif defined(GB_ARCH_64_BIT)
#define GB_ATOMIC_PTR_ALIGNMENT 8
#else
#error Unknown architecture
#endif

typedef struct gbAtomic32 {
  int32_t volatile value;
} __attribute__ ((aligned(4))) gbAtomic32;
typedef struct gbAtomic64 {
  int64_t volatile value;
} __attribute__ ((aligned(8))) gbAtomic64;
typedef struct gbAtomicPtr {
  void *volatile value;
} __attribute__ ((aligned(GB_ATOMIC_PTR_ALIGNMENT))) gbAtomicPtr;
#endif

GB_DEF int32_t gb_atomic32_load(gbAtomic32 const volatile *a);

GB_DEF void gb_atomic32_store(gbAtomic32 volatile *a, int32_t value);

GB_DEF int32_t gb_atomic32_compare_exchange(gbAtomic32 volatile *a, int32_t expected, int32_t desired);

GB_DEF int32_t gb_atomic32_exchanged(gbAtomic32 volatile *a, int32_t desired);

GB_DEF int32_t gb_atomic32_fetch_add(gbAtomic32 volatile *a, int32_t operand);

GB_DEF int32_t gb_atomic32_fetch_and(gbAtomic32 volatile *a, int32_t operand);

GB_DEF int32_t gb_atomic32_fetch_or(gbAtomic32 volatile *a, int32_t operand);

GB_DEF byte32_t gb_atomic32_spin_lock(gbAtomic32 volatile *a, ssize_t time_out); // NOTE(bill): time_out = -1 as default
GB_DEF void gb_atomic32_spin_unlock(gbAtomic32 volatile *a);

GB_DEF byte32_t gb_atomic32_try_acquire_lock(gbAtomic32 volatile *a);

GB_DEF int64_t gb_atomic64_load(gbAtomic64 const volatile *a);

GB_DEF void gb_atomic64_store(gbAtomic64 volatile *a, int64_t value);

GB_DEF int64_t gb_atomic64_compare_exchange(gbAtomic64 volatile *a, int64_t expected, int64_t desired);

GB_DEF int64_t gb_atomic64_exchanged(gbAtomic64 volatile *a, int64_t desired);

GB_DEF int64_t gb_atomic64_fetch_add(gbAtomic64 volatile *a, int64_t operand);

GB_DEF int64_t gb_atomic64_fetch_and(gbAtomic64 volatile *a, int64_t operand);

GB_DEF int64_t gb_atomic64_fetch_or(gbAtomic64 volatile *a, int64_t operand);

GB_DEF byte32_t gb_atomic64_spin_lock(gbAtomic64 volatile *a, ssize_t time_out); // NOTE(bill): time_out = -1 as default
GB_DEF void gb_atomic64_spin_unlock(gbAtomic64 volatile *a);

GB_DEF byte32_t gb_atomic64_try_acquire_lock(gbAtomic64 volatile *a);

GB_DEF void *gb_atomic_ptr_load(gbAtomicPtr const volatile *a);

GB_DEF void gb_atomic_ptr_store(gbAtomicPtr volatile *a, void *value);

GB_DEF void *gb_atomic_ptr_compare_exchange(gbAtomicPtr volatile *a, void *expected, void *desired);

GB_DEF void *gb_atomic_ptr_exchanged(gbAtomicPtr volatile *a, void *desired);

GB_DEF void *gb_atomic_ptr_fetch_add(gbAtomicPtr volatile *a, void *operand);

GB_DEF void *gb_atomic_ptr_fetch_and(gbAtomicPtr volatile *a, void *operand);

GB_DEF void *gb_atomic_ptr_fetch_or(gbAtomicPtr volatile *a, void *operand);

GB_DEF byte32_t gb_atomic_ptr_spin_lock(gbAtomicPtr volatile *a, ssize_t time_out); // NOTE(bill): time_out = -1 as default
GB_DEF void gb_atomic_ptr_spin_unlock(gbAtomicPtr volatile *a);

GB_DEF byte32_t gb_atomic_ptr_try_acquire_lock(gbAtomicPtr volatile *a);

#endif /* GB_ATOMIC_H__ */
