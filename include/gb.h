/* gb.h - v0.27  - Ginger Bill's C Helper Library - public domain
                 - no warranty implied; use at your own risk

	This is a single header file with a bunch of useful stuff
	to replace the C/C++ standard library

===========================================================================
	YOU MUST

		#define GB_IMPLEMENTATION

	in EXACTLY _one_ C or C++ file that includes this header, BEFORE the
	include like this:

		#define GB_IMPLEMENTATION
		#include "gb.h"

	All other files should just #include "gb.h" without #define


	If you want the platform layer, YOU MUST

		#define GB_PLATFORM

	BEFORE the include like this:

		#define GB_PLATFORM
		#include "gb.h"

===========================================================================

LICENSE
	This software is dual-licensed to the public domain and under the following
	license: you are granted a perpetual, irrevocable license to copy, modify,
	publish, and distribute this file as you see fit.

WARNING
	- This library is _slightly_ experimental and features may not work as expected.
	- This also means that many functions are not documented.

CREDITS
	Written by Ginger Bill

TODOS
	- Remove CRT dependency for people who want that
		- But do I really?
		- Or make it only depend on the really needed stuff?
	- Older compiler support?
		- How old do you wanna go?
		- Only support C90+extension and C99 not pure C89.
	- File handling
		- All files to be UTF-8 (even on windows)
	- Better Virtual Memory handling
	- Generic Heap Allocator (tcmalloc/dlmalloc/?)
	- Fixed Heap Allocator
	- Better UTF support and conversion
	- Free List, best fit rather than first fit
	- More date & time functions

VERSION HISTORY
	0.27  - OSX fixes and Linux gbAffinity
	0.26d - Minor changes to how gbFile works
	0.26c - gb_str_to_f* fix
	0.26b - Minor fixes
	0.26a - gbString Fix
	0.26  - Default allocator flags and generic hash table
	0.25a - Fix UTF-8 stuff
	0.25  - OS X gbPlatform Support (missing some things)
	0.24b - Compile on OSX (excluding platform part)
	0.24a - Minor additions
	0.24  - Enum convention change
	0.23  - Optional Windows.h removal (because I'm crazy)
	0.22a - Remove gbVideoMode from gb_platform_init_*
	0.22  - gbAffinity - (Missing Linux version)
	0.21  - Platform Layer Restructuring
	0.20  - Improve file io
	0.19  - Clipboard Text
	0.18a - Controller vibration
	0.18  - Raw keyboard and mouse input for WIN32
	0.17d - Fixed printf bug for strings
	0.17c - Compile as 32 bit
	0.17b - Change formating style because why not?
	0.17a - Dropped C90 Support (For numerous reasons)
	0.17  - Instantiated Hash Table
	0.16a - Minor code layout changes
	0.16  - New file API and improved platform layer
	0.15d - Linux Experimental Support (DON'T USE IT PLEASE)
	0.15c - Linux Experimental Support (DON'T USE IT)
	0.15b - C90 Support
	0.15a - gb_atomic(32|64)_spin_(lock|unlock)
	0.15  - Recursive "Mutex"; Key States; gbRandom
	0.14  - Better File Handling and better printf (WIN32 Only)
	0.13  - Highly experimental platform layer (WIN32 Only)
	0.12b - Fix minor file bugs
	0.12a - Compile as C++
	0.12  - New File Handing System! No stdio or stdlib! (WIN32 Only)
	0.11a - Add string precision and width (experimental)
	0.11  - Started making stdio & stdlib optional (Not tested much)
	0.10c - Fix gb_endian_swap32()
	0.10b - Probable timing bug for gb_time_now()
	0.10a - Work on multiple compilers
	0.10  - Scratch Memory Allocator
	0.09a - Faster Mutex and the Free List is slightly improved
	0.09  - Basic Virtual Memory System and Dreadful Free List allocator
	0.08a - Fix *_appendv bug
	0.08  - Huge Overhaul!
	0.07a - Fix alignment in gb_heap_allocator_proc
	0.07  - Hash Table and Hashing Functions
	0.06c - Better Documentation
	0.06b - OS X Support
	0.06a - Linux Support
	0.06  - Windows GCC Support and MSVC x86 Support
	0.05b - Formatting
	0.05a - Minor function name changes
	0.05  - Radix Sort for unsigned integers (TODO: Other primitives)
	0.04  - Better UTF support and search/sort procs
	0.03  - Completely change procedure naming convention
	0.02a - Bug fixes
	0.02  - Change naming convention and gbArray(Type)
	0.01  - Initial Version
*/


#ifndef GB_INCLUDE_GB_H
#define GB_INCLUDE_GB_H

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(__cplusplus)
	#define GB_EXTERN extern "C"
#else
	#define GB_EXTERN extern
#endif

#if defined(_WIN32)
	#define GB_DLL_EXPORT GB_EXTERN __declspec(dllexport)
	#define GB_DLL_IMPORT GB_EXTERN __declspec(dllimport)
#else
	#define GB_DLL_EXPORT GB_EXTERN __attribute__((visibility("default")))
	#define GB_DLL_IMPORT GB_EXTERN
#endif

// NOTE(bill): Redefine for DLL, etc.
#ifndef GB_DEF
	#ifdef GB_STATIC
		#define GB_DEF static
	#else
		#define GB_DEF extern
	#endif
#endif

#if defined(_WIN64) || defined(__x86_64__) || defined(_M_X64) || defined(__64BIT__) || defined(__powerpc64__) || defined(__ppc64__)
	#ifndef GB_ARCH_64_BIT
	#define GB_ARCH_64_BIT 1
	#endif
#else
	// NOTE(bill): I'm only supporting 32 bit and 64 bit systems
	#ifndef GB_ARCH_32_BIT
	#define GB_ARCH_32_BIT 1
	#endif
#endif


#ifndef GB_EDIAN_ORDER
#define GB_EDIAN_ORDER
	// TODO(bill): Is the a good way or is it better to test for certain compilers and macros?
	#define GB_IS_BIG_EDIAN    (!*(u8*)&(u16){1})
	#define GB_IS_LITTLE_EDIAN (!GB_IS_BIG_EDIAN)
#endif

#if defined(_WIN32) || defined(_WIN64)
	#ifndef GB_SYSTEM_WINDOWS
	#define GB_SYSTEM_WINDOWS 1
	#endif
#elif defined(__APPLE__) && defined(__MACH__)
	#ifndef GB_SYSTEM_OSX
	#define GB_SYSTEM_OSX 1
	#endif
#elif defined(__unix__)
	#ifndef GB_SYSTEM_UNIX
	#define GB_SYSTEM_UNIX 1
	#endif

	#if defined(__linux__)
		#ifndef GB_SYSTEM_LINUX
		#define GB_SYSTEM_LINUX 1
		#endif
	#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
		#ifndef GB_SYSTEM_FREEBSD
		#define GB_SYSTEM_FREEBSD 1
		#endif
	#else
		#error This UNIX operating system is not supported
	#endif
#else
	#error This operating system is not supported
#endif

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


////////////////////////////////////////////////////////////////
//
// Base Types
//
//

#if defined(GB_COMPILER_MSVC)
	#if _MSC_VER < 1300
	typedef unsigned char     u8;
	typedef   signed char     i8;
	typedef unsigned short   u16;
	typedef   signed short   i16;
	typedef unsigned int     u32;
	typedef   signed int     i32;
	#else
	typedef unsigned __int8   u8;
	typedef   signed __int8   i8;
	typedef unsigned __int16 u16;
	typedef   signed __int16 i16;
	typedef unsigned __int32 u32;
	typedef   signed __int32 i32;
	#endif
	typedef unsigned __int64 u64;
	typedef   signed __int64 i64;
#else
	#include <stdint.h>
	typedef uint8_t   u8;
	typedef  int8_t   i8;
	typedef uint16_t u16;
	typedef  int16_t i16;
	typedef uint32_t u32;
	typedef  int32_t i32;
	typedef uint64_t u64;
	typedef  int64_t i64;
#endif

GB_STATIC_ASSERT(sizeof(u8)  == sizeof(i8));
GB_STATIC_ASSERT(sizeof(u16) == sizeof(i16));
GB_STATIC_ASSERT(sizeof(u32) == sizeof(i32));
GB_STATIC_ASSERT(sizeof(u64) == sizeof(i64));

GB_STATIC_ASSERT(sizeof(u8)  == 1);
GB_STATIC_ASSERT(sizeof(u16) == 2);
GB_STATIC_ASSERT(sizeof(u32) == 4);
GB_STATIC_ASSERT(sizeof(u64) == 8);

typedef size_t    usize;
typedef ptrdiff_t isize;

GB_STATIC_ASSERT(sizeof(usize) == sizeof(isize));

// NOTE(bill): (u)intptr is only here for semantic reasons really as this library will only support 32/64 bit OSes.
// NOTE(bill): Are there any modern OSes (not 16 bit) where intptr != isize ?
#if defined(_WIN64)
	typedef signed   __int64  intptr;
	typedef unsigned __int64 uintptr;
#elif defined(_WIN32)
	// NOTE(bill); To mark types changing their size, e.g. intptr
	#ifndef _W64
		#if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
			#define _W64 __w64
		#else
			#define _W64
		#endif
	#endif

	typedef _W64   signed int  intptr;
	typedef _W64 unsigned int uintptr;
#else
	typedef uintptr_t uintptr;
	typedef  intptr_t  intptr;
#endif

GB_STATIC_ASSERT(sizeof(uintptr) == sizeof(intptr));

typedef float  f32;
typedef double f64;

GB_STATIC_ASSERT(sizeof(f32) == 4);
GB_STATIC_ASSERT(sizeof(f64) == 8);

typedef i32 Rune; // NOTE(bill): Unicode codepoint
#define GB_RUNE_INVALID cast(Rune)(0xfffd)
#define GB_RUNE_MAX     cast(Rune)(0x0010ffff)
#define GB_RUNE_BOM     cast(Rune)(0xfeff)
#define GB_RUNE_EOF     cast(Rune)(-1)


typedef i8  b8;
typedef i16 b16;
typedef i32 b32; // NOTE(bill): Prefer this!!!

// NOTE(bill): Get true and false
#if !defined(__cplusplus)
	#if (defined(_MSC_VER) && _MSC_VER <= 1800) || (!defined(_MSC_VER) && !defined(__STDC_VERSION__))
		#ifndef true
		#define true  (0 == 0)
		#endif
		#ifndef false
		#define false (0 != 0)
		#endif
		typedef b8 bool;
	#else
		#include <stdbool.h>
	#endif
#endif

// NOTE(bill): These do are not prefixed with gb because the types are not.
#ifndef U8_MIN
#define U8_MIN 0u
#define U8_MAX 0xffu
#define I8_MIN (-0x7f - 1)
#define I8_MAX 0x7f

#define U16_MIN 0u
#define U16_MAX 0xffffu
#define I16_MIN (-0x7fff - 1)
#define I16_MAX 0x7fff

#define U32_MIN 0u
#define U32_MAX 0xffffffffu
#define I32_MIN (-0x7fffffff - 1)
#define I32_MAX 0x7fffffff

#define U64_MIN 0ull
#define U64_MAX 0xffffffffffffffffull
#define I64_MIN (-0x7fffffffffffffffll - 1)
#define I64_MAX 0x7fffffffffffffffll

