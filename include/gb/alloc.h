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

#ifndef  GB_ALLOC_H__
# define GB_ALLOC_H__

#include "gb/affinity.h"

#define GB_ALLOCATOR_PROC(name) \
  void *name(void *allocator_data, enum gb_allocation_type type, \
    ssize_t size, ssize_t alignment, \
    void *old_memory, ssize_t old_size, \
    uint64_t flags)

// TODO(bill): Is this a decent default alignment?
#ifndef GB_DEFAULT_MEMORY_ALIGNMENT
# define GB_DEFAULT_MEMORY_ALIGNMENT (2 * gb_size_of(void *))
#endif

#ifndef GB_DEFAULT_ALLOCATOR_FLAGS
# define GB_DEFAULT_ALLOCATOR_FLAGS (gbAllocatorFlag_ClearToZero)
#endif

typedef struct gb_virtual_memory gb_virtual_memory_t;
typedef struct gb_allocator gb_allocator_t;
typedef struct gb_arena gb_arena_t;
typedef struct gb_temp_arena_memory gb_temp_arena_memory_t;
typedef struct gb_pool gb_pool_t;
typedef struct gb_allocation_header gb_allocation_header_t;
typedef struct gb_free_list_block gb_free_list_block_t;
typedef struct gb_free_list gb_free_list_t;
typedef struct gb_scratch_memory gb_scratch_memory_t;

struct gb_virtual_memory {
  void *data;
  ssize_t size;
};

GB_DEF gb_virtual_memory_t gb_virtual_memory(void *data, ssize_t size);
GB_DEF gb_virtual_memory_t gb_vm_alloc(void *addr, ssize_t size);
GB_DEF byte32_t gb_vm_free(gb_virtual_memory_t vm);
GB_DEF gb_virtual_memory_t gb_vm_trim(gb_virtual_memory_t vm, ssize_t lead_size, ssize_t size);
GB_DEF byte32_t gb_vm_purge(gb_virtual_memory_t vm);
GB_DEF ssize_t gb_virtual_memory_page_size(ssize_t *alignment_out);

enum gb_allocation_type {
  gbAllocation_Alloc,
  gbAllocation_Free,
  gbAllocation_FreeAll,
  gbAllocation_Resize,
};

typedef GB_ALLOCATOR_PROC(gbAllocatorProc);

struct gb_allocator {
  gbAllocatorProc *proc;
  void *data;
};

enum gb_allocator_flag {
  gbAllocatorFlag_ClearToZero = GB_BIT(0),
};

GB_DEF void *gb_alloc_align(gb_allocator_t a, ssize_t size, ssize_t alignment);
GB_DEF void *gb_alloc(gb_allocator_t a, ssize_t size);
GB_DEF void gb_free(gb_allocator_t a, void *ptr);
GB_DEF void gb_free_all(gb_allocator_t a);
GB_DEF void *gb_resize(gb_allocator_t a, void *ptr, ssize_t old_size, ssize_t new_size);
GB_DEF void *gb_resize_align(gb_allocator_t a, void *ptr, ssize_t old_size, ssize_t new_size, ssize_t alignment);
GB_DEF void *gb_alloc_copy(gb_allocator_t a, void const *src, ssize_t size);
GB_DEF void *gb_alloc_copy_align(gb_allocator_t a, void const *src, ssize_t size, ssize_t alignment);
GB_DEF char *gb_alloc_str(gb_allocator_t a, char const *str);
GB_DEF char *gb_alloc_str_len(gb_allocator_t a, char const *str, ssize_t len);
GB_DEF void *gb_default_resize_align(gb_allocator_t a, void *ptr, ssize_t old_size, ssize_t new_size, ssize_t alignment);

// TODO(bill): Probably use a custom heap allocator system that doesn't depend on malloc/free
// Base it off TCMalloc or something else? Or something entirely custom?
GB_DEF gb_allocator_t gb_heap_allocator(void);
GB_DEF GB_ALLOCATOR_PROC(gb_heap_allocator_proc);

#ifndef gb_alloc_item
# define gb_alloc_item(allocator_, Type)         (Type *)gb_alloc(allocator_, gb_size_of(Type))
# define gb_alloc_array(allocator_, Type, count) (Type *)gb_alloc(allocator_, gb_size_of(Type) * (count))
#endif

#ifndef gb_malloc
# define gb_malloc(sz) gb_alloc(gb_heap_allocator(), sz)
# define gb_mfree(ptr) gb_free(gb_heap_allocator(), ptr)
#endif

struct gb_arena {
  gb_allocator_t backing;
  void *physical_start;
  ssize_t total_size;
  ssize_t total_allocated;
  ssize_t temp_count;
};

