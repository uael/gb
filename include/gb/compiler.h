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

#ifndef  GB_COMPILER_H__
# define GB_COMPILER_H__

#include "gb/arch.h"

#if defined(_MSC_VER)
#define GB_COMPILER_MSVC 1
#elif defined(__GNUC__)
#define GB_COMPILER_GCC 1
#elif defined(__clang__)
#define GB_COMPILER_CLANG 1
#else
#error Unknown compiler
#endif

#if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__x86_64__)
#ifndef GB_CPU_X86
#define GB_CPU_X86 1
#endif
#ifndef GB_CACHE_LINE_SIZE
#define GB_CACHE_LINE_SIZE 64
#endif

#elif defined(_M_PPC) || defined(__powerpc__) || defined(__powerpc64__)
#ifndef GB_CPU_PPC
#define GB_CPU_PPC 1
#endif
#ifndef GB_CACHE_LINE_SIZE
#define GB_CACHE_LINE_SIZE 128
#endif

#elif defined(__arm__)
#ifndef GB_CPU_ARM
#define GB_CPU_ARM 1
#endif
#ifndef GB_CACHE_LINE_SIZE
#define GB_CACHE_LINE_SIZE 64
#endif

#elif defined(__MIPSEL__) || defined(__mips_isa_rev)
#ifndef GB_CPU_MIPS
#define GB_CPU_MIPS 1
#endif
#ifndef GB_CACHE_LINE_SIZE
#define GB_CACHE_LINE_SIZE 64
#endif

#else
#error Unknown CPU Type
#endif

#ifndef GB_STATIC_ASSERT
#define GB_STATIC_ASSERT3(cond, msg) typedef char static_assertion_##msg[(!!(cond))*2-1]
// NOTE(bill): Token pasting madness!!
#define GB_STATIC_ASSERT2(cond, line) GB_STATIC_ASSERT3(cond, static_assertion_at_line_##line)
#define GB_STATIC_ASSERT1(cond, line) GB_STATIC_ASSERT2(cond, line)
#define GB_STATIC_ASSERT(cond)        GB_STATIC_ASSERT1(cond, __LINE__)
#endif


////////////////////////////////////////////////////////////////
//
// Headers
//
//

#if defined(_WIN32) && !defined(__MINGW32__)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#if defined(GB_SYSTEM_UNIX)
#define _GNU_SOURCE
#define _LARGEFILE64_SOURCE
#endif


// TODO(bill): How many of these headers do I really need?
#include <stdlib.h> // maybe this one
#include <stdarg.h>
#include <stddef.h>

#if defined(GB_SYSTEM_WINDOWS)
#if !defined(GB_NO_WINDOWS_H)
#define NOMINMAX            1
#define WIN32_LEAN_AND_MEAN 1
#define WIN32_MEAN_AND_LEAN 1
#define VC_EXTRALEAN        1
#include <windows.h>
#undef NOMINMAX
#undef WIN32_LEAN_AND_MEAN
#undef WIN32_MEAN_AND_LEAN
#undef VC_EXTRALEAN
#endif

#include <malloc.h> // NOTE(bill): _aligned_*()
#include <intrin.h>
#else

#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#ifndef _IOSC11_SOURCE
#define _IOSC11_SOURCE
#endif

#include <stdlib.h> // NOTE(bill): malloc on linux
#include <sys/mman.h>

#if !defined(GB_SYSTEM_OSX)

#include <sys/sendfile.h>

#endif

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <xmmintrin.h>

#endif

#if defined(GB_SYSTEM_OSX)
#include <mach/mach.h>
#include <mach/mach_init.h>
#include <mach/mach_time.h>
#include <mach/thread_act.h>
#include <mach/thread_policy.h>
#include <sys/sysctl.h>
#include <copyfile.h>
#include <mach/clock.h>
#endif

#if defined(GB_SYSTEM_UNIX)

#include <semaphore.h>

#endif

#endif /* GB_COMPILER_H__ */