#if defined(GB_ARCH_32_BIT)
	#define USIZE_MIX U32_MIN
	#define USIZE_MAX U32_MAX

	#define ISIZE_MIX S32_MIN
	#define ISIZE_MAX S32_MAX
#elif defined(GB_ARCH_64_BIT)
	#define USIZE_MIX U64_MIN
	#define USIZE_MAX U64_MAX

	#define ISIZE_MIX I64_MIN
	#define ISIZE_MAX I64_MAX
#else
	#error Unknown architecture size. This library only supports 32 bit and 64 bit architectures.
#endif

#define F32_MIN 1.17549435e-38f
#define F32_MAX 3.40282347e+38f

#define F64_MIN 2.2250738585072014e-308
#define F64_MAX 1.7976931348623157e+308

#endif

#ifndef NULL
	#if defined(__cplusplus)
		#if __cplusplus >= 201103L
			#define NULL nullptr
		#else
			#define NULL 0
		#endif
	#else
		#define NULL ((void *)0)
	#endif
#endif

// TODO(bill): Is this enough to get inline working?
#if !defined(__cplusplus)
	#if defined(_MSC_VER) && _MSC_VER <= 1800
	#define inline __inline
	#elif !defined(__STDC_VERSION__)
	#define inline __inline__
	#else
	#define inline
	#endif
#endif

#if !defined(gb_restrict)
	#if defined(_MSC_VER)
		#define gb_restrict __restrict
	#elif defined(__STDC_VERSION__)
		#define gb_restrict restrict
	#else
		#define gb_restrict
	#endif
#endif

// TODO(bill): Should force inline be a separate keyword and gb_inline be inline?
#if !defined(gb_inline)
	#if defined(_MSC_VER)
		#if _MSC_VER < 1300
		#define gb_inline
		#else
		#define gb_inline __forceinline
		#endif
	#else
		#define gb_inline __attribute__ ((__always_inline__))
	#endif
#endif

#if !defined(gb_no_inline)
	#if defined(_MSC_VER)
		#define gb_no_inline __declspec(noinline)
	#else
		#define gb_no_inline __attribute__ ((noinline))
	#endif
#endif


#if !defined(gb_thread_local)
	#if defined(_MSC_VER) && _MSC_VER >= 1300
		#define gb_thread_local __declspec(thread)
	#elif defined(__GNUC__)
		#define gb_thread_local __thread
	#else
		#define gb_thread_local thread_local
	#endif
#endif


// NOTE(bill): Easy to grep
// NOTE(bill): Not needed in macros
#ifndef cast
#define cast(Type) (Type)
#endif

// NOTE(bill): Because a signed sizeof is more useful
#ifndef gb_size_of
#define gb_size_of(x) (isize)(sizeof(x))
#endif

#ifndef gb_count_of
#define gb_count_of(x) ((gb_size_of(x)/gb_size_of(0[x])) / ((isize)(!(gb_size_of(x) % gb_size_of(0[x])))))
#endif

#ifndef gb_offset_of
#define gb_offset_of(Type, element) ((isize)&(((Type *)0)->element))
#endif

#if defined(__cplusplus)
#ifndef gb_align_of
	#if __cplusplus >= 201103L
		#define gb_align_of(Type) (isize)alignof(Type)
	#else
extern "C++" {
		// NOTE(bill): Fucking Templates!
		template <typename T> struct gbAlignment_Trick { char c; T member; };
		#define gb_align_of(Type) gb_offset_of(gbAlignment_Trick<Type>, member)
}
	#endif
#endif
#else
	#ifndef gb_align_of
	#define gb_align_of(Type) gb_offset_of(struct { char c; Type member; }, member)
	#endif
#endif

// NOTE(bill): I do wish I had a type_of that was portable
#ifndef gb_swap
#define gb_swap(Type, a, b) do { Type tmp = (a); (a) = (b); (b) = tmp; } while (0)
#endif

// NOTE(bill): Because static means 3/4 different things in C/C++. Great design (!)
#ifndef gb_global
#define gb_global        static // Global variables
#define gb_internal      static // Internal linkage
#define gb_local_persist static // Local Persisting variables
#endif


#ifndef gb_unused
	#if defined(_MSC_VER)
		#define gb_unused(x) (__pragma(warning(suppress:4100))(x))
	#elif defined (__GCC__)
		#define gb_unused(x) __attribute__((__unused__))(x)
	#else
		#define gb_unused(x) ((void)(gb_size_of(x)))
	#endif
#endif




////////////////////////////////////////////////////////////////
//
// Defer statement
// Akin to D's SCOPE_EXIT or
// similar to Go's defer but scope-based
//
// NOTE: C++11 (and above) only!
//
#if !defined(GB_NO_DEFER) && defined(__cplusplus) && ((defined(_MSC_VER) && _MSC_VER >= 1400) || (__cplusplus >= 201103L))
extern "C++" {
	// NOTE(bill): Stupid fucking templates
	template <typename T> struct gbRemoveReference       { typedef T Type; };
	template <typename T> struct gbRemoveReference<T &>  { typedef T Type; };
	template <typename T> struct gbRemoveReference<T &&> { typedef T Type; };

	/// NOTE(bill): "Move" semantics - invented because the C++ committee are idiots (as a collective not as indiviuals (well a least some aren't))
	template <typename T> inline T &&gb_forward(typename gbRemoveReference<T>::Type &t)  { return static_cast<T &&>(t); }
	template <typename T> inline T &&gb_forward(typename gbRemoveReference<T>::Type &&t) { return static_cast<T &&>(t); }
	template <typename T> inline T &&gb_move   (T &&t)                                   { return static_cast<typename gbRemoveReference<T>::Type &&>(t); }
	template <typename F>
	struct gbprivDefer {
		F f;
		gbprivDefer(F &&f) : f(gb_forward<F>(f)) {}
		~gbprivDefer() { f(); }
	};
	template <typename F> gbprivDefer<F> gb__defer_func(F &&f) { return gbprivDefer<F>(gb_forward<F>(f)); }

	#define GB_DEFER_1(x, y) x##y
	#define GB_DEFER_2(x, y) GB_DEFER_1(x, y)
	#define GB_DEFER_3(x)    GB_DEFER_2(x, __COUNTER__)
	#define defer(code)      auto GB_DEFER_3(_defer_) = gb__defer_func([&]()->void{code;})
}

// Example
#if 0
	gbMutex m;
	gb_mutex_init(&m);
	{
		gb_mutex_lock(&m);
		defer (gb_mutex_unlock(&m));

		...
	}
#endif

#endif


////////////////////////////////////////////////////////////////
//
// Macro Fun!
//
//

#ifndef GB_JOIN_MACROS
#define GB_JOIN_MACROS
	#define GB_JOIN2_IND(a, b) a##b

	#define GB_JOIN2(a, b)       GB_JOIN2_IND(a, b)
	#define GB_JOIN3(a, b, c)    GB_JOIN2(GB_JOIN2(a, b), c)
	#define GB_JOIN4(a, b, c, d) GB_JOIN2(GB_JOIN2(GB_JOIN2(a, b), c), d)
#endif


#ifndef GB_BIT
#define GB_BIT(x) (1<<(x))
#endif

#ifndef gb_min
#define gb_min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef gb_max
#define gb_max(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef gb_min3
#define gb_min3(a, b, c) gb_min(gb_min(a, b), c)
#endif

#ifndef gb_max3
#define gb_max3(a, b, c) gb_max(gb_max(a, b), c)
#endif

#ifndef gb_clamp
#define gb_clamp(x, lower, upper) gb_min(gb_max((x), (lower)), (upper))
#endif

#ifndef gb_clamp01
#define gb_clamp01(x) gb_clamp((x), 0, 1)
#endif

#ifndef gb_is_between
#define gb_is_between(x, lower, upper) (((x) >= (lower)) && ((x) <= (upper)))
#endif

#ifndef gb_abs
#define gb_abs(x) ((x) < 0 ? -(x) : (x))
#endif

/* NOTE(bill): Very useful bit setting */
#ifndef GB_MASK_SET
#define GB_MASK_SET(var, set, mask) do { \
	if (set) (var) |=  (mask); \
	else     (var) &= ~(mask); \
} while (0)
#endif


// NOTE(bill): Some compilers support applying printf-style warnings to user functions.
#if defined(__clang__) || defined(__GNUC__)
#define GB_PRINTF_ARGS(FMT) __attribute__((format(printf, FMT, (FMT+1))))
#else
#define GB_PRINTF_ARGS(FMT)
#endif

////////////////////////////////////////////////////////////////
//
// Debug
//
//


#ifndef GB_DEBUG_TRAP
	#if defined(_MSC_VER)
	 	#if _MSC_VER < 1300
		#define GB_DEBUG_TRAP() __asm int 3 /* Trap to debugger! */
		#else
		#define GB_DEBUG_TRAP() __debugbreak()
		#endif
	#else
		#define GB_DEBUG_TRAP() __builtin_trap()
	#endif
#endif

#ifndef GB_ASSERT_MSG
#define GB_ASSERT_MSG(cond, msg, ...) do { \
	if (!(cond)) { \
		gb_assert_handler(#cond, __FILE__, cast(i64)__LINE__, msg, ##__VA_ARGS__); \
		GB_DEBUG_TRAP(); \
	} \
} while (0)
#endif

#ifndef GB_ASSERT
#define GB_ASSERT(cond) GB_ASSERT_MSG(cond, NULL)
#endif

#ifndef GB_ASSERT_NOT_NULL
#define GB_ASSERT_NOT_NULL(ptr) GB_ASSERT_MSG((ptr) != NULL, #ptr " must not be NULL")
#endif

// NOTE(bill): Things that shouldn't happen with a message!
#ifndef GB_PANIC
#define GB_PANIC(msg, ...) GB_ASSERT_MSG(0, msg, ##__VA_ARGS__)
#endif

GB_DEF void gb_assert_handler(char const *condition, char const *file, i32 line, char const *msg, ...);



////////////////////////////////////////////////////////////////
//
// Memory
//
//


GB_DEF b32 gb_is_power_of_two(isize x);

GB_DEF void *      gb_align_forward(void *ptr, isize alignment);

GB_DEF void *      gb_pointer_add      (void *ptr, isize bytes);
GB_DEF void *      gb_pointer_sub      (void *ptr, isize bytes);
GB_DEF void const *gb_pointer_add_const(void const *ptr, isize bytes);
GB_DEF void const *gb_pointer_sub_const(void const *ptr, isize bytes);
GB_DEF isize       gb_pointer_diff     (void const *begin, void const *end);


GB_DEF void gb_zero_size(void *ptr, isize size);
#ifndef     gb_zero_item
#define     gb_zero_item(t)         gb_zero_size((t), gb_size_of(*(t))) // NOTE(bill): Pass pointer of struct
#define     gb_zero_array(a, count) gb_zero_size((a), gb_size_of(*(a))*count)
#endif

