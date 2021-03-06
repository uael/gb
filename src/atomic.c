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

#include "gb/atomic.h"
#include "gb/thread.h"

////////////////////////////////////////////////////////////////
//
// Concurrency
//
//
// IMPORTANT TODO(bill): Use compiler intrinsics for the atomics

#if defined(GB_COMPILER_MSVC) && !defined(GB_COMPILER_CLANG)
gb_inline int32_t  gb_atomic32_load (gbAtomic32 const volatile *a)      { return a->value;  }
gb_inline void gb_atomic32_store(gbAtomic32 volatile *a, int32_t value) { a->value = value; }

gb_inline int32_t gb_atomic32_compare_exchange(gbAtomic32 volatile *a, int32_t expected, int32_t desired) {
  return _InterlockedCompareExchange(cast(long volatile *)a, desired, expected);
}
gb_inline int32_t gb_atomic32_exchanged(gbAtomic32 volatile *a, int32_t desired) {
  return _InterlockedExchange(cast(long volatile *)a, desired);
}
gb_inline int32_t gb_atomic32_fetch_add(gbAtomic32 volatile *a, int32_t operand) {
  return _InterlockedExchangeAdd(cast(long volatile *)a, operand);
}
gb_inline int32_t gb_atomic32_fetch_and(gbAtomic32 volatile *a, int32_t operand) {
  return _InterlockedAnd(cast(long volatile *)a, operand);
}
gb_inline int32_t gb_atomic32_fetch_or(gbAtomic32 volatile *a, int32_t operand) {
  return _InterlockedOr(cast(long volatile *)a, operand);
}

gb_inline int64_t gb_atomic64_load(gbAtomic64 const volatile *a) {
#if defined(GB_ARCH_64_BIT)
  return a->value;
#elif GB_CPU_X86
  // NOTE(bill): The most compatible way to get an atomic 64-bit load on x86 is with cmpxchg8b
  int64_t result;
  __asm {
    mov esi, a;
    mov ebx, eax;
    mov ecx, edx;
    lock cmpxchg8b [esi];
    mov dword ptr result, eax;
    mov dword ptr result[4], edx;
  }
  return result;
#else
#error TODO(bill): atomics for this CPU
#endif
}

gb_inline void gb_atomic64_store(gbAtomic64 volatile *a, int64_t value) {
#if defined(GB_ARCH_64_BIT)
  a->value = value;
#elif GB_CPU_X86
  // NOTE(bill): The most compatible way to get an atomic 64-bit store on x86 is with cmpxchg8b
  __asm {
    mov esi, a;
    mov ebx, dword ptr value;
    mov ecx, dword ptr value[4];
  retry:
    cmpxchg8b [esi];
    jne retry;
  }
#else
#error TODO(bill): atomics for this CPU
#endif
}

gb_inline int64_t gb_atomic64_compare_exchange(gbAtomic64 volatile *a, int64_t expected, int64_t desired) {
  return _InterlockedCompareExchange64(cast(int64_t volatile *)a, desired, expected);
}

gb_inline int64_t gb_atomic64_exchanged(gbAtomic64 volatile *a, int64_t desired) {
#if defined(GB_ARCH_64_BIT)
  return _InterlockedExchange64(cast(int64_t volatile *)a, desired);
#elif GB_CPU_X86
  int64_t expected = a->value;
  for (;;) {
    int64_t original = _InterlockedCompareExchange64(cast(int64_t volatile *)a, desired, expected);
    if (original == expected)
      return original;
    expected = original;
  }
#else
#error TODO(bill): atomics for this CPU
#endif
}

gb_inline int64_t gb_atomic64_fetch_add(gbAtomic64 volatile *a, int64_t operand) {
#if defined(GB_ARCH_64_BIT)
  return _InterlockedExchangeAdd64(cast(int64_t volatile *)a, operand);
#elif GB_CPU_X86
  int64_t expected = a->value;
  for (;;) {
    int64_t original = _InterlockedCompareExchange64(cast(int64_t volatile *)a, expected + operand, expected);
    if (original == expected)
      return original;
    expected = original;
  }
#else
#error TODO(bill): atomics for this CPU
#endif
}

gb_inline int64_t gb_atomic64_fetch_and(gbAtomic64 volatile *a, int64_t operand) {
#if defined(GB_ARCH_64_BIT)
  return _InterlockedAnd64(cast(int64_t volatile *)a, operand);
#elif GB_CPU_X86
  int64_t expected = a->value;
  for (;;) {
    int64_t original = _InterlockedCompareExchange64(cast(int64_t volatile *)a, expected & operand, expected);
    if (original == expected)
      return original;
    expected = original;
  }
#else
#error TODO(bill): atomics for this CPU
#endif
}

