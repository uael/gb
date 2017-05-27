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

#ifndef  GB_SYSTEM_H__
# define GB_SYSTEM_H__

#ifndef GB_COMPILE
# define GB_COMPILE 0
#endif

#if defined(_WIN32)
# define GB_EXPORT_LINK __declspec(dllexport)
# define GB_IMPORT_LINK __declspec(dllimport)
#else
# define GB_EXPORT_LINK __attribute__((visibility("default")))
# define GB_IMPORT_LINK
#endif
#if GB_COMPILE
# ifdef __cplusplus
#   define GB_EXTERN extern "C" GB_IMPORT_LINK
#   define GB_API extern "C" GB_EXPORT_LINK
# else
#   define GB_EXTERN extern GB_IMPORT_LINK
#   define GB_API extern GB_EXPORT_LINK
# endif
#else
# ifdef __cplusplus
#   define GB_EXTERN extern "C" GB_IMPORT_LINK
#   define GB_API extern "C" GB_IMPORT_LINK
# else
#   define GB_EXTERN extern GB_IMPORT_LINK
#   define GB_API extern GB_IMPORT_LINK
# endif
#endif

#define GB_SYSTEM_ANDROID 0
#define GB_SYSTEM_BSD 0
#define GB_SYSTEM_IOS 0
#define GB_SYSTEM_IOS_SIMULATOR 0
#define GB_SYSTEM_LINUX 0
#define GB_SYSTEM_LINUX_RASPBERRYPI 0
#define GB_SYSTEM_MACOSX 0
#define GB_SYSTEM_WINDOWS 0
#define GB_SYSTEM_PNACL 0
#define GB_SYSTEM_TIZEN 0

#define GB_SYSTEM_APPLE 0
#define GB_SYSTEM_POSIX 0
#define GB_SYSTEM_MOBILE 0
#define GB_SYSTEM_DESKTOP 0
#define GB_SYSTEM_CONSOLE 0

#define GB_ARCH_ARM 0
#define GB_ARCH_ARM5 0
#define GB_ARCH_ARM6 0
#define GB_ARCH_ARM7 0
#define GB_ARCH_ARM8 0
#define GB_ARCH_ARM_64 0
#define GB_ARCH_ARM8_64 0
#define GB_ARCH_X86 0
#define GB_ARCH_X86_64 0
#define GB_ARCH_PPC 0
#define GB_ARCH_PPC_64 0
#define GB_ARCH_IA64 0
#define GB_ARCH_MIPS 0
#define GB_ARCH_MIPS_64 0
#define GB_ARCH_GENERIC 0
#define GB_ARCH_SSE2 0
#define GB_ARCH_SSE3 0
#define GB_ARCH_SSE4 0
#define GB_ARCH_SSE4_FMA3 0
#define GB_ARCH_NEON 0
#define GB_ARCH_THUMB 0
#define GB_ARCH_ENDIAN_LITTLE 0
#define GB_ARCH_ENDIAN_BIG 0

#define GB_CPU_X86 0
#define GB_ARCH_64 0
#define GB_ARCH_32 0