GB_DEF void *      gb_memcopy   (void *dest, void const *source, isize size);
GB_DEF void *      gb_memmove   (void *dest, void const *source, isize size);
GB_DEF void *      gb_memset    (void *data, u8 byte_value, isize size);
GB_DEF i32         gb_memcompare(void const *s1, void const *s2, isize size);
GB_DEF void        gb_memswap   (void *i, void *j, isize size);
GB_DEF void const *gb_memchr    (void const *data, u8 byte_value, isize size);
GB_DEF void const *gb_memrchr   (void const *data, u8 byte_value, isize size);


// NOTE(bill): Very similar to doing `*cast(T *)(&u)`
#ifndef GB_BIT_CAST
#define GB_BIT_CAST(dest, source) do { \
	GB_STATIC_ASSERT(gb_size_of(*(dest)) <= gb_size_of(source)); \
	gb_memcopy((dest), &(source), gb_size_of(*dest)); \
} while (0)
#endif




#ifndef gb_kilobytes
#define gb_kilobytes(x) (            (x) * (i64)(1024))
#define gb_megabytes(x) (gb_kilobytes(x) * (i64)(1024))
#define gb_gigabytes(x) (gb_megabytes(x) * (i64)(1024))
#define gb_terabytes(x) (gb_gigabytes(x) * (i64)(1024))
#endif




// Atomics

// TODO(bill): Be specific with memory order?
// e.g. relaxed, acquire, release, acquire_release

#if defined(GB_COMPILER_MSVC)
typedef struct gbAtomic32  { i32   volatile value; } gbAtomic32;
typedef struct gbAtomic64  { i64   volatile value; } gbAtomic64;
typedef struct gbAtomicPtr { void *volatile value; } gbAtomicPtr;
#else
	#if defined(GB_ARCH_32_BIT)
	#define GB_ATOMIC_PTR_ALIGNMENT 4
	#elif defined(GB_ARCH_64_BIT)
	#define GB_ATOMIC_PTR_ALIGNMENT 8
	#else
	#error Unknown architecture
	#endif

typedef struct gbAtomic32  { i32   volatile value; } __attribute__ ((aligned(4))) gbAtomic32;
typedef struct gbAtomic64  { i64   volatile value; } __attribute__ ((aligned(8))) gbAtomic64;
typedef struct gbAtomicPtr { void *volatile value; } __attribute__ ((aligned(GB_ATOMIC_PTR_ALIGNMENT))) gbAtomicPtr;
#endif

GB_DEF i32  gb_atomic32_load            (gbAtomic32 const volatile *a);
GB_DEF void gb_atomic32_store           (gbAtomic32 volatile *a, i32 value);
GB_DEF i32  gb_atomic32_compare_exchange(gbAtomic32 volatile *a, i32 expected, i32 desired);
GB_DEF i32  gb_atomic32_exchanged       (gbAtomic32 volatile *a, i32 desired);
GB_DEF i32  gb_atomic32_fetch_add       (gbAtomic32 volatile *a, i32 operand);
GB_DEF i32  gb_atomic32_fetch_and       (gbAtomic32 volatile *a, i32 operand);
GB_DEF i32  gb_atomic32_fetch_or        (gbAtomic32 volatile *a, i32 operand);
GB_DEF b32  gb_atomic32_spin_lock       (gbAtomic32 volatile *a, isize time_out); // NOTE(bill): time_out = -1 as default
GB_DEF void gb_atomic32_spin_unlock     (gbAtomic32 volatile *a);
GB_DEF b32  gb_atomic32_try_acquire_lock(gbAtomic32 volatile *a);


GB_DEF i64  gb_atomic64_load            (gbAtomic64 const volatile *a);
GB_DEF void gb_atomic64_store           (gbAtomic64 volatile *a, i64 value);
GB_DEF i64  gb_atomic64_compare_exchange(gbAtomic64 volatile *a, i64 expected, i64 desired);
GB_DEF i64  gb_atomic64_exchanged       (gbAtomic64 volatile *a, i64 desired);
GB_DEF i64  gb_atomic64_fetch_add       (gbAtomic64 volatile *a, i64 operand);
GB_DEF i64  gb_atomic64_fetch_and       (gbAtomic64 volatile *a, i64 operand);
GB_DEF i64  gb_atomic64_fetch_or        (gbAtomic64 volatile *a, i64 operand);
GB_DEF b32  gb_atomic64_spin_lock       (gbAtomic64 volatile *a, isize time_out); // NOTE(bill): time_out = -1 as default
GB_DEF void gb_atomic64_spin_unlock     (gbAtomic64 volatile *a);
GB_DEF b32  gb_atomic64_try_acquire_lock(gbAtomic64 volatile *a);


GB_DEF void *gb_atomic_ptr_load            (gbAtomicPtr const volatile *a);
GB_DEF void  gb_atomic_ptr_store           (gbAtomicPtr volatile *a, void *value);
GB_DEF void *gb_atomic_ptr_compare_exchange(gbAtomicPtr volatile *a, void *expected, void *desired);
GB_DEF void *gb_atomic_ptr_exchanged       (gbAtomicPtr volatile *a, void *desired);
GB_DEF void *gb_atomic_ptr_fetch_add       (gbAtomicPtr volatile *a, void *operand);
GB_DEF void *gb_atomic_ptr_fetch_and       (gbAtomicPtr volatile *a, void *operand);
GB_DEF void *gb_atomic_ptr_fetch_or        (gbAtomicPtr volatile *a, void *operand);
GB_DEF b32   gb_atomic_ptr_spin_lock       (gbAtomicPtr volatile *a, isize time_out); // NOTE(bill): time_out = -1 as default
GB_DEF void  gb_atomic_ptr_spin_unlock     (gbAtomicPtr volatile *a);
GB_DEF b32   gb_atomic_ptr_try_acquire_lock(gbAtomicPtr volatile *a);


// Fences
GB_DEF void gb_yield_thread(void);
GB_DEF void gb_mfence      (void);
GB_DEF void gb_sfence      (void);
GB_DEF void gb_lfence      (void);


#if defined(GB_SYSTEM_WINDOWS)
typedef struct gbSemaphore { void *win32_handle; }     gbSemaphore;
#elif defined(GB_SYSTEM_OSX)
typedef struct gbSemaphore { semaphore_t osx_handle; } gbSemaphore;
#elif defined(GB_SYSTEM_UNIX)
typedef struct gbSemaphore { sem_t unix_handle; }      gbSemaphore;
#else
#error
#endif

GB_DEF void gb_semaphore_init   (gbSemaphore *s);
GB_DEF void gb_semaphore_destroy(gbSemaphore *s);
GB_DEF void gb_semaphore_post   (gbSemaphore *s, i32 count);
GB_DEF void gb_semaphore_release(gbSemaphore *s); // NOTE(bill): gb_semaphore_post(s, 1)
GB_DEF void gb_semaphore_wait   (gbSemaphore *s);


// Mutex
// TODO(bill): Should this be replaced with a CRITICAL_SECTION on win32 or is the better?
typedef struct gbMutex {
	gbSemaphore semaphore;
	gbAtomic32  counter;
	gbAtomic32  owner;
	i32         recursion;
} gbMutex;

GB_DEF void gb_mutex_init    (gbMutex *m);
GB_DEF void gb_mutex_destroy (gbMutex *m);
GB_DEF void gb_mutex_lock    (gbMutex *m);
GB_DEF b32  gb_mutex_try_lock(gbMutex *m);
GB_DEF void gb_mutex_unlock  (gbMutex *m);

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



#define GB_THREAD_PROC(name) void name(void *data)
typedef GB_THREAD_PROC(gbThreadProc);

typedef struct gbThread {
#if defined(GB_SYSTEM_WINDOWS)
	void *        win32_handle;
#else
	pthread_t     posix_handle;
#endif

	gbThreadProc *proc;
	void *        data;

	gbSemaphore   semaphore;
	isize         stack_size;
	b32           is_running;
} gbThread;

GB_DEF void gb_thread_init            (gbThread *t);
GB_DEF void gb_thread_destory         (gbThread *t);
GB_DEF void gb_thread_start           (gbThread *t, gbThreadProc *proc, void *data);
GB_DEF void gb_thread_start_with_stack(gbThread *t, gbThreadProc *proc, void *data, isize stack_size);
GB_DEF void gb_thread_join            (gbThread *t);
GB_DEF b32  gb_thread_is_running      (gbThread const *t);
GB_DEF u32  gb_thread_current_id      (void);
GB_DEF void gb_thread_set_name        (gbThread *t, char const *name);


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

GB_DEF void gb_sync_init          (gbSync *s);
GB_DEF void gb_sync_destroy       (gbSync *s);
GB_DEF void gb_sync_set_target    (gbSync *s, i32 count);
GB_DEF void gb_sync_release       (gbSync *s);
GB_DEF i32  gb_sync_reach         (gbSync *s);
GB_DEF void gb_sync_reach_and_wait(gbSync *s);



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
	b32   is_accurate;
	isize core_count;
	isize thread_count;
	isize threads_per_core;
} gbAffinity;
#else
#error TODO(bill): Unknown system
#endif

GB_DEF void  gb_affinity_init   (gbAffinity *a);
GB_DEF void  gb_affinity_destroy(gbAffinity *a);
GB_DEF b32   gb_affinity_set    (gbAffinity *a, isize core, isize thread);
GB_DEF isize gb_affinity_thread_count_for_core(gbAffinity *a, isize core);




////////////////////////////////////////////////////////////////
//
// Virtual Memory
//
//

typedef struct gbVirtualMemory {
	void *data;
	isize size;
} gbVirtualMemory;

GB_DEF gbVirtualMemory gb_virtual_memory(void *data, isize size);
GB_DEF gbVirtualMemory gb_vm_alloc      (void *addr, isize size);
GB_DEF b32             gb_vm_free       (gbVirtualMemory vm);
GB_DEF gbVirtualMemory gb_vm_trim       (gbVirtualMemory vm, isize lead_size, isize size);
GB_DEF b32             gb_vm_purge      (gbVirtualMemory vm);
GB_DEF isize gb_virtual_memory_page_size(isize *alignment_out);




////////////////////////////////////////////////////////////////
//
// Custom Allocation
//
//

typedef enum gbAllocationType {
	gbAllocation_Alloc,
	gbAllocation_Free,
	gbAllocation_FreeAll,
	gbAllocation_Resize,
} gbAllocationType;

// NOTE(bill): This is useful so you can define an allocator of the same type and parameters
#define GB_ALLOCATOR_PROC(name)                         \
void *name(void *allocator_data, gbAllocationType type, \
           isize size, isize alignment,                 \
           void *old_memory, isize old_size,            \
           u64 flags)
typedef GB_ALLOCATOR_PROC(gbAllocatorProc);

typedef struct gbAllocator {
	gbAllocatorProc *proc;
	void *           data;
} gbAllocator;

typedef enum gbAllocatorFlag {
	gbAllocatorFlag_ClearToZero = GB_BIT(0),
} gbAllocatorFlag;

// TODO(bill): Is this a decent default alignment?
#ifndef GB_DEFAULT_MEMORY_ALIGNMENT
#define GB_DEFAULT_MEMORY_ALIGNMENT (2 * gb_size_of(void *))
#endif

#ifndef GB_DEFAULT_ALLOCATOR_FLAGS
#define GB_DEFAULT_ALLOCATOR_FLAGS (gbAllocatorFlag_ClearToZero)
#endif

