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

GB_DEF gbVirtualMemory gb_vm_alloc(void *addr, isize size);

GB_DEF b32 gb_vm_free(gbVirtualMemory vm);

GB_DEF gbVirtualMemory gb_vm_trim(gbVirtualMemory vm, isize lead_size, isize size);

GB_DEF b32 gb_vm_purge(gbVirtualMemory vm);

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
  void *data;
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

GB_DEF void *gb_alloc_align(gbAllocator a, isize size, isize alignment);

GB_DEF void *gb_alloc(gbAllocator a, isize size);

GB_DEF void gb_free(gbAllocator a, void *ptr);

GB_DEF void gb_free_all(gbAllocator a);

GB_DEF void *gb_resize(gbAllocator a, void *ptr, isize old_size, isize new_size);

GB_DEF void *gb_resize_align(gbAllocator a, void *ptr, isize old_size, isize new_size, isize alignment);
// TODO(bill): For gb_resize, should the use need to pass the old_size or only the new_size?

GB_DEF void *gb_alloc_copy(gbAllocator a, void const *src, isize size);

GB_DEF void *gb_alloc_copy_align(gbAllocator a, void const *src, isize size, isize alignment);

GB_DEF char *gb_alloc_str(gbAllocator a, char const *str);

GB_DEF char *gb_alloc_str_len(gbAllocator a, char const *str, isize len);


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
  void *physical_start;
  isize total_size;
  isize total_allocated;
  isize temp_count;
} gbArena;

GB_DEF void gb_arena_init_from_memory(gbArena *arena, void *start, isize size);

GB_DEF void gb_arena_init_from_allocator(gbArena *arena, gbAllocator backing, isize size);

GB_DEF void gb_arena_init_sub(gbArena *arena, gbArena *parent_arena, isize size);

GB_DEF void gb_arena_free(gbArena *arena);

GB_DEF isize gb_arena_alignment_of(gbArena *arena, isize alignment);

GB_DEF isize gb_arena_size_remaining(gbArena *arena, isize alignment);

GB_DEF void gb_arena_check(gbArena *arena);


// Allocation Types: alloc, free_all, resize
GB_DEF gbAllocator gb_arena_allocator(gbArena *arena);

GB_DEF GB_ALLOCATOR_PROC(gb_arena_allocator_proc);

typedef struct gbTempArenaMemory {
  gbArena *arena;
  isize original_count;
} gbTempArenaMemory;

GB_DEF gbTempArenaMemory gb_temp_arena_memory_begin(gbArena *arena);

GB_DEF void gb_temp_arena_memory_end(gbTempArenaMemory tmp_mem);







//
// Pool Allocator
//


typedef struct gbPool {
  gbAllocator backing;
  void *physical_start;
  void *free_list;
  isize block_size;
  isize block_align;
  isize total_size;
} gbPool;

GB_DEF void gb_pool_init(gbPool *pool, gbAllocator backing, isize num_blocks, isize block_size);

GB_DEF void
gb_pool_init_align(gbPool *pool, gbAllocator backing, isize num_blocks, isize block_size, isize block_align);

GB_DEF void gb_pool_free(gbPool *pool);

// Allocation Types: alloc, free
GB_DEF gbAllocator gb_pool_allocator(gbPool *pool);

GB_DEF GB_ALLOCATOR_PROC(gb_pool_allocator_proc);

// NOTE(bill): Used for allocators to keep track of sizes
typedef struct gbAllocationHeader {
  isize size;
} gbAllocationHeader;

GB_DEF gbAllocationHeader *gb_allocation_header(void *data);

GB_DEF void gb_allocation_header_fill(gbAllocationHeader *header, void *data, isize size);

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
  isize size;
};

typedef struct gbFreeList {
  void *physical_start;
  isize total_size;

  gbFreeListBlock *curr_block;

  isize total_allocated;
  isize allocation_count;
} gbFreeList;

GB_DEF void gb_free_list_init(gbFreeList *fl, void *start, isize size);

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

GB_DEF void gb_scratch_memory_init(gbScratchMemory *s, void *start, isize size);

GB_DEF b32 gb_scratch_memory_is_in_use(gbScratchMemory *s, void *ptr);


// Allocation Types: alloc, free, free_all, resize
GB_DEF gbAllocator gb_scratch_allocator(gbScratchMemory *s);

GB_DEF GB_ALLOCATOR_PROC(gb_scratch_allocator_proc);

// TODO(bill): Stack allocator
// TODO(bill): Fixed heap allocator
// TODO(bill): General heap allocator. Maybe a TCMalloc like clone?

#endif /* GB_ALLOC_H__ */