#if defined(__pnacl__)
# undef  GB_SYSTEM_PNACL
# define GB_SYSTEM_PNACL 1
# undef  GB_ARCH_GENERIC
# define GB_ARCH_GENERIC 1
# undef  GB_ARCH_ENDIAN_LITTLE
# define GB_ARCH_ENDIAN_LITTLE 1
#elif defined(__ANDROID__)
# undef  GB_SYSTEM_ANDROID
# define GB_SYSTEM_ANDROID 1
# undef  GB_SYSTEM_POSIX
# define GB_SYSTEM_POSIX 1
# if defined(__arm__)
#   undef  GB_ARCH_ARM
#   define GB_ARCH_ARM 1
#   ifdef __ARM_ARCH_7A__
#     undef  GB_ARCH_ARM7
#     define GB_ARCH_ARM7 1
#   elif defined(__ARM_ARCH_5TE__)
#     undef  GB_ARCH_ARM5
#     define GB_ARCH_ARM5 1
#   else
#     error Unsupported ARM architecture
#   endif
# elif defined(__aarch64__)
#   undef  GB_ARCH_ARM
#   define GB_ARCH_ARM 1
#   undef  GB_ARCH_ARM_64
#   define GB_ARCH_ARM_64 1
#   undef  GB_ARCH_ARM8_64
#   define GB_ARCH_ARM8_64 1
# elif defined(__i386__)
#   undef  GB_ARCH_X86
#   define GB_ARCH_X86 1
# elif defined(__x86_64__)
#   undef  GB_ARCH_X86_64
#   define GB_ARCH_X86_64 1
# elif defined(__mips__) && defined(__mips64)
#   undef  GB_ARCH_MIPS
#   define GB_ARCH_MIPS 1
#   undef  GB_ARCH_MIPS_64
#   define GB_ARCH_MIPS_64 1
#   ifndef _MIPS_ISA
#     define _MIPS_ISA 7
#   endif
# elif defined(__mips__)
#   undef  GB_ARCH_MIPS
#   define GB_ARCH_MIPS 1
#   ifndef _MIPS_ISA
#     define _MIPS_ISA 6
#   endif
# else
#   error Unknown architecture
# endif
# if GB_ARCH_MIPS
#   if defined(__MIPSEL__) || defined(__MIPSEL) || defined(_MIPSEL)
#     undef  GB_ARCH_ENDIAN_LITTLE
#     define GB_ARCH_ENDIAN_LITTLE 1
#   else
#     undef  GB_ARCH_ENDIAN_BIG
#     define GB_ARCH_ENDIAN_BIG 1
#   endif
# elif defined(__AARCH64EB__) || defined(__ARMEB__)
#   undef  GB_ARCH_ENDIAN_BIG
#   define GB_ARCH_ENDIAN_BIG 1
# else
#   undef  GB_ARCH_ENDIAN_LITTLE
#   define GB_ARCH_ENDIAN_LITTLE 1
# endif
# undef  GB_SYSTEM_MOBILE
# define GB_SYSTEM_MOBILE 1
# undef  GB_SYSTEM_CONSOLE
# define GB_SYSTEM_CONSOLE 1
#elif defined(__TIZEN__)
# undef  GB_SYSTEM_TIZEN
# define GB_SYSTEM_TIZEN 1
# undef  GB_SYSTEM_POSIX
# define GB_SYSTEM_POSIX 1
# define GB_SYSTEM_NAME "Tizen"
# if defined(__arm__)
#   undef  GB_ARCH_ARM
#   define GB_ARCH_ARM 1
#   ifdef __ARM_ARCH_7A__
#     undef  GB_ARCH_ARM7
#     define GB_ARCH_ARM7 1
#   elif defined(__ARM_ARCH_5TE__)
#     undef  GB_ARCH_ARM5
#     define GB_ARCH_ARM5 1
#   else
#     error Unsupported ARM architecture
#   endif
# elif defined(__i386__)
#   undef  GB_ARCH_X86
#   define GB_ARCH_X86 1
# elif defined(__x86_64__)
#   undef  GB_ARCH_X86_64
#   define GB_ARCH_X86_64 1
# else
#   error Unknown architecture
# endif
# if defined(__AARCH64EB__) || defined(__ARMEB__)
#   undef  GB_ARCH_ENDIAN_BIG
#   define GB_ARCH_ENDIAN_BIG 1
# else
#   undef  GB_ARCH_ENDIAN_LITTLE
#   define GB_ARCH_ENDIAN_LITTLE 1
# endif
# undef  GB_SYSTEM_MOBILE
# define GB_SYSTEM_MOBILE 1
# undef  GB_SYSTEM_CONSOLE
# define GB_SYSTEM_CONSOLE 1
#elif defined(__APPLE__) && __APPLE__
# undef  GB_SYSTEM_APPLE
# define GB_SYSTEM_APPLE 1
# undef  GB_SYSTEM_POSIX
# define GB_SYSTEM_POSIX 1
# include <TargetConditionals.h>
# if defined(__IPHONE__) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) || (defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR)
#   undef  GB_SYSTEM_IOS
#   define GB_SYSTEM_IOS 1
#   if defined(__arm__)
#     undef  GB_ARCH_ARM
#     define GB_ARCH_ARM 1
#     if defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7S__)
#       undef  GB_ARCH_ARM7
#       define GB_ARCH_ARM7 1
#       ifndef __ARM_NEON__
#         error Missing ARM NEON support
#       endif
#     elif defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6__)
#       undef  GB_ARCH_ARM6
#       define GB_ARCH_ARM6 1
#     else
#       error Unrecognized ARM architecture
#     endif
#   elif defined(__arm64__)
#     undef  GB_ARCH_ARM
#     define GB_ARCH_ARM 1
#     undef  GB_ARCH_ARM_64
#     define GB_ARCH_ARM_64 1
#     if defined(__ARM64_ARCH_8__)
#       undef  GB_ARCH_ARM8_64
#       define GB_ARCH_ARM8_64 1
#     else
#       error Unrecognized ARM architecture
#     endif
#   elif defined(__i386__)
#     undef  GB_SYSTEM_IOS_SIMULATOR
#     define GB_SYSTEM_IOS_SIMULATOR 1
#     undef  GB_ARCH_X86
#     define GB_ARCH_X86 1
#   elif defined(__x86_64__)
#     undef  GB_SYSTEM_IOS_SIMULATOR
#     define GB_SYSTEM_IOS_SIMULATOR 1
#     undef  GB_ARCH_X86_64
#     define GB_ARCH_X86_64 1
#   else
#     error Unknown architecture
#   endif
#   undef  GB_ARCH_ENDIAN_LITTLE
#   define GB_ARCH_ENDIAN_LITTLE 1
#   undef  GB_SYSTEM_MOBILE
#   define GB_SYSTEM_MOBILE 1
#   undef  GB_SYSTEM_CONSOLE
#   define GB_SYSTEM_CONSOLE 1
# elif defined(__MACH__)
#   undef  GB_SYSTEM_MACOSX
#   define GB_SYSTEM_MACOSX 1
#   if defined(__x86_64__) ||  defined(__x86_64) || defined(__amd64)
#     undef  GB_ARCH_X86_64
#     define GB_ARCH_X86_64 1
#     undef  GB_ARCH_ENDIAN_LITTLE
#     define GB_ARCH_ENDIAN_LITTLE 1
#   elif defined(__i386__) || defined(__intel__)
#     undef  GB_ARCH_X86
#     define GB_ARCH_X86 1
#     undef  GB_ARCH_ENDIAN_LITTLE
#     define GB_ARCH_ENDIAN_LITTLE 1
#   elif defined(__powerpc64__) || defined(__POWERPC64__)
#     undef  GB_ARCH_PPC_64
#     define GB_ARCH_PPC_64 1
#     undef  GB_ARCH_ENDIAN_BIG
#     define GB_ARCH_ENDIAN_BIG 1
#   elif defined(__powerpc__) || defined(__POWERPC__)
#     undef  GB_ARCH_PPC
#     define GB_ARCH_PPC 1
#     undef  GB_ARCH_ENDIAN_BIG
#     define GB_ARCH_ENDIAN_BIG 1
#   else
#     error Unknown architecture
#   endif
#   undef  GB_SYSTEM_DESKTOP
#   define GB_SYSTEM_DESKTOP 1
# else
#   error Unknown Apple Platform
# endif
#elif defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
# undef  GB_SYSTEM_WINDOWS
# define GB_SYSTEM_WINDOWS 1
# define GB_SYSTEM_NAME "Windows"
# if defined(__x86_64__) || defined(_M_AMD64) || defined(_AMD64_)
#   undef  GB_ARCH_X86_64
#   define GB_ARCH_X86_64 1
# elif defined(__x86__) || defined(_M_IX86) || defined(_X86_)
#   undef  GB_ARCH_X86
#   define GB_ARCH_X86 1
# elif defined(__ia64__) || defined(_M_IA64) || defined(_IA64_)
#   undef  GB_ARCH_IA64
#   define GB_ARCH_IA64 1
# elif defined(_M_ARM)
#   undef  GB_ARCH_ARM
#   define GB_ARCH_ARM 1
#   if _M_ARM == 5
#     undef  GB_ARCH_ARM5
#     define GB_ARCH_ARM5 1
#   elif _M_ARM == 6
#     undef  GB_ARCH_ARM6
#     define GB_ARCH_ARM6 1
#   elif _M_ARM == 7
#     undef  GB_ARCH_ARM7
#     define GB_ARCH_ARM7 1
#   else
#     error Unknown ARM architecture
#   endif
# else
#   error Unknown architecture
# endif
# undef  GB_ARCH_ENDIAN_LITTLE
# define GB_ARCH_ENDIAN_LITTLE 1
# undef  GB_SYSTEM_DESKTOP
# define GB_SYSTEM_DESKTOP 1
#elif defined(__BSD__) || defined(__FreeBSD__)
# undef  GB_SYSTEM_BSD
# define GB_SYSTEM_BSD 1
# undef  GB_SYSTEM_POSIX
# define GB_SYSTEM_POSIX 1
# if defined(__x86_64__) || defined(__x86_64) || defined(__amd64)
#   undef  GB_ARCH_X86_64
#   define GB_ARCH_X86_64 1
#   undef  GB_ARCH_ENDIAN_LITTLE
#   define GB_ARCH_ENDIAN_LITTLE 1
# elif defined(__i386__) || defined(__intel__) || defined(_M_IX86)
#   undef  GB_ARCH_X86
#   define GB_ARCH_X86 1
#   undef  GB_ARCH_ENDIAN_LITTLE
#   define GB_ARCH_ENDIAN_LITTLE 1
# elif defined(__powerpc64__) || defined(__POWERPC64__)
#   undef  GB_ARCH_PPC_64
#   define GB_ARCH_PPC_64 1
#   undef  GB_ARCH_ENDIAN_BIG
#   define GB_ARCH_ENDIAN_BIG 1
# elif defined(__powerpc__) || defined(__POWERPC__)
#   undef  GB_ARCH_PPC
#   define GB_ARCH_PPC 1
#   undef  GB_ARCH_ENDIAN_BIG
#   define GB_ARCH_ENDIAN_BIG 1
# else
#   error Unknown architecture
# endif
# undef  GB_SYSTEM_DESKTOP
# define GB_SYSTEM_DESKTOP 1
#elif defined(__linux__) || defined(__linux)
# undef  GB_SYSTEM_LINUX
# define GB_SYSTEM_LINUX 1
# undef  GB_SYSTEM_POSIX
# define GB_SYSTEM_POSIX 1
# if defined(__x86_64__) || defined(__x86_64) || defined(__amd64)
#   undef  GB_ARCH_X86_64
#   define GB_ARCH_X86_64 1
#   undef  GB_ARCH_ENDIAN_LITTLE
#   define GB_ARCH_ENDIAN_LITTLE 1
# elif defined(__i386__) || defined(__intel__) || defined(_M_IX86)
#   undef  GB_ARCH_X86
#   define GB_ARCH_X86 1
#   undef  GB_ARCH_ENDIAN_LITTLE
#   define GB_ARCH_ENDIAN_LITTLE 1
# elif defined(__powerpc64__) || defined(__POWERPC64__)
#   undef  GB_ARCH_PPC_64
#   define GB_ARCH_PPC_64 1
#   undef  GB_ARCH_ENDIAN_BIG
#   define GB_ARCH_ENDIAN_BIG 1
# elif defined(__powerpc__) || defined(__POWERPC__)
#   undef  GB_ARCH_PPC
#   define GB_ARCH_PPC 1
#   undef  GB_ARCH_ENDIAN_BIG
#   define GB_ARCH_ENDIAN_BIG 1
# elif defined(__arm__)
#   undef  GB_ARCH_ARM
#   define GB_ARCH_ARM 1
#   if defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7S__)
#     undef  GB_ARCH_ARM7
#     define GB_ARCH_ARM7 1
#     ifndef __ARM_NEON__
#       error Missing ARM NEON support
#     endif
#   elif defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6ZK__)
#     undef  GB_ARCH_ARM6
#     define GB_ARCH_ARM6 1
#   else
#     error Unrecognized ARM architecture
#   endif
#   if defined(__ARMEB__)
#     undef  GB_ARCH_ENDIAN_BIG
#     define GB_ARCH_ENDIAN_BIG 1
#   else
#     undef  GB_ARCH_ENDIAN_LITTLE
#     define GB_ARCH_ENDIAN_LITTLE 1
#   endif
# elif defined(__arm64__) || defined(__aarch64__)
#   undef  GB_ARCH_ARM
#   define GB_ARCH_ARM 1
#   undef  GB_ARCH_ARM_64
#   define GB_ARCH_ARM_64 1
#   undef  GB_ARCH_ENDIAN_LITTLE
#   define GB_ARCH_ENDIAN_LITTLE 1
#   if defined(__ARM64_ARCH_8__)
#     undef  GB_ARCH_ARM8_64
#     define GB_ARCH_ARM8_64 1
#   else
#     error Unrecognized ARM architecture
#   endif
#   if defined(__AARCH64EB__)
#     undef  GB_ARCH_ENDIAN_BIG
#     define GB_ARCH_ENDIAN_BIG 1
#   else
#     undef  GB_ARCH_ENDIAN_LITTLE
#     define GB_ARCH_ENDIAN_LITTLE 1
#   endif
# else
#   error Unknown architecture
# endif
# if defined(__raspberrypi__)
#   undef  GB_SYSTEM_LINUX_RASPBERRYPI
#   define GB_SYSTEM_LINUX_RASPBERRYPI 1
# endif
# undef  GB_SYSTEM_DESKTOP
# define GB_SYSTEM_DESKTOP 1
#else
# error Unknown platform
#endif

#if defined(__SSE2__) || GB_ARCH_X86_64
# undef  GB_ARCH_SSE2
# define GB_ARCH_SSE2 1
#endif
#ifdef __SSE3__
# undef  GB_ARCH_SSE3
# define GB_ARCH_SSE3 1
#endif
#ifdef __SSE4_1__
# undef  GB_ARCH_SSE4
# define GB_ARCH_SSE4 1
#endif
#ifdef __ARM_NEON__
# undef  GB_ARCH_NEON
# define GB_ARCH_NEON 1
#endif
#ifdef __thumb__
# undef  GB_ARCH_THUMB
# define GB_ARCH_THUMB 1
#endif

#if GB_ARCH_ARM_64 || GB_ARCH_X86_64 || GB_ARCH_PPC_64 || GB_ARCH_IA64 || GB_ARCH_MIPS_64
# undef  GB_ARCH_64
# define GB_ARCH_64 1
#else
# undef  GB_ARCH_32
# define GB_ARCH_32 1
#endif

#if GB_ARCH_X86 || GB_ARCH_X86_64
#undef  GB_CPU_X86
#define GB_CPU_X86 1
#endif

#endif /* GB_SYSTEM_H__ */