GB_DEF void *gb_alloc_align (gbAllocator a, isize size, isize alignment);
GB_DEF void *gb_alloc       (gbAllocator a, isize size);
GB_DEF void  gb_free        (gbAllocator a, void *ptr);
GB_DEF void  gb_free_all    (gbAllocator a);
GB_DEF void *gb_resize      (gbAllocator a, void *ptr, isize old_size, isize new_size);
GB_DEF void *gb_resize_align(gbAllocator a, void *ptr, isize old_size, isize new_size, isize alignment);
// TODO(bill): For gb_resize, should the use need to pass the old_size or only the new_size?

GB_DEF void *gb_alloc_copy      (gbAllocator a, void const *src, isize size);
GB_DEF void *gb_alloc_copy_align(gbAllocator a, void const *src, isize size, isize alignment);
GB_DEF char *gb_alloc_str       (gbAllocator a, char const *str);
GB_DEF char *gb_alloc_str_len   (gbAllocator a, char const *str, isize len);


// NOTE(bill): These are very useful and the type cast has saved me from numerous bugs
#ifndef gb_alloc_item
#define gb_alloc_item(allocator_, Type)         (Type *)gb_alloc(allocator_, gb_size_of(Type))
#define gb_alloc_array(allocator_, Type, count) (Type *)gb_alloc(allocator_, gb_size_of(Type) * (count))
#endif

// NOTE(bill): Use this if you don't need a "fancy" resize allocation
GB_DEF void *gb_default_resize_align(gbAllocator a, void *ptr, isize old_size, isize new_size, isize alignment);



// TODO(bill): Probably use a custom heap allocator system that doesn't depend on malloc/free
// Base it off TCMalloc or something else? Or something entirely custom?
GB_DEF gbAllocator gb_heap_allocator(void);
GB_DEF GB_ALLOCATOR_PROC(gb_heap_allocator_proc);

// NOTE(bill): Yep, I use my own allocator system!
#ifndef gb_malloc
#define gb_malloc(sz) gb_alloc(gb_heap_allocator(), sz)
#define gb_mfree(ptr) gb_free(gb_heap_allocator(), ptr)
#endif



//
// Arena Allocator
//
typedef struct gbArena {
	gbAllocator backing;
	void *      physical_start;
	isize       total_size;
	isize       total_allocated;
	isize       temp_count;
} gbArena;

GB_DEF void gb_arena_init_from_memory   (gbArena *arena, void *start, isize size);
GB_DEF void gb_arena_init_from_allocator(gbArena *arena, gbAllocator backing, isize size);
GB_DEF void gb_arena_init_sub           (gbArena *arena, gbArena *parent_arena, isize size);
GB_DEF void gb_arena_free               (gbArena *arena);

GB_DEF isize gb_arena_alignment_of  (gbArena *arena, isize alignment);
GB_DEF isize gb_arena_size_remaining(gbArena *arena, isize alignment);
GB_DEF void  gb_arena_check         (gbArena *arena);


// Allocation Types: alloc, free_all, resize
GB_DEF gbAllocator gb_arena_allocator(gbArena *arena);
GB_DEF GB_ALLOCATOR_PROC(gb_arena_allocator_proc);



typedef struct gbTempArenaMemory {
	gbArena *arena;
	isize    original_count;
} gbTempArenaMemory;

GB_DEF gbTempArenaMemory gb_temp_arena_memory_begin(gbArena *arena);
GB_DEF void              gb_temp_arena_memory_end  (gbTempArenaMemory tmp_mem);







//
// Pool Allocator
//


typedef struct gbPool {
	gbAllocator backing;
	void *      physical_start;
	void *      free_list;
	isize       block_size;
	isize       block_align;
	isize       total_size;
} gbPool;

GB_DEF void gb_pool_init      (gbPool *pool, gbAllocator backing, isize num_blocks, isize block_size);
GB_DEF void gb_pool_init_align(gbPool *pool, gbAllocator backing, isize num_blocks, isize block_size, isize block_align);
GB_DEF void gb_pool_free      (gbPool *pool);

// Allocation Types: alloc, free
GB_DEF gbAllocator gb_pool_allocator(gbPool *pool);
GB_DEF GB_ALLOCATOR_PROC(gb_pool_allocator_proc);



// NOTE(bill): Used for allocators to keep track of sizes
typedef struct gbAllocationHeader {
	isize size;
} gbAllocationHeader;

GB_DEF gbAllocationHeader *gb_allocation_header     (void *data);
GB_DEF void                gb_allocation_header_fill(gbAllocationHeader *header, void *data, isize size);

// TODO(bill): Find better way of doing this without #if #elif etc.
#if defined(GB_ARCH_32_BIT)
#define GB_ISIZE_HIGH_BIT 0x80000000
#elif defined(GB_ARCH_64_BIT)
#define GB_ISIZE_HIGH_BIT 0x8000000000000000ll
#else
#error
#endif

//
// Free List Allocator
//

// IMPORTANT TODO(bill): Thoroughly test the free list allocator!
// NOTE(bill): This is a very shitty free list as it just picks the first free block not the best size
// as I am just being lazy. Also, I will probably remove it later; it's only here because why not?!
//
// NOTE(bill): I may also complete remove this if I completely implement a fixed heap allocator

typedef struct gbFreeListBlock gbFreeListBlock;
struct gbFreeListBlock {
	gbFreeListBlock *next;
	isize            size;
};

typedef struct gbFreeList {
	void *           physical_start;
	isize            total_size;

	gbFreeListBlock *curr_block;

	isize            total_allocated;
	isize            allocation_count;
} gbFreeList;

GB_DEF void gb_free_list_init               (gbFreeList *fl, void *start, isize size);
GB_DEF void gb_free_list_init_from_allocator(gbFreeList *fl, gbAllocator backing, isize size);

// Allocation Types: alloc, free, free_all, resize
GB_DEF gbAllocator gb_free_list_allocator(gbFreeList *fl);
GB_DEF GB_ALLOCATOR_PROC(gb_free_list_allocator_proc);



//
// Scratch Memory Allocator - Ring Buffer Based Arena
//

typedef struct gbScratchMemory {
	void *physical_start;
	isize total_size;
	void *alloc_point;
	void *free_point;
} gbScratchMemory;

GB_DEF void gb_scratch_memory_init     (gbScratchMemory *s, void *start, isize size);
GB_DEF b32  gb_scratch_memory_is_in_use(gbScratchMemory *s, void *ptr);


// Allocation Types: alloc, free, free_all, resize
GB_DEF gbAllocator gb_scratch_allocator(gbScratchMemory *s);
GB_DEF GB_ALLOCATOR_PROC(gb_scratch_allocator_proc);

// TODO(bill): Stack allocator
// TODO(bill): Fixed heap allocator
// TODO(bill): General heap allocator. Maybe a TCMalloc like clone?


////////////////////////////////////////////////////////////////
//
// Sort & Search
//
//

#define GB_COMPARE_PROC(name) int name(void const *a, void const *b)
typedef GB_COMPARE_PROC(gbCompareProc);

#define GB_COMPARE_PROC_PTR(def) GB_COMPARE_PROC((*def))

// Producure pointers
// NOTE(bill): The offset parameter specifies the offset in the structure
// e.g. gb_i32_cmp(gb_offset_of(Thing, value))
// Use 0 if it's just the type instead.

GB_DEF GB_COMPARE_PROC_PTR(gb_i16_cmp  (isize offset));
GB_DEF GB_COMPARE_PROC_PTR(gb_i32_cmp  (isize offset));
GB_DEF GB_COMPARE_PROC_PTR(gb_i64_cmp  (isize offset));
GB_DEF GB_COMPARE_PROC_PTR(gb_isize_cmp(isize offset));
GB_DEF GB_COMPARE_PROC_PTR(gb_str_cmp  (isize offset));
GB_DEF GB_COMPARE_PROC_PTR(gb_f32_cmp  (isize offset));
GB_DEF GB_COMPARE_PROC_PTR(gb_f64_cmp  (isize offset));
GB_DEF GB_COMPARE_PROC_PTR(gb_char_cmp (isize offset));

// TODO(bill): Better sorting algorithms
// NOTE(bill): Uses quick sort for large arrays but insertion sort for small
#define gb_sort_array(array, count, compare_proc) gb_sort(array, count, gb_size_of(*(array)), compare_proc)
GB_DEF void gb_sort(void *base, isize count, isize size, gbCompareProc compare_proc);

// NOTE(bill): the count of temp == count of items
#define gb_radix_sort(Type) gb_radix_sort_##Type
#define GB_RADIX_SORT_PROC(Type) void gb_radix_sort(Type)(Type *items, Type *temp, isize count)

GB_DEF GB_RADIX_SORT_PROC(u8);
GB_DEF GB_RADIX_SORT_PROC(u16);
GB_DEF GB_RADIX_SORT_PROC(u32);
GB_DEF GB_RADIX_SORT_PROC(u64);


// NOTE(bill): Returns index or -1 if not found
#define gb_binary_search_array(array, count, key, compare_proc) gb_binary_search(array, count, gb_size_of(*(array)), key, compare_proc)
GB_DEF isize gb_binary_search(void const *base, isize count, isize size, void const *key, gbCompareProc compare_proc);

#define gb_shuffle_array(array, count) gb_shuffle(array, count, gb_size_of(*(array)))
GB_DEF void gb_shuffle(void *base, isize count, isize size);

#define gb_reverse_array(array, count) gb_reverse(array, count, gb_size_of(*(array)))
GB_DEF void gb_reverse(void *base, isize count, isize size);

////////////////////////////////////////////////////////////////
//
// Char Functions
//
//

GB_DEF char gb_char_to_lower       (char c);
GB_DEF char gb_char_to_upper       (char c);
GB_DEF b32  gb_char_is_space       (char c);
GB_DEF b32  gb_char_is_digit       (char c);
GB_DEF b32  gb_char_is_hex_digit   (char c);
GB_DEF b32  gb_char_is_alpha       (char c);
GB_DEF b32  gb_char_is_alphanumeric(char c);
GB_DEF i32  gb_digit_to_int        (char c);
GB_DEF i32  gb_hex_digit_to_int    (char c);

// NOTE(bill): ASCII only
GB_DEF void gb_str_to_lower(char *str);
GB_DEF void gb_str_to_upper(char *str);

GB_DEF isize gb_strlen (char const *str);
GB_DEF isize gb_strnlen(char const *str, isize max_len);
GB_DEF i32   gb_strcmp (char const *s1, char const *s2);
GB_DEF i32   gb_strncmp(char const *s1, char const *s2, isize len);
GB_DEF char *gb_strcpy (char *dest, char const *source);
GB_DEF char *gb_strncpy(char *dest, char const *source, isize len);
GB_DEF isize gb_strlcpy(char *dest, char const *source, isize len);
GB_DEF char *gb_strrev (char *str); // NOTE(bill): ASCII only

// NOTE(bill): A less fucking crazy strtok!
GB_DEF char const *gb_strtok(char *output, char const *src, char const *delimit);

GB_DEF b32 gb_str_has_prefix(char const *str, char const *prefix);
GB_DEF b32 gb_str_has_suffix(char const *str, char const *suffix);

GB_DEF char const *gb_char_first_occurence(char const *str, char c);
GB_DEF char const *gb_char_last_occurence (char const *str, char c);