GB_DEF void gb_arena_init_from_memory(gb_arena_t *arena, void *start, ssize_t size);
GB_DEF void gb_arena_init_from_allocator(gb_arena_t *arena, gb_allocator_t backing, ssize_t size);
GB_DEF void gb_arena_init_sub(gb_arena_t *arena, gb_arena_t *parent_arena, ssize_t size);
GB_DEF void gb_arena_free(gb_arena_t *arena);
GB_DEF ssize_t gb_arena_alignment_of(gb_arena_t *arena, ssize_t alignment);
GB_DEF ssize_t gb_arena_size_remaining(gb_arena_t *arena, ssize_t alignment);
GB_DEF void gb_arena_check(gb_arena_t *arena);

// Allocation Types: alloc, free_all, resize
GB_DEF gb_allocator_t gb_arena_allocator(gb_arena_t *arena);
GB_DEF GB_ALLOCATOR_PROC(gb_arena_allocator_proc);

struct gb_temp_arena_memory {
  gb_arena_t *arena;
  ssize_t original_count;
};

GB_DEF gb_temp_arena_memory_t gb_temp_arena_memory_begin(gb_arena_t *arena);
GB_DEF void gb_temp_arena_memory_end(gb_temp_arena_memory_t tmp_mem);

struct gb_pool {
  gb_allocator_t backing;
  void *physical_start;
  void *free_list;
  ssize_t block_size;
  ssize_t block_align;
  ssize_t total_size;
};

GB_DEF void gb_pool_init(gb_pool_t *pool, gb_allocator_t backing, ssize_t num_blocks, ssize_t block_size);
GB_DEF void gb_pool_init_align(gb_pool_t *pool, gb_allocator_t backing, ssize_t num_blocks, ssize_t block_size, ssize_t block_align);
GB_DEF void gb_pool_free(gb_pool_t *pool);

// Allocation Types: alloc, free
GB_DEF gb_allocator_t gb_pool_allocator(gb_pool_t *pool);
GB_DEF GB_ALLOCATOR_PROC(gb_pool_allocator_proc);

// NOTE(bill): Used for allocators to keep track of sizes
struct gb_allocation_header {
  ssize_t size;
};

GB_DEF gb_allocation_header_t *gb_allocation_header(void *data);
GB_DEF void gb_allocation_header_fill(gb_allocation_header_t *header, void *data, ssize_t size);

// TODO(bill): Find better way of doing this without #if #elif etc.
#if defined(GB_ARCH_32_BIT)
# define GB_ISIZE_HIGH_BIT 0x80000000
#elif defined(GB_ARCH_64_BIT)
# define GB_ISIZE_HIGH_BIT 0x8000000000000000ll
#else
# error
#endif

// IMPORTANT TODO(bill): Thoroughly test the free list allocator!
// NOTE(bill): This is a very shitty free list as it just picks the first free block not the best size
// as I am just being lazy. Also, I will probably remove it later; it's only here because why not?!
//
// NOTE(bill): I may also complete remove this if I completely implement a fixed heap allocator
struct gb_free_list_block {
  gb_free_list_block_t *next;
  ssize_t size;
};

struct gb_free_list {
  void *physical_start;
  ssize_t total_size;
  gb_free_list_block_t *curr_block;
  ssize_t total_allocated;
  ssize_t allocation_count;
};

GB_DEF void gb_free_list_init(gb_free_list_t *fl, void *start, ssize_t size);
GB_DEF void gb_free_list_init_from_allocator(gb_free_list_t *fl, gb_allocator_t backing, ssize_t size);

// Allocation Types: alloc, free, free_all, resize
GB_DEF gb_allocator_t gb_free_list_allocator(gb_free_list_t *fl);
GB_DEF GB_ALLOCATOR_PROC(gb_free_list_allocator_proc);

struct gb_scratch_memory {
  void *physical_start;
  ssize_t total_size;
  void *alloc_point;
  void *free_point;
};

GB_DEF void gb_scratch_memory_init(gb_scratch_memory_t *s, void *start, ssize_t size);
GB_DEF byte32_t gb_scratch_memory_is_in_use(gb_scratch_memory_t *s, void *ptr);

// Allocation Types: alloc, free, free_all, resize
GB_DEF gb_allocator_t gb_scratch_allocator(gb_scratch_memory_t *s);
GB_DEF GB_ALLOCATOR_PROC(gb_scratch_allocator_proc);

// TODO(bill): Stack allocator
// TODO(bill): Fixed heap allocator
// TODO(bill): General heap allocator. Maybe a TCMalloc like clone?

#endif /* GB_ALLOC_H__ */