gb_inline int64_t gb_atomic64_fetch_or(gbAtomic64 volatile *a, int64_t operand) {
#if defined(GB_ARCH_64_BIT)
  return _InterlockedOr64(cast(int64_t volatile *)a, operand);
#elif GB_CPU_X86
  int64_t expected = a->value;
  for (;;) {
    int64_t original = _InterlockedCompareExchange64(cast(int64_t volatile *)a, expected | operand, expected);
    if (original == expected)
      return original;
    expected = original;
  }
#else
#error TODO(bill): atomics for this CPU
#endif
}



#elif defined(GB_CPU_X86)

gb_inline int32_t gb_atomic32_load(gbAtomic32 const volatile *a) { return a->value; }

gb_inline void gb_atomic32_store(gbAtomic32 volatile *a, int32_t value) { a->value = value; }

gb_inline int32_t gb_atomic32_compare_exchange(gbAtomic32 volatile *a, int32_t expected, int32_t desired) {
  int32_t original;
  __asm__ volatile(
  "lock; cmpxchgl %2, %1"
  : "=a"(original), "+m"(a->value)
  : "q"(desired), "0"(expected)
  );
  return original;
}

gb_inline int32_t gb_atomic32_exchanged(gbAtomic32 volatile *a, int32_t desired) {
  // NOTE(bill): No lock prefix is necessary for xchgl
  int32_t original;
  __asm__ volatile(
  "xchgl %0, %1"
  : "=r"(original), "+m"(a->value)
  : "0"(desired)
  );
  return original;
}

gb_inline int32_t gb_atomic32_fetch_add(gbAtomic32 volatile *a, int32_t operand) {
  int32_t original;
  __asm__ volatile(
  "lock; xaddl %0, %1"
  : "=r"(original), "+m"(a->value)
  : "0"(operand)
  );
  return original;
}

gb_inline int32_t gb_atomic32_fetch_and(gbAtomic32 volatile *a, int32_t operand) {
  int32_t original;
  int32_t tmp;
  __asm__ volatile(
  "1:     movl    %1, %0\n"
    "       movl    %0, %2\n"
    "       andl    %3, %2\n"
    "       lock; cmpxchgl %2, %1\n"
    "       jne     1b"
  : "=&a"(original), "+m"(a->value), "=&r"(tmp)
  : "r"(operand)
  );
  return original;
}

gb_inline int32_t gb_atomic32_fetch_or(gbAtomic32 volatile *a, int32_t operand) {
  int32_t original;
  int32_t temp;
  __asm__ volatile(
  "1:     movl    %1, %0\n"
    "       movl    %0, %2\n"
    "       orl     %3, %2\n"
    "       lock; cmpxchgl %2, %1\n"
    "       jne     1b"
  : "=&a"(original), "+m"(a->value), "=&r"(temp)
  : "r"(operand)
  );
  return original;
}

gb_inline int64_t gb_atomic64_load(gbAtomic64 const volatile *a) {
#if defined(GB_ARCH_64_BIT)
  return a->value;
#else
  int64_t original;
  __asm__ volatile(
    "movl %%ebx, %%eax\n"
    "movl %%ecx, %%edx\n"
    "lock; cmpxchg8b %1"
    : "=&A"(original)
    : "m"(a->value)
  );
  return original;
#endif
}

gb_inline void gb_atomic64_store(gbAtomic64 volatile *a, int64_t value) {
#if defined(GB_ARCH_64_BIT)
  a->value = value;
#else
  int64_t expected = a->value;
  __asm__ volatile(
    "1:    cmpxchg8b %0\n"
    "      jne 1b"
    : "=m"(a->value)
    : "b"((int32_t)value), "c"((int32_t)(value >> 32)), "A"(expected)
  );
#endif
}

gb_inline int64_t gb_atomic64_compare_exchange(gbAtomic64 volatile *a, int64_t expected, int64_t desired) {
#if defined(GB_ARCH_64_BIT)
  int64_t original;
  __asm__ volatile(
  "lock; cmpxchgq %2, %1"
  : "=a"(original), "+m"(a->value)
  : "q"(desired), "0"(expected)
  );
  return original;
#else
  int64_t original;
  __asm__ volatile(
    "lock; cmpxchg8b %1"
    : "=A"(original), "+m"(a->value)
    : "b"((int32_t)desired), "c"((int32_t)(desired >> 32)), "0"(expected)
  );
  return original;
#endif
}

gb_inline int64_t gb_atomic64_exchanged(gbAtomic64 volatile *a, int64_t desired) {
#if defined(GB_ARCH_64_BIT)
  int64_t original;
  __asm__ volatile(
  "xchgq %0, %1"
  : "=r"(original), "+m"(a->value)
  : "0"(desired)
  );
  return original;
#else
  int64_t original = a->value;
  for (;;) {
    int64_t previous = gb_atomic64_compare_exchange(a, original, desired);
    if (original == previous)
      return original;
    original = previous;
  }
#endif
}