GB_DEF void gb_str_concat(char *dest, isize dest_len,
                          char const *src_a, isize src_a_len,
                          char const *src_b, isize src_b_len);

GB_DEF u64   gb_str_to_u64(char const *str, char **end_ptr, i32 base); // TODO(bill): Support more than just decimal and hexadecimal
GB_DEF i64   gb_str_to_i64(char const *str, char **end_ptr, i32 base); // TODO(bill): Support more than just decimal and hexadecimal
GB_DEF f32   gb_str_to_f32(char const *str, char **end_ptr);
GB_DEF f64   gb_str_to_f64(char const *str, char **end_ptr);
GB_DEF void  gb_i64_to_str(i64 value, char *string, i32 base);
GB_DEF void  gb_u64_to_str(u64 value, char *string, i32 base);


////////////////////////////////////////////////////////////////
//
// UTF-8 Handling
//
//

// NOTE(bill): Does not check if utf-8 string is valid
GB_DEF isize gb_utf8_strlen (u8 const *str);
GB_DEF isize gb_utf8_strnlen(u8 const *str, isize max_len);

// NOTE(bill): Windows doesn't handle 8 bit filenames well ('cause Micro$hit)
GB_DEF u16 *gb_utf8_to_ucs2    (u16 *buffer, isize len, u8 const *str);
GB_DEF u8 * gb_ucs2_to_utf8    (u8 *buffer, isize len, u16 const *str);
GB_DEF u16 *gb_utf8_to_ucs2_buf(u8 const *str);   // NOTE(bill): Uses locally persisting buffer
GB_DEF u8 * gb_ucs2_to_utf8_buf(u16 const *str); // NOTE(bill): Uses locally persisting buffer

// NOTE(bill): Returns size of codepoint in bytes
GB_DEF isize gb_utf8_decode        (u8 const *str, isize str_len, Rune *codepoint);
GB_DEF isize gb_utf8_codepoint_size(u8 const *str, isize str_len);
GB_DEF isize gb_utf8_encode_rune   (u8 buf[4], Rune r);

////////////////////////////////////////////////////////////////
//
// gbString - C Read-Only-Compatible
//
//
/*
Reasoning:

	By default, strings in C are null terminated which means you have to count
	the number of character up to the null character to calculate the length.
	Many "better" C string libraries will create a struct for a string.
	i.e.

	    struct String {
	    	Allocator allocator;
	        size_t    length;
	        size_t    capacity;
	        char *    cstring;
	    };

	This library tries to augment normal C strings in a better way that is still
	compatible with C-style strings.

	+--------+-----------------------+-----------------+
	| Header | Binary C-style String | Null Terminator |
	+--------+-----------------------+-----------------+
	         |
	         +-> Pointer returned by functions

	Due to the meta-data being stored before the string pointer and every gb string
	having an implicit null terminator, gb strings are full compatible with c-style
	strings and read-only functions.

Advantages:

    * gb strings can be passed to C-style string functions without accessing a struct
      member of calling a function, i.e.

          gb_printf("%s\n", gb_str);

      Many other libraries do either of these:

          gb_printf("%s\n", string->cstr);
          gb_printf("%s\n", get_cstring(string));

    * You can access each character just like a C-style string:

          gb_printf("%c %c\n", str[0], str[13]);

    * gb strings are singularly allocated. The meta-data is next to the character
      array which is better for the cache.

Disadvantages:

    * In the C version of these functions, many return the new string. i.e.
          str = gb_string_appendc(str, "another string");
      This could be changed to gb_string_appendc(&str, "another string"); but I'm still not sure.

	* This is incompatible with "gb_string.h" strings
*/

#if 0
#define GB_IMPLEMENTATION
#include "gb.h"
int main(int argc, char **argv) {
	gbString str = gb_string_make("Hello");
	gbString other_str = gb_string_make_length(", ", 2);
	str = gb_string_append(str, other_str);
	str = gb_string_appendc(str, "world!");

	gb_printf("%s\n", str); // Hello, world!

	gb_printf("str length = %d\n", gb_string_length(str));

	str = gb_string_set(str, "Potato soup");
	gb_printf("%s\n", str); // Potato soup

	str = gb_string_set(str, "Hello");
	other_str = gb_string_set(other_str, "Pizza");
	if (gb_strings_are_equal(str, other_str))
		gb_printf("Not called\n");
	else
		gb_printf("Called\n");

	str = gb_string_set(str, "Ab.;!...AHello World       ??");
	str = gb_string_trim(str, "Ab.;!. ?");
	gb_printf("%s\n", str); // "Hello World"

	gb_string_free(str);
	gb_string_free(other_str);

	return 0;
}
#endif

// TODO(bill): Should this be a wrapper to gbArray(char) or this extra type safety better?
typedef char *gbString;

// NOTE(bill): If you only need a small string, just use a standard c string or change the size from isize to u16, etc.
typedef struct gbStringHeader {
	gbAllocator allocator;
	isize       length;
	isize       capacity;
} gbStringHeader;

#define GB_STRING_HEADER(str) (cast(gbStringHeader *)(str) - 1)

GB_DEF gbString gb_string_make           (gbAllocator a, char const *str);
GB_DEF gbString gb_string_make_length    (gbAllocator a, void const *str, isize num_bytes);
GB_DEF void     gb_string_free           (gbString str);
GB_DEF gbString gb_string_duplicate      (gbAllocator a, gbString const str);
GB_DEF isize    gb_string_length         (gbString const str);
GB_DEF isize    gb_string_capacity       (gbString const str);
GB_DEF isize    gb_string_available_space(gbString const str);
GB_DEF void     gb_string_clear          (gbString str);
GB_DEF gbString gb_string_append         (gbString str, gbString const other);
GB_DEF gbString gb_string_append_length  (gbString str, void const *other, isize num_bytes);
GB_DEF gbString gb_string_appendc        (gbString str, char const *other);
GB_DEF gbString gb_string_set            (gbString str, char const *cstr);
GB_DEF gbString gb_string_make_space_for (gbString str, isize add_len);
GB_DEF isize    gb_string_allocation_size(gbString const str);
GB_DEF b32      gb_string_are_equal      (gbString const lhs, gbString const rhs);
GB_DEF gbString gb_string_trim           (gbString str, char const *cut_set);
GB_DEF gbString gb_string_trim_space     (gbString str); // Whitespace ` \t\r\n\v\f`



////////////////////////////////////////////////////////////////
//
// Fixed Capacity Buffer (POD Types)
//
//
// gbBuffer(Type) works like gbString or gbArray where the actual type is just a pointer to the first
// element.
//

typedef struct gbBufferHeader {
	isize count;
	isize capacity;
} gbBufferHeader;

#define gbBuffer(Type) Type *

#define GB_BUFFER_HEADER(x)   (cast(gbBufferHeader *)(x) - 1)
#define gb_buffer_count(x)    (GB_BUFFER_HEADER(x)->count)
#define gb_buffer_capacity(x) (GB_BUFFER_HEADER(x)->capacity)

#define gb_buffer_init(x, allocator, cap) do { \
	void **nx = cast(void **)&(x); \
	gbBufferHeader *gb__bh = cast(gbBufferHeader *)gb_alloc((allocator), (cap)*gb_size_of(*(x))); \
	gb__bh->count = 0; \
	gb__bh->capacity = cap; \
	*nx = cast(void *)(gb__bh+1); \
} while (0)


#define gb_buffer_free(x, allocator) (gb_free(allocator, GB_BUFFER_HEADER(x)))

#define gb_buffer_append(x, item) do { (x)[gb_buffer_count(x)++] = (item); } while (0)

#define gb_buffer_appendv(x, items, item_count) do { \
	GB_ASSERT(gb_size_of(*(items)) == gb_size_of(*(x))); \
	GB_ASSERT(gb_buffer_count(x)+item_count <= gb_buffer_capacity(x)); \
	gb_memcopy(&(x)[gb_buffer_count(x)], (items), gb_size_of(*(x))*(item_count)); \
	gb_buffer_count(x) += (item_count); \
} while (0)

#define gb_buffer_pop(x)   do { GB_ASSERT(gb_buffer_count(x) > 0); gb_buffer_count(x)--; } while (0)
#define gb_buffer_clear(x) do { gb_buffer_count(x) = 0; } while (0)



////////////////////////////////////////////////////////////////
//
// Dynamic Array (POD Types)
//
// NOTE(bill): I know this is a macro hell but C is an old (and shit) language with no proper arrays
// Also why the fuck not?! It fucking works! And it has custom allocation, which is already better than C++!
//
// gbArray(Type) works like gbString or gbBuffer where the actual type is just a pointer to the first
// element.
//



// Available Procedures for gbArray(Type)
// gb_array_init
// gb_array_free
// gb_array_set_capacity
// gb_array_grow
// gb_array_append
// gb_array_appendv
// gb_array_pop
// gb_array_clear
// gb_array_resize
// gb_array_reserve
//

#if 0 // Example
void foo(void) {
	isize i;
	int test_values[] = {4, 2, 1, 7};
	gbAllocator a = gb_heap_allocator();
	gbArray(int) items;

	gb_array_init(items, a);

	gb_array_append(items, 1);
	gb_array_append(items, 4);
	gb_array_append(items, 9);
	gb_array_append(items, 16);

	items[1] = 3; // Manually set value
	              // NOTE: No array bounds checking

	for (i = 0; i < items.count; i++)
		gb_printf("%d\n", items[i]);
	// 1
	// 3
	// 9
	// 16

	gb_array_clear(items);

	gb_array_appendv(items, test_values, gb_count_of(test_values));
	for (i = 0; i < items.count; i++)
		gb_printf("%d\n", items[i]);
	// 4
	// 2
	// 1
	// 7

	gb_array_free(items);
}
#endif

typedef struct gbArrayHeader {
	gbAllocator allocator;
	isize       count;
	isize       capacity;
} gbArrayHeader;

// NOTE(bill): This thing is magic!
#define gbArray(Type) Type *

#ifndef GB_ARRAY_GROW_FORMULA
#define GB_ARRAY_GROW_FORMULA(x) (2*(x) + 8)
#endif

GB_STATIC_ASSERT(GB_ARRAY_GROW_FORMULA(0) > 0);

#define GB_ARRAY_HEADER(x)    (cast(gbArrayHeader *)(x) - 1)
#define gb_array_allocator(x) (GB_ARRAY_HEADER(x)->allocator)
#define gb_array_count(x)     (GB_ARRAY_HEADER(x)->count)
#define gb_array_capacity(x)  (GB_ARRAY_HEADER(x)->capacity)

// TODO(bill): Have proper alignment!
#define gb_array_init_reserve(x, allocator_, cap) do { \
	void **gb__array_ = cast(void **)&(x); \
	gbArrayHeader *gb__ah = cast(gbArrayHeader *)gb_alloc(allocator_, gb_size_of(gbArrayHeader)+gb_size_of(*(x))*(cap)); \
	gb__ah->allocator = allocator_; \
	gb__ah->count = 0; \
	gb__ah->capacity = cap; \
	*gb__array_ = cast(void *)(gb__ah+1); \
} while (0)

// NOTE(bill): Give it an initial default capacity
#define gb_array_init(x, allocator) gb_array_init_reserve(x, allocator, GB_ARRAY_GROW_FORMULA(0))

