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

#ifndef  GB_ASSERT_H__
# define GB_ASSERT_H__

#include "gb/macros.h"

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

#endif /* GB_ASSERT_H__ */