gb_inline int64_t gb_atomic64_fetch_add(gbAtomic64 volatile *a, int64_t operand) {
#if defined(GB_ARCH_64_BIT)
  int64_t original;
  __asm__ volatile(
  "lock; xaddq %0, %1"
  : "=r"(original), "+m"(a->value)
  : "0"(operand)
  );
  return original;
#else
  for (;;) {
    int64_t original = a->value;
    if (gb_atomic64_compare_exchange(a, original, original + operand) == original)
      return original;
  }
#endif
}

gb_inline int64_t gb_atomic64_fetch_and(gbAtomic64 volatile *a, int64_t operand) {
#if defined(GB_ARCH_64_BIT)
  int64_t original;
  int64_t tmp;
  __asm__ volatile(
  "1:     movq    %1, %0\n"
    "       movq    %0, %2\n"
    "       andq    %3, %2\n"
    "       lock; cmpxchgq %2, %1\n"
    "       jne     1b"
  : "=&a"(original), "+m"(a->value), "=&r"(tmp)
  : "r"(operand)
  );
  return original;
#else
  for (;;) {
    int64_t original = a->value;
    if (gb_atomic64_compare_exchange(a, original, original & operand) == original)
      return original;
  }
#endif
}

gb_inline int64_t gb_atomic64_fetch_or(gbAtomic64 volatile *a, int64_t operand) {
#if defined(GB_ARCH_64_BIT)
  int64_t original;
  int64_t temp;
  __asm__ volatile(
  "1:     movq    %1, %0\n"
    "       movq    %0, %2\n"
    "       orq     %3, %2\n"
    "       lock; cmpxchgq %2, %1\n"
    "       jne     1b"
  : "=&a"(original), "+m"(a->value), "=&r"(temp)
  : "r"(operand)
  );
  return original;
#else
  for (;;) {
    int64_t original = a->value;
    if (gb_atomic64_compare_exchange(a, original, original | operand) == original)
      return original;
  }
#endif
}

#else
#error TODO(bill): Implement Atomics for this CPU
#endif

gb_inline byte32_t gb_atomic32_spin_lock(gbAtomic32 volatile *a, ssize_t time_out) {
  int32_t old_value = gb_atomic32_compare_exchange(a, 1, 0);
  int32_t counter = 0;
  while (old_value != 0 && (time_out < 0 || counter++ < time_out)) {
    gb_yield_thread();
    old_value = gb_atomic32_compare_exchange(a, 1, 0);
    gb_mfence();
  }
  return old_value == 0;
}

gb_inline void gb_atomic32_spin_unlock(gbAtomic32 volatile *a) {
  gb_atomic32_store(a, 0);
  gb_mfence();
}

gb_inline byte32_t gb_atomic64_spin_lock(gbAtomic64 volatile *a, ssize_t time_out) {
  int64_t old_value = gb_atomic64_compare_exchange(a, 1, 0);
  int64_t counter = 0;
  while (old_value != 0 && (time_out < 0 || counter++ < time_out)) {
    gb_yield_thread();
    old_value = gb_atomic64_compare_exchange(a, 1, 0);
    gb_mfence();
  }
  return old_value == 0;
}

gb_inline void gb_atomic64_spin_unlock(gbAtomic64 volatile *a) {
  gb_atomic64_store(a, 0);
  gb_mfence();
}

gb_inline byte32_t gb_atomic32_try_acquire_lock(gbAtomic32 volatile *a) {
  int32_t old_value;
  gb_yield_thread();
  old_value = gb_atomic32_compare_exchange(a, 1, 0);
  gb_mfence();
  return old_value == 0;
}

gb_inline byte32_t gb_atomic64_try_acquire_lock(gbAtomic64 volatile *a) {
  int64_t old_value;
  gb_yield_thread();
  old_value = gb_atomic64_compare_exchange(a, 1, 0);
  gb_mfence();
  return old_value == 0;
}

#if defined(GB_ARCH_32_BIT)