#define gb_array_free(x) do { \
	gbArrayHeader *gb__ah = GB_ARRAY_HEADER(x); \
	gb_free(gb__ah->allocator, gb__ah); \
} while (0)

#define gb_array_set_capacity(x, capacity) do { \
	if (x) { \
		void **gb__array_ = cast(void **)&(x); \
		*gb__array_ = gb__array_set_capacity((x), (capacity), gb_size_of(*(x))); \
	} \
} while (0)

// NOTE(bill): Do not use the thing below directly, use the macro
GB_DEF void *gb__array_set_capacity(void *array, isize capacity, isize element_size);


// TODO(bill): Decide on a decent growing formula for gbArray
#define gb_array_grow(x, min_capacity) do { \
	isize new_capacity = GB_ARRAY_GROW_FORMULA(gb_array_capacity(x)); \
	if (new_capacity < (min_capacity)) \
		new_capacity = (min_capacity); \
	gb_array_set_capacity(x, new_capacity); \
} while (0)


#define gb_array_append(x, item) do { \
	if (gb_array_capacity(x) < gb_array_count(x)+1) \
		gb_array_grow(x, 0); \
	(x)[gb_array_count(x)++] = (item); \
} while (0)

#define gb_array_appendv(x, items, item_count) do { \
	gbArrayHeader *gb__ah = GB_ARRAY_HEADER(x); \
	GB_ASSERT(gb_size_of((items)[0]) == gb_size_of((x)[0])); \
	if (gb__ah->capacity < gb__ah->count+(item_count)) \
		gb_array_grow(x, gb__ah->count+(item_count)); \
	gb_memcopy(&(x)[gb__ah->count], (items), gb_size_of((x)[0])*(item_count));\
	gb__ah->count += (item_count); \
} while (0)



#define gb_array_pop(x)   do { GB_ASSERT(GB_ARRAY_HEADER(x)->count > 0); GB_ARRAY_HEADER(x)->count--; } while (0)
#define gb_array_clear(x) do { GB_ARRAY_HEADER(x)->count = 0; } while (0)

#define gb_array_resize(x, new_count) do { \
	if (GB_ARRAY_HEADER(x)->capacity < (new_count)) \
		gb_array_grow(x, (new_count)); \
	GB_ARRAY_HEADER(x)->count = (new_count); \
} while (0)


#define gb_array_reserve(x, new_capacity) do { \
	if (GB_ARRAY_HEADER(x)->capacity < (new_capacity)) \
		gb_array_set_capacity(x, new_capacity); \
} while (0)





////////////////////////////////////////////////////////////////
//
// Hashing and Checksum Functions
//
//

GB_EXTERN u32 gb_adler32(void const *data, isize len);

GB_EXTERN u32 gb_crc32(void const *data, isize len);
GB_EXTERN u64 gb_crc64(void const *data, isize len);

GB_EXTERN u32 gb_fnv32 (void const *data, isize len);
GB_EXTERN u64 gb_fnv64 (void const *data, isize len);
GB_EXTERN u32 gb_fnv32a(void const *data, isize len);
GB_EXTERN u64 gb_fnv64a(void const *data, isize len);

// NOTE(bill): Default seed of 0x9747b28c
// NOTE(bill): I prefer using murmur64 for most hashes
GB_EXTERN u32 gb_murmur32(void const *data, isize len);
GB_EXTERN u64 gb_murmur64(void const *data, isize len);

GB_EXTERN u32 gb_murmur32_seed(void const *data, isize len, u32 seed);
GB_EXTERN u64 gb_murmur64_seed(void const *data, isize len, u64 seed);


////////////////////////////////////////////////////////////////
//
// Instantiated Hash Table
//
// This is an attempt to implement a templated hash table
// NOTE(bill): The key is aways a u64 for simplicity and you will _probably_ _never_ need anything bigger.
//
// Hash table type and function declaration, call: GB_TABLE_DECLARE(PREFIX, NAME, N, VALUE)
// Hash table function definitions, call: GB_TABLE_DEFINE(NAME, N, VALUE)
//
//     PREFIX  - a prefix for function prototypes e.g. extern, static, etc.
//     NAME    - Name of the Hash Table
//     FUNC    - the name will prefix function names
//     VALUE   - the type of the value to be stored
//
// NOTE(bill): I really wish C had decent metaprogramming capabilities (and no I don't mean C++'s templates either)
//

typedef struct gbHashTableFindResult {
	isize hash_index;
	isize entry_prev;
	isize entry_index;
} gbHashTableFindResult;

#define GB_TABLE(PREFIX, NAME, FUNC, VALUE) \
	GB_TABLE_DECLARE(PREFIX, NAME, FUNC, VALUE); \
	GB_TABLE_DEFINE(NAME, FUNC, VALUE);

#define GB_TABLE_DECLARE(PREFIX, NAME, FUNC, VALUE) \
typedef struct GB_JOIN2(NAME,Entry) { \
	u64 key; \
	isize next; \
	VALUE value; \
} GB_JOIN2(NAME,Entry); \
\
typedef struct NAME { \
	gbArray(isize) hashes; \
	gbArray(GB_JOIN2(NAME,Entry)) entries; \
} NAME; \
\
PREFIX void                  GB_JOIN2(FUNC,init)       (NAME *h, gbAllocator a); \
PREFIX void                  GB_JOIN2(FUNC,destroy)    (NAME *h); \
PREFIX VALUE *               GB_JOIN2(FUNC,get)        (NAME *h, u64 key); \
PREFIX void                  GB_JOIN2(FUNC,set)        (NAME *h, u64 key, VALUE value); \
PREFIX void                  GB_JOIN2(FUNC,grow)       (NAME *h); \
PREFIX void                  GB_JOIN2(FUNC,rehash)     (NAME *h, isize new_count); \





#define GB_TABLE_DEFINE(NAME, FUNC, VALUE) \
void GB_JOIN2(FUNC,init)(NAME *h, gbAllocator a) { \
	gb_array_init(h->hashes,  a); \
	gb_array_init(h->entries, a); \
} \
\
void GB_JOIN2(FUNC,destroy)(NAME *h) { \
	if (h->entries) gb_array_free(h->entries); \
	if (h->hashes)  gb_array_free(h->hashes); \
} \
\
gb_internal isize GB_JOIN2(FUNC,_add_entry)(NAME *h, u64 key) { \
	isize index; \
	GB_JOIN2(NAME,Entry) e = {0}; \
	e.key = key; \
	e.next = -1; \
	index = gb_array_count(h->entries); \
	gb_array_append(h->entries, e); \
	return index; \
} \
\
gb_internal gbHashTableFindResult GB_JOIN2(FUNC,_find)(NAME *h, u64 key) { \
	gbHashTableFindResult r = {-1, -1, -1}; \
	if (gb_array_count(h->hashes) > 0) { \
		r.hash_index  = key % gb_array_count(h->hashes); \
		r.entry_index = h->hashes[r.hash_index]; \
		while (r.entry_index >= 0) { \
			if (h->entries[r.entry_index].key == key) \
				return r; \
			r.entry_prev = r.entry_index; \
			r.entry_index = h->entries[r.entry_index].next; \
		} \
	} \
	return r; \
} \
\
gb_internal b32 GB_JOIN2(FUNC,_full)(NAME *h) { \
	return 0.75f * gb_array_count(h->hashes) < gb_array_count(h->entries); \
} \
\
void GB_JOIN2(FUNC,grow)(NAME *h) { \
	isize new_count = GB_ARRAY_GROW_FORMULA(gb_array_count(h->entries)); \
	GB_JOIN2(FUNC,rehash)(h, new_count); \
} \
\
void GB_JOIN2(FUNC,rehash)(NAME *h, isize new_count) { \
	isize i, j; \
	NAME nh = {0}; \
	GB_JOIN2(FUNC,init)(&nh, gb_array_allocator(h->hashes)); \
	gb_array_resize(nh.hashes, new_count); \
	gb_array_reserve(nh.entries, gb_array_count(h->entries)); \
	for (i = 0; i < new_count; i++) \
		nh.hashes[i] = -1; \
	for (i = 0; i < gb_array_count(h->entries); i++) { \
		GB_JOIN2(NAME,Entry) *e; \
		gbHashTableFindResult fr; \
		if (gb_array_count(nh.hashes) == 0) \
			GB_JOIN2(FUNC,grow)(&nh); \
		e = &nh.entries[i]; \
		fr = GB_JOIN2(FUNC,_find)(&nh, e->key); \
		j = GB_JOIN2(FUNC,_add_entry)(&nh, e->key); \
		if (fr.entry_prev < 0) \
			nh.hashes[fr.hash_index] = j; \
		else \
			nh.entries[fr.entry_prev].next = j; \
		nh.entries[j].next = fr.entry_index; \
		nh.entries[j].value = e->value; \
		if (GB_JOIN2(FUNC,_full)(&nh)) \
			GB_JOIN2(FUNC,grow)(&nh); \
	} \
	GB_JOIN2(FUNC,destroy)(h); \
	h->hashes  = nh.hashes; \
	h->entries = nh.entries; \
} \
\
VALUE *GB_JOIN2(FUNC,get)(NAME *h, u64 key) { \
	isize index = GB_JOIN2(FUNC,_find)(h, key).entry_index; \
	if (index >= 0) \
		return &h->entries[index].value; \
	return NULL; \
} \
\
void GB_JOIN2(FUNC,set)(NAME *h, u64 key, VALUE value) { \
	isize index; \
	gbHashTableFindResult fr; \
	if (gb_array_count(h->hashes) == 0) \
		GB_JOIN2(FUNC,grow)(h); \
	fr = GB_JOIN2(FUNC,_find)(h, key); \
	if (fr.entry_index >= 0) { \
		index = fr.entry_index; \
	} else { \
		index = GB_JOIN2(FUNC,_add_entry)(h, key); \
		if (fr.entry_prev >= 0) { \
			h->entries[fr.entry_prev].next = index; \
		} else { \
			h->hashes[fr.hash_index] = index; \
		} \
	} \
	h->entries[index].value = value; \
	if (GB_JOIN2(FUNC,_full)(h)) \
		GB_JOIN2(FUNC,grow)(h); \
} \




////////////////////////////////////////////////////////////////
//
// File Handling
//


typedef u32 gbFileMode;
typedef enum gbFileModeFlag {
	gbFileMode_Read       = GB_BIT(0),
	gbFileMode_Write      = GB_BIT(1),
	gbFileMode_Append     = GB_BIT(2),
	gbFileMode_Rw         = GB_BIT(3),

	gbFileMode_Modes = gbFileMode_Read | gbFileMode_Write | gbFileMode_Append | gbFileMode_Rw,
} gbFileModeFlag;

// NOTE(bill): Only used internally and for the file operations
typedef enum gbSeekWhenceType {
	gbSeekWhence_Begin   = 0,
	gbSeekWhence_Current = 1,
	gbSeekWhence_End     = 2,
} gbSeekWhenceType;

typedef enum gbFileError {
	gbFileError_None,
	gbFileError_Invalid,
	gbFileError_Exists,
	gbFileError_NotExists,
	gbFileError_Permission,
	gbFileError_TruncationFailure,
} gbFileError;

typedef union gbFileDescriptor {
	void *  p;
	intptr  i;
	uintptr u;
} gbFileDescriptor;

typedef struct gbFileOperations gbFileOperations;

#define GB_FILE_OPEN_PROC(name)     gbFileError name(gbFileDescriptor *fd, gbFileOperations *ops, gbFileMode mode, char const *filename)
#define GB_FILE_READ_AT_PROC(name)  b32         name(gbFileDescriptor fd, void *buffer, isize size, i64 offset, isize *bytes_read)
#define GB_FILE_WRITE_AT_PROC(name) b32         name(gbFileDescriptor fd, void const *buffer, isize size, i64 offset, isize *bytes_written)
#define GB_FILE_SEEK_PROC(name)     b32         name(gbFileDescriptor fd, i64 offset, gbSeekWhenceType whence, i64 *new_offset)
#define GB_FILE_CLOSE_PROC(name)    void        name(gbFileDescriptor fd)
typedef GB_FILE_OPEN_PROC(gbFileOpenProc);
typedef GB_FILE_READ_AT_PROC(gbFileReadProc);
typedef GB_FILE_WRITE_AT_PROC(gbFileWriteProc);
typedef GB_FILE_SEEK_PROC(gbFileSeekProc);
typedef GB_FILE_CLOSE_PROC(gbFileCloseProc);

struct gbFileOperations {
	gbFileReadProc  *read_at;
	gbFileWriteProc *write_at;
	gbFileSeekProc  *seek;
	gbFileCloseProc *close;
};

extern gbFileOperations const gbDefaultFileOperations;


// typedef struct gbDirInfo {
// 	u8 *buf;
// 	isize buf_count;
// 	isize buf_pos;
// } gbDirInfo;

typedef u64 gbFileTime;

typedef struct gbFile {
	gbFileOperations ops;
	gbFileDescriptor fd;
	char const *     filename;
	gbFileTime       last_write_time;
	// gbDirInfo *   dir_info; // TODO(bill): Get directory info
} gbFile;

// TODO(bill): gbAsyncFile

typedef enum gbFileStandardType {
	gbFileStandard_Input,
	gbFileStandard_Output,
	gbFileStandard_Error,

	gbFileStandard_Count,
} gbFileStandardType;

GB_DEF gbFile     *gb_file_get_standard(gbFileStandardType std);

GB_DEF gbFileError gb_file_create        (gbFile *file, char const *filename);
GB_DEF gbFileError gb_file_open          (gbFile *file, char const *filename);
GB_DEF gbFileError gb_file_open_mode     (gbFile *file, gbFileMode mode, char const *filename);
GB_DEF gbFileError gb_file_new           (gbFile *file, gbFileDescriptor fd, gbFileOperations ops, char const *filename);
GB_DEF b32         gb_file_read_at_check (gbFile *file, void *buffer, isize size, i64 offset, isize *bytes_read);
GB_DEF b32         gb_file_write_at_check(gbFile *file, void const *buffer, isize size, i64 offset, isize *bytes_written);
GB_DEF b32         gb_file_read_at       (gbFile *file, void *buffer, isize size, i64 offset);
GB_DEF b32         gb_file_write_at      (gbFile *file, void const *buffer, isize size, i64 offset);
GB_DEF i64         gb_file_seek          (gbFile *file, i64 offset);
GB_DEF i64         gb_file_seek_to_end   (gbFile *file);
GB_DEF i64         gb_file_skip          (gbFile *file, i64 bytes); // NOTE(bill): Skips a certain amount of bytes
GB_DEF i64         gb_file_tell          (gbFile *file);
GB_DEF gbFileError gb_file_close         (gbFile *file);
GB_DEF b32         gb_file_read          (gbFile *file, void *buffer, isize size);
GB_DEF b32         gb_file_write         (gbFile *file, void const *buffer, isize size);
GB_DEF i64         gb_file_size          (gbFile *file);
GB_DEF char const *gb_file_name          (gbFile *file);
GB_DEF gbFileError gb_file_truncate      (gbFile *file, i64 size);
GB_DEF b32         gb_file_has_changed   (gbFile *file); // NOTE(bill): Changed since lasted checked
// TODO(bill):
// gbFileError gb_file_temp(gbFile *file);
//

typedef struct gbFileContents {
	gbAllocator allocator;
	void *      data;
	isize       size;
} gbFileContents;


GB_DEF gbFileContents gb_file_read_contents(gbAllocator a, b32 zero_terminate, char const *filepath);
GB_DEF void           gb_file_free_contents(gbFileContents *fc);


// TODO(bill): Should these have different na,es as they do not take in a gbFile * ???
GB_DEF b32        gb_file_exists         (char const *filepath);
GB_DEF gbFileTime gb_file_last_write_time(char const *filepath);
GB_DEF b32        gb_file_copy           (char const *existing_filename, char const *new_filename, b32 fail_if_exists);
GB_DEF b32        gb_file_move           (char const *existing_filename, char const *new_filename);


#ifndef GB_PATH_SEPARATOR
	#if defined(GB_SYSTEM_WINDOWS)
		#define GB_PATH_SEPARATOR '\\'
	#else
		#define GB_PATH_SEPARATOR '/'
	#endif
#endif

GB_DEF b32         gb_path_is_absolute  (char const *path);
GB_DEF b32         gb_path_is_relative  (char const *path);
GB_DEF b32         gb_path_is_root      (char const *path);
GB_DEF char const *gb_path_base_name    (char const *path);
GB_DEF char const *gb_path_extension    (char const *path);
GB_DEF char *      gb_path_get_full_name(gbAllocator a, char const *path);


////////////////////////////////////////////////////////////////
//
// Printing
//
//

GB_DEF isize gb_printf        (char const *fmt, ...) GB_PRINTF_ARGS(1);
GB_DEF isize gb_printf_va     (char const *fmt, va_list va);
GB_DEF isize gb_printf_err    (char const *fmt, ...) GB_PRINTF_ARGS(1);
GB_DEF isize gb_printf_err_va (char const *fmt, va_list va);
GB_DEF isize gb_fprintf       (gbFile *f, char const *fmt, ...) GB_PRINTF_ARGS(2);
GB_DEF isize gb_fprintf_va    (gbFile *f, char const *fmt, va_list va);

GB_DEF char *gb_bprintf    (char const *fmt, ...) GB_PRINTF_ARGS(1); // NOTE(bill): A locally persisting buffer is used internally
GB_DEF char *gb_bprintf_va (char const *fmt, va_list va);            // NOTE(bill): A locally persisting buffer is used internally
GB_DEF isize gb_snprintf   (char *str, isize n, char const *fmt, ...) GB_PRINTF_ARGS(3);
GB_DEF isize gb_snprintf_va(char *str, isize n, char const *fmt, va_list va);

////////////////////////////////////////////////////////////////
//
// DLL Handling
//
//

typedef void *gbDllHandle;
typedef void (*gbDllProc)(void);

GB_DEF gbDllHandle gb_dll_load        (char const *filepath);
GB_DEF void        gb_dll_unload      (gbDllHandle dll);
GB_DEF gbDllProc   gb_dll_proc_address(gbDllHandle dll, char const *proc_name);


////////////////////////////////////////////////////////////////
//
// Time
//
//

GB_DEF u64  gb_rdtsc       (void);
GB_DEF f64  gb_time_now    (void); // NOTE(bill): This is only for relative time e.g. game loops
GB_DEF u64  gb_utc_time_now(void); // NOTE(bill): Number of microseconds since 1601-01-01 UTC
GB_DEF void gb_sleep_ms    (u32 ms);


////////////////////////////////////////////////////////////////
//
// Miscellany
//
//

typedef struct gbRandom {
	u32 offsets[8];
	u32 value;
} gbRandom;

// NOTE(bill): Generates from numerous sources to produce a decent pseudo-random seed
GB_DEF void  gb_random_init          (gbRandom *r);
GB_DEF u32   gb_random_gen_u32       (gbRandom *r);
GB_DEF u32   gb_random_gen_u32_unique(gbRandom *r);
GB_DEF u64   gb_random_gen_u64       (gbRandom *r); // NOTE(bill): (gb_random_gen_u32() << 32) | gb_random_gen_u32()
GB_DEF isize gb_random_gen_isize     (gbRandom *r);
GB_DEF i64   gb_random_range_i64     (gbRandom *r, i64 lower_inc, i64 higher_inc);
GB_DEF isize gb_random_range_isize   (gbRandom *r, isize lower_inc, isize higher_inc);
GB_DEF f64   gb_random_range_f64     (gbRandom *r, f64 lower_inc, f64 higher_inc);




GB_DEF void gb_exit     (u32 code);
GB_DEF void gb_yield    (void);
GB_DEF void gb_set_env  (char const *name, char const *value);
GB_DEF void gb_unset_env(char const *name);

GB_DEF u16 gb_endian_swap16(u16 i);
GB_DEF u32 gb_endian_swap32(u32 i);
GB_DEF u64 gb_endian_swap64(u64 i);

GB_DEF isize gb_count_set_bits(u64 mask);

////////////////////////////////////////////////////////////////
//
// Platform Stuff
//
//

#if defined(GB_PLATFORM)

// NOTE(bill):
// Coordiate system - +ve x - left to right
//                  - +ve y - bottom to top
//                  - Relative to window

// TODO(bill): Proper documentation for this with code examples

// Window Support - Complete
// OS X Support - Missing:
//     * Sofware framebuffer
//     * (show|hide) window
//     * show_cursor
//     * toggle (fullscreen|borderless)
//     * set window position
//     * Clipboard
//     * GameControllers
// Linux Support - None
// Other OS Support - None

#ifndef GB_MAX_GAME_CONTROLLER_COUNT
#define GB_MAX_GAME_CONTROLLER_COUNT 4
#endif