gb_inline void *gb_atomic_ptr_load(gbAtomicPtr const volatile *a) {
  return cast(void *)cast(intptr_t)gb_atomic32_load(cast(gbAtomic32 const volatile *)a);
}
gb_inline void gb_atomic_ptr_store(gbAtomicPtr volatile *a, void *value) {
  gb_atomic32_store(cast(gbAtomic32 volatile *)a, cast(int32_t)cast(intptr_t)value);
}
gb_inline void *gb_atomic_ptr_compare_exchange(gbAtomicPtr volatile *a, void *expected, void *desired) {
  return cast(void *)cast(intptr_t)gb_atomic32_compare_exchange(cast(gbAtomic32 volatile *)a, cast(int32_t)cast(intptr_t)expected, cast(int32_t)cast(intptr_t)desired);
}
gb_inline void *gb_atomic_ptr_exchanged(gbAtomicPtr volatile *a, void *desired) {
  return cast(void *)cast(intptr_t)gb_atomic32_exchanged(cast(gbAtomic32 volatile *)a, cast(int32_t)cast(intptr_t)desired);
}
gb_inline void *gb_atomic_ptr_fetch_add(gbAtomicPtr volatile *a, void *operand) {
  return cast(void *)cast(intptr_t)gb_atomic32_fetch_add(cast(gbAtomic32 volatile *)a, cast(int32_t)cast(intptr_t)operand);
}
gb_inline void *gb_atomic_ptr_fetch_and(gbAtomicPtr volatile *a, void *operand) {
  return cast(void *)cast(intptr_t)gb_atomic32_fetch_and(cast(gbAtomic32 volatile *)a, cast(int32_t)cast(intptr_t)operand);
}
gb_inline void *gb_atomic_ptr_fetch_or(gbAtomicPtr volatile *a, void *operand) {
  return cast(void *)cast(intptr_t)gb_atomic32_fetch_or(cast(gbAtomic32 volatile *)a, cast(int32_t)cast(intptr_t)operand);
}
gb_inline byte32_t gb_atomic_ptr_spin_lock(gbAtomicPtr volatile *a, ssize_t time_out) {
  return gb_atomic32_spin_lock(cast(gbAtomic32 volatile *)a, time_out);
}
gb_inline void gb_atomic_ptr_spin_unlock(gbAtomicPtr volatile *a) {
  gb_atomic32_spin_unlock(cast(gbAtomic32 volatile *)a);
}
gb_inline byte32_t gb_atomic_ptr_try_acquire_lock(gbAtomicPtr volatile *a) {
  return gb_atomic32_try_acquire_lock(cast(gbAtomic32 volatile *)a);
}

#elif defined(GB_ARCH_64_BIT)

gb_inline void *gb_atomic_ptr_load(gbAtomicPtr const volatile *a) {
  return cast(void *) cast(intptr_t) gb_atomic64_load(cast(gbAtomic64 const volatile *) a);
}

gb_inline void gb_atomic_ptr_store(gbAtomicPtr volatile *a, void *value) {
  gb_atomic64_store(cast(gbAtomic64 volatile *) a, cast(int64_t) cast(intptr_t) value);
}

gb_inline void *gb_atomic_ptr_compare_exchange(gbAtomicPtr volatile *a, void *expected, void *desired) {
  return cast(void *) cast(intptr_t) gb_atomic64_compare_exchange(cast(gbAtomic64 volatile *) a,
                                                                cast(int64_t) cast(intptr_t) expected,
                                                                cast(int64_t) cast(intptr_t) desired);
}

gb_inline void *gb_atomic_ptr_exchanged(gbAtomicPtr volatile *a, void *desired) {
  return cast(void *) cast(intptr_t) gb_atomic64_exchanged(cast(gbAtomic64 volatile *) a, cast(int64_t) cast(intptr_t) desired);
}

gb_inline void *gb_atomic_ptr_fetch_add(gbAtomicPtr volatile *a, void *operand) {
  return cast(void *) cast(intptr_t) gb_atomic64_fetch_add(cast(gbAtomic64 volatile *) a, cast(int64_t) cast(intptr_t) operand);
}

gb_inline void *gb_atomic_ptr_fetch_and(gbAtomicPtr volatile *a, void *operand) {
  return cast(void *) cast(intptr_t) gb_atomic64_fetch_and(cast(gbAtomic64 volatile *) a, cast(int64_t) cast(intptr_t) operand);
}

gb_inline void *gb_atomic_ptr_fetch_or(gbAtomicPtr volatile *a, void *operand) {
  return cast(void *) cast(intptr_t) gb_atomic64_fetch_or(cast(gbAtomic64 volatile *) a, cast(int64_t) cast(intptr_t) operand);
}

gb_inline byte32_t gb_atomic_ptr_spin_lock(gbAtomicPtr volatile *a, ssize_t time_out) {
  return gb_atomic64_spin_lock(cast(gbAtomic64 volatile *) a, time_out);
}

gb_inline void gb_atomic_ptr_spin_unlock(gbAtomicPtr volatile *a) {
  gb_atomic64_spin_unlock(cast(gbAtomic64 volatile *) a);
}

gb_inline byte32_t gb_atomic_ptr_try_acquire_lock(gbAtomicPtr volatile *a) {
  return gb_atomic64_try_acquire_lock(cast(gbAtomic64 volatile *) a);
}

#endif