typedef enum gbKeyType {
	gbKey_Unknown = 0,  // Unhandled key

	// NOTE(bill): Allow the basic printable keys to be aliased with their chars
	gbKey_0 = '0',
	gbKey_1,
	gbKey_2,
	gbKey_3,
	gbKey_4,
	gbKey_5,
	gbKey_6,
	gbKey_7,
	gbKey_8,
	gbKey_9,

	gbKey_A = 'A',
	gbKey_B,
	gbKey_C,
	gbKey_D,
	gbKey_E,
	gbKey_F,
	gbKey_G,
	gbKey_H,
	gbKey_I,
	gbKey_J,
	gbKey_K,
	gbKey_L,
	gbKey_M,
	gbKey_N,
	gbKey_O,
	gbKey_P,
	gbKey_Q,
	gbKey_R,
	gbKey_S,
	gbKey_T,
	gbKey_U,
	gbKey_V,
	gbKey_W,
	gbKey_X,
	gbKey_Y,
	gbKey_Z,

	gbKey_Lbracket  = '[',
	gbKey_Rbracket  = ']',
	gbKey_Semicolon = ';',
	gbKey_Comma     = ',',
	gbKey_Period    = '.',
	gbKey_Quote     = '\'',
	gbKey_Slash     = '/',
	gbKey_Backslash = '\\',
	gbKey_Grave     = '`',
	gbKey_Equals    = '=',
	gbKey_Minus     = '-',
	gbKey_Space     = ' ',

	gbKey__Pad = 128,   // NOTE(bill): make sure ASCII is reserved

	gbKey_Escape,       // Escape
	gbKey_Lcontrol,     // Left Control
	gbKey_Lshift,       // Left Shift
	gbKey_Lalt,         // Left Alt
	gbKey_Lsystem,      // Left OS specific: window (Windows and Linux), apple/cmd (MacOS X), ...
	gbKey_Rcontrol,     // Right Control
	gbKey_Rshift,       // Right Shift
	gbKey_Ralt,         // Right Alt
	gbKey_Rsystem,      // Right OS specific: window (Windows and Linux), apple/cmd (MacOS X), ...
	gbKey_Menu,         // Menu
	gbKey_Return,       // Return
	gbKey_Backspace,    // Backspace
	gbKey_Tab,          // Tabulation
	gbKey_Pageup,       // Page up
	gbKey_Pagedown,     // Page down
	gbKey_End,          // End
	gbKey_Home,         // Home
	gbKey_Insert,       // Insert
	gbKey_Delete,       // Delete
	gbKey_Plus,         // +
	gbKey_Subtract,     // -
	gbKey_Multiply,     // *
	gbKey_Divide,       // /
	gbKey_Left,         // Left arrow
	gbKey_Right,        // Right arrow
	gbKey_Up,           // Up arrow
	gbKey_Down,         // Down arrow
	gbKey_Numpad0,      // Numpad 0
	gbKey_Numpad1,      // Numpad 1
	gbKey_Numpad2,      // Numpad 2
	gbKey_Numpad3,      // Numpad 3
	gbKey_Numpad4,      // Numpad 4
	gbKey_Numpad5,      // Numpad 5
	gbKey_Numpad6,      // Numpad 6
	gbKey_Numpad7,      // Numpad 7
	gbKey_Numpad8,      // Numpad 8
	gbKey_Numpad9,      // Numpad 9
	gbKey_NumpadDot,    // Numpad .
	gbKey_NumpadEnter,  // Numpad Enter
	gbKey_F1,           // F1
	gbKey_F2,           // F2
	gbKey_F3,           // F3
	gbKey_F4,           // F4
	gbKey_F5,           // F5
	gbKey_F6,           // F6
	gbKey_F7,           // F7
	gbKey_F8,           // F8
	gbKey_F9,           // F8
	gbKey_F10,          // F10
	gbKey_F11,          // F11
	gbKey_F12,          // F12
	gbKey_F13,          // F13
	gbKey_F14,          // F14
	gbKey_F15,          // F15
	gbKey_Pause,        // Pause

	gbKey_Count,
} gbKeyType;

/* TODO(bill): Change name? */
typedef u8 gbKeyState;
typedef enum gbKeyStateFlag {
	gbKeyState_Down     = GB_BIT(0),
	gbKeyState_Pressed  = GB_BIT(1),
	gbKeyState_Released = GB_BIT(2)
} gbKeyStateFlag;

GB_DEF void gb_key_state_update(gbKeyState *s, b32 is_down);

typedef enum gbMouseButtonType {
	gbMouseButton_Left,
	gbMouseButton_Middle,
	gbMouseButton_Right,
	gbMouseButton_X1,
	gbMouseButton_X2,

	gbMouseButton_Count
} gbMouseButtonType;

typedef enum gbControllerAxisType {
	gbControllerAxis_LeftX,
	gbControllerAxis_LeftY,
	gbControllerAxis_RightX,
	gbControllerAxis_RightY,
	gbControllerAxis_LeftTrigger,
	gbControllerAxis_RightTrigger,

	gbControllerAxis_Count
} gbControllerAxisType;

typedef enum gbControllerButtonType {
	gbControllerButton_Up,
	gbControllerButton_Down,
	gbControllerButton_Left,
	gbControllerButton_Right,
	gbControllerButton_A,
	gbControllerButton_B,
	gbControllerButton_X,
	gbControllerButton_Y,
	gbControllerButton_LeftShoulder,
	gbControllerButton_RightShoulder,
	gbControllerButton_Back,
	gbControllerButton_Start,
	gbControllerButton_LeftThumb,
	gbControllerButton_RightThumb,

	gbControllerButton_Count
} gbControllerButtonType;

typedef struct gbGameController {
	b16 is_connected, is_analog;

	f32        axes[gbControllerAxis_Count];
	gbKeyState buttons[gbControllerButton_Count];
} gbGameController;

#if defined(GB_SYSTEM_WINDOWS)
	typedef struct _XINPUT_GAMEPAD XINPUT_GAMEPAD;
	typedef struct _XINPUT_STATE   XINPUT_STATE;
	typedef struct _XINPUT_VIBRATION XINPUT_VIBRATION;

	#define GB_XINPUT_GET_STATE(name) unsigned long __stdcall name(unsigned long dwUserIndex, XINPUT_STATE *pState)
	typedef GB_XINPUT_GET_STATE(gbXInputGetStateProc);

	#define GB_XINPUT_SET_STATE(name) unsigned long __stdcall name(unsigned long dwUserIndex, XINPUT_VIBRATION *pVibration)
	typedef GB_XINPUT_SET_STATE(gbXInputSetStateProc);
#endif


typedef enum gbWindowFlag {
	gbWindow_Fullscreen        = GB_BIT(0),
	gbWindow_Hidden            = GB_BIT(1),
	gbWindow_Borderless        = GB_BIT(2),
	gbWindow_Resizable         = GB_BIT(3),
	gbWindow_Minimized         = GB_BIT(4),
	gbWindow_Maximized         = GB_BIT(5),
	gbWindow_FullscreenDesktop = gbWindow_Fullscreen | gbWindow_Borderless,
} gbWindowFlag;

typedef enum gbRendererType {
	gbRenderer_Opengl,
	gbRenderer_Software,

	gbRenderer_Count,
} gbRendererType;



#if defined(GB_SYSTEM_WINDOWS) && !defined(_WINDOWS_)
typedef struct tagBITMAPINFOHEADER {
	unsigned long biSize;
	long          biWidth;
	long          biHeight;
	u16           biPlanes;
	u16           biBitCount;
	unsigned long biCompression;
	unsigned long biSizeImage;
	long          biXPelsPerMeter;
	long          biYPelsPerMeter;
	unsigned long biClrUsed;
	unsigned long biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
typedef struct tagRGBQUAD {
	u8 rgbBlue;
	u8 rgbGreen;
	u8 rgbRed;
	u8 rgbReserved;
} RGBQUAD;
typedef struct tagBITMAPINFO {
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD          bmiColors[1];
} BITMAPINFO, *PBITMAPINFO;
#endif

typedef struct gbPlatform {
	b32 is_initialized;

	void *window_handle;
	i32   window_x, window_y;
	i32   window_width, window_height;
	u32   window_flags;
	b16   window_is_closed, window_has_focus;

#if defined(GB_SYSTEM_WINDOWS)
	void *win32_dc;
#elif defined(GB_SYSTEM_OSX)
	void *osx_autorelease_pool; // TODO(bill): Is this really needed?
#endif

	gbRendererType renderer_type;
	union {
		struct {
			void *      context;
			i32         major;
			i32         minor;
			b16         core, compatible;
			gbDllHandle dll_handle;
		} opengl;

		// NOTE(bill): Software rendering
		struct {
#if defined(GB_SYSTEM_WINDOWS)
			BITMAPINFO win32_bmi;
#endif
			void *     memory;
			isize      memory_size;
			i32        pitch;
			i32        bits_per_pixel;
		} sw_framebuffer;
	};

	gbKeyState keys[gbKey_Count];
	struct {
		gbKeyState control;
		gbKeyState alt;
		gbKeyState shift;
	} key_modifiers;

	Rune  char_buffer[256];
	isize char_buffer_count;

	b32 mouse_clip;
	i32 mouse_x, mouse_y;
	i32 mouse_dx, mouse_dy; // NOTE(bill): Not raw mouse movement
	i32 mouse_raw_dx, mouse_raw_dy; // NOTE(bill): Raw mouse movement
	f32 mouse_wheel_delta;
	gbKeyState mouse_buttons[gbMouseButton_Count];

	gbGameController game_controllers[GB_MAX_GAME_CONTROLLER_COUNT];

	f64              curr_time;
	f64              dt_for_frame;
	b32              quit_requested;

#if defined(GB_SYSTEM_WINDOWS)
	struct {
		gbXInputGetStateProc *get_state;
		gbXInputSetStateProc *set_state;
	} xinput;
#endif
} gbPlatform;


typedef struct gbVideoMode {
	i32 width, height;
	i32 bits_per_pixel;
} gbVideoMode;

GB_DEF gbVideoMode gb_video_mode                     (i32 width, i32 height, i32 bits_per_pixel);
GB_DEF b32         gb_video_mode_is_valid            (gbVideoMode mode);
GB_DEF gbVideoMode gb_video_mode_get_desktop         (void);
GB_DEF isize       gb_video_mode_get_fullscreen_modes(gbVideoMode *modes, isize max_mode_count); // NOTE(bill): returns mode count
GB_DEF GB_COMPARE_PROC(gb_video_mode_cmp);     // NOTE(bill): Sort smallest to largest (Ascending)
GB_DEF GB_COMPARE_PROC(gb_video_mode_dsc_cmp); // NOTE(bill): Sort largest to smallest (Descending)


// NOTE(bill): Software rendering
GB_DEF b32   gb_platform_init_with_software         (gbPlatform *p, char const *window_title, i32 width, i32 height, u32 window_flags);
// NOTE(bill): OpenGL Rendering
GB_DEF b32   gb_platform_init_with_opengl           (gbPlatform *p, char const *window_title, i32 width, i32 height, u32 window_flags, i32 major, i32 minor, b32 core, b32 compatible);
GB_DEF void  gb_platform_update                     (gbPlatform *p);
GB_DEF void  gb_platform_display                    (gbPlatform *p);
GB_DEF void  gb_platform_destroy                    (gbPlatform *p);
GB_DEF void  gb_platform_show_cursor                (gbPlatform *p, b32 show);
GB_DEF void  gb_platform_set_mouse_position         (gbPlatform *p, i32 x, i32 y);
GB_DEF void  gb_platform_set_controller_vibration   (gbPlatform *p, isize index, f32 left_motor, f32 right_motor);
GB_DEF b32   gb_platform_has_clipboard_text         (gbPlatform *p);
GB_DEF void  gb_platform_set_clipboard_text         (gbPlatform *p, char const *str);
GB_DEF char *gb_platform_get_clipboard_text         (gbPlatform *p, gbAllocator a);
GB_DEF void  gb_platform_set_window_position        (gbPlatform *p, i32 x, i32 y);
GB_DEF void  gb_platform_set_window_title           (gbPlatform *p, char const *title, ...) GB_PRINTF_ARGS(2);
GB_DEF void  gb_platform_toggle_fullscreen          (gbPlatform *p, b32 fullscreen_desktop);
GB_DEF void  gb_platform_toggle_borderless          (gbPlatform *p);
GB_DEF void  gb_platform_make_opengl_context_current(gbPlatform *p);
GB_DEF void  gb_platform_show_window                (gbPlatform *p);
GB_DEF void  gb_platform_hide_window                (gbPlatform *p);


#endif // GB_PLATFORM

#if defined(__cplusplus)
}
#endif

#endif // GB_INCLUDE_GB_H
