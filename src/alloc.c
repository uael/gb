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

#include "gb/alloc.h"
#include "gb/string.h"
#include "gb/io.h"

gb_inline void *gb_alloc_align(gbAllocator a, isize size, isize alignment) {
  return a.proc(a.data, gbAllocation_Alloc, size, alignment, NULL, 0, GB_DEFAULT_ALLOCATOR_FLAGS);
}

gb_inline void *gb_alloc(gbAllocator a, isize size) { return gb_alloc_align(a, size, GB_DEFAULT_MEMORY_ALIGNMENT); }

gb_inline void gb_free(gbAllocator a, void *ptr) {
  if (ptr != NULL)
    a.proc(a.data, gbAllocation_Free, 0, 0, ptr, 0, GB_DEFAULT_ALLOCATOR_FLAGS);
}

gb_inline void gb_free_all(gbAllocator a) {
  a.proc(a.data, gbAllocation_FreeAll, 0, 0, NULL, 0, GB_DEFAULT_ALLOCATOR_FLAGS);
}

gb_inline void *gb_resize(gbAllocator a, void *ptr, isize old_size, isize new_size) {
  return gb_resize_align(a, ptr, old_size, new_size, GB_DEFAULT_MEMORY_ALIGNMENT);
}

gb_inline void *gb_resize_align(gbAllocator a, void *ptr, isize old_size, isize new_size, isize alignment) {
  return a.proc(a.data, gbAllocation_Resize, new_size, alignment, ptr, old_size, GB_DEFAULT_ALLOCATOR_FLAGS);
}

gb_inline void *gb_alloc_copy(gbAllocator a, void const *src, isize size) {
  return gb_memcopy(gb_alloc(a, size), src, size);
}

gb_inline void *gb_alloc_copy_align(gbAllocator a, void const *src, isize size, isize alignment) {
  return gb_memcopy(gb_alloc_align(a, size, alignment), src, size);
}

gb_inline char *gb_alloc_str(gbAllocator a, char const *str) {
  return gb_alloc_str_len(a, str, gb_strlen(str));
}

gb_inline char *gb_alloc_str_len(gbAllocator a, char const *str, isize len) {
  char *result;
  result = cast(char *) gb_alloc_copy(a, str, len + 1);
  result[len] = '\0';
  return result;
}

gb_inline void *
gb_default_resize_align(gbAllocator a, void *old_memory, isize old_size, isize new_size, isize alignment) {
  if (!old_memory) return gb_alloc_align(a, new_size, alignment);

  if (new_size == 0) {
    gb_free(a, old_memory);
    return NULL;
  }

  if (new_size < old_size)
    new_size = old_size;

  if (old_size == new_size) {
    return old_memory;
  } else {
    void *new_memory = gb_alloc_align(a, new_size, alignment);
    if (!new_memory) return NULL;
    gb_memmove(new_memory, old_memory, gb_min(new_size, old_size));
    gb_free(a, old_memory);
    return new_memory;
  }
}

gb_inline gbAllocator gb_heap_allocator(void) {
  gbAllocator a;
  a.proc = gb_heap_allocator_proc;
  a.data = NULL;
  return a;
}

GB_ALLOCATOR_PROC(gb_heap_allocator_proc) {
  void *ptr = NULL;
  gb_unused(allocator_data);
  gb_unused(old_size);
// TODO(bill): Throughly test!
  switch (type) {
#if defined(GB_COMPILER_MSVC)
    case gbAllocation_Alloc:
    ptr = _aligned_malloc(size, alignment);
    if (flags & gbAllocatorFlag_ClearToZero)
      gb_zero_size(ptr, size);
    break;
  case gbAllocation_Free:
    _aligned_free(old_memory);
    break;
  case gbAllocation_Resize:
    ptr = _aligned_realloc(old_memory, size, alignment);
    break;
#else
    // TODO(bill): *nix version that's decent
    case gbAllocation_Alloc: {
      posix_memalign(&ptr, alignment, size);

      if (flags & gbAllocatorFlag_ClearToZero) {
        gb_zero_size(ptr, size);
      }
    }
      break;

    case gbAllocation_Free: {
      free(old_memory);
    }
      break;

    case gbAllocation_Resize: {
      gbAllocator a = gb_heap_allocator();
      ptr = gb_default_resize_align(a, old_memory, old_size, size, alignment);
    }
      break;
#endif

    case gbAllocation_FreeAll:
      break;
  }

  return ptr;
}

gbVirtualMemory gb_virtual_memory(void *data, isize size) {
  gbVirtualMemory vm;
  vm.data = data;
  vm.size = size;
  return vm;
}

#if GB_SYSTEM_WINDOWS
gb_inline gbVirtualMemory gb_vm_alloc(void *addr, isize size) {
  gbVirtualMemory vm;
  GB_ASSERT(size > 0);
  vm.data = VirtualAlloc(addr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  vm.size = size;
  return vm;
}

gb_inline b32 gb_vm_free(gbVirtualMemory vm) {
  MEMORY_BASIC_INFORMATION info;
  while (vm.size > 0) {
    if (VirtualQuery(vm.data, &info, gb_size_of(info)) == 0)
      return false;
    if (info.BaseAddress != vm.data ||
        info.AllocationBase != vm.data ||
        info.State != MEM_COMMIT || info.RegionSize > cast(usize)vm.size) {
      return false;
    }
    if (VirtualFree(vm.data, 0, MEM_RELEASE) == 0)
      return false;
    vm.data = gb_pointer_add(vm.data, info.RegionSize);
    vm.size -= info.RegionSize;
  }
  return true;
}

gb_inline gbVirtualMemory gb_vm_trim(gbVirtualMemory vm, isize lead_size, isize size) {
  gbVirtualMemory new_vm = {0};
  void *ptr;
  GB_ASSERT(vm.size >= lead_size + size);

  ptr = gb_pointer_add(vm.data, lead_size);

  gb_vm_free(vm);
  new_vm = gb_vm_alloc(ptr, size);
  if (new_vm.data == ptr)
    return new_vm;
  if (new_vm.data)
    gb_vm_free(new_vm);
  return new_vm;
}

gb_inline b32 gb_vm_purge(gbVirtualMemory vm) {
  VirtualAlloc(vm.data, vm.size, MEM_RESET, PAGE_READWRITE);
  // NOTE(bill): Can this really fail?
  return true;
}

isize gb_virtual_memory_page_size(isize *alignment_out) {
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  if (alignment_out) *alignment_out = info.dwAllocationGranularity;
  return info.dwPageSize;
}

#else

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

gb_inline gbVirtualMemory gb_vm_alloc(void *addr, isize size) {
  gbVirtualMemory vm;
  GB_ASSERT(size > 0);
  vm.data = mmap(addr, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  vm.size = size;
  return vm;
}

gb_inline b32 gb_vm_free(gbVirtualMemory vm) {
  munmap(vm.data, vm.size);
  return true;
}

gb_inline gbVirtualMemory gb_vm_trim(gbVirtualMemory vm, isize lead_size, isize size) {
  void *ptr;
  isize trail_size;
  GB_ASSERT(vm.size >= lead_size + size);

  ptr = gb_pointer_add(vm.data, lead_size);
  trail_size = vm.size - lead_size - size;

  if (lead_size != 0)
    gb_vm_free(gb_virtual_memory(vm.data, lead_size));
  if (trail_size != 0)
    gb_vm_free(gb_virtual_memory(ptr, trail_size));
  return gb_virtual_memory(ptr, size);

}

gb_inline b32 gb_vm_purge(gbVirtualMemory vm) {
  int err = madvise(vm.data, vm.size, MADV_DONTNEED);
  return err != 0;
}

isize gb_virtual_memory_page_size(isize *alignment_out) {
  // TODO(bill): Is this always true?
  isize result = cast(isize) sysconf(_SC_PAGE_SIZE);
  if (alignment_out) *alignment_out = result;
  return result;
}

#endif




////////////////////////////////////////////////////////////////
//
// Custom Allocation
//
//


//
// Arena Allocator
//

gb_inline void gb_arena_init_from_memory(gbArena *arena, void *start, isize size) {
  arena->backing.proc = NULL;
  arena->backing.data = NULL;
  arena->physical_start = start;
  arena->total_size = size;
  arena->total_allocated = 0;
  arena->temp_count = 0;
}

gb_inline void gb_arena_init_from_allocator(gbArena *arena, gbAllocator backing, isize size) {
  arena->backing = backing;
  arena->physical_start = gb_alloc(backing, size); // NOTE(bill): Uses default alignment
  arena->total_size = size;
  arena->total_allocated = 0;
  arena->temp_count = 0;
}

gb_inline void gb_arena_init_sub(gbArena *arena, gbArena *parent_arena, isize size) {
  gb_arena_init_from_allocator(arena, gb_arena_allocator(parent_arena), size);
}

gb_inline void gb_arena_free(gbArena *arena) {
  if (arena->backing.proc) {
    gb_free(arena->backing, arena->physical_start);
    arena->physical_start = NULL;
  }
}

gb_inline isize gb_arena_alignment_of(gbArena *arena, isize alignment) {
  isize alignment_offset, result_pointer, mask;
  GB_ASSERT(gb_is_power_of_two(alignment));

  alignment_offset = 0;
  result_pointer = cast(isize) arena->physical_start + arena->total_allocated;
  mask = alignment - 1;
  if (result_pointer & mask)
    alignment_offset = alignment - (result_pointer & mask);

  return alignment_offset;
}

gb_inline isize gb_arena_size_remaining(gbArena *arena, isize alignment) {
  isize result = arena->total_size - (arena->total_allocated + gb_arena_alignment_of(arena, alignment));
  return result;
}

gb_inline void gb_arena_check(gbArena *arena) { GB_ASSERT(arena->temp_count == 0); }

gb_inline gbAllocator gb_arena_allocator(gbArena *arena) {
  gbAllocator allocator;
  allocator.proc = gb_arena_allocator_proc;
  allocator.data = arena;
  return allocator;
}

GB_ALLOCATOR_PROC(gb_arena_allocator_proc) {
  gbArena *arena = cast(gbArena *) allocator_data;
  void *ptr = NULL;

  gb_unused(old_size);

  switch (type) {
    case gbAllocation_Alloc: {
      void *end = gb_pointer_add(arena->physical_start, arena->total_allocated);
      isize total_size = size + alignment;

      // NOTE(bill): Out of memory
      if (arena->total_allocated + total_size > cast(isize) arena->total_size) {
        gb_printf_err("Arena out of memory\n");
        return NULL;
      }

      ptr = gb_align_forward(end, alignment);
      arena->total_allocated += total_size;
      if (flags & gbAllocatorFlag_ClearToZero)
        gb_zero_size(ptr, size);
    }
      break;

    case gbAllocation_Free:
      // NOTE(bill): Free all at once
      // Use Temp_Arena_Memory if you want to free a block
      break;

    case gbAllocation_FreeAll:
      arena->total_allocated = 0;
      break;

    case gbAllocation_Resize: {
      // TODO(bill): Check if ptr is on top of stack and just extend
      gbAllocator a = gb_arena_allocator(arena);
      ptr = gb_default_resize_align(a, old_memory, old_size, size, alignment);
    }
      break;
  }
  return ptr;
}

gb_inline gbTempArenaMemory gb_temp_arena_memory_begin(gbArena *arena) {
  gbTempArenaMemory tmp;
  tmp.arena = arena;
  tmp.original_count = arena->total_allocated;
  arena->temp_count++;
  return tmp;
}

gb_inline void gb_temp_arena_memory_end(gbTempArenaMemory tmp) {
  GB_ASSERT(tmp.arena->total_allocated >= tmp.original_count);
  GB_ASSERT(tmp.arena->temp_count > 0);
  tmp.arena->total_allocated = tmp.original_count;
  tmp.arena->temp_count--;
}




//
// Pool Allocator
//


gb_inline void gb_pool_init(gbPool *pool, gbAllocator backing, isize num_blocks, isize block_size) {
  gb_pool_init_align(pool, backing, num_blocks, block_size, GB_DEFAULT_MEMORY_ALIGNMENT);
}

void gb_pool_init_align(gbPool *pool, gbAllocator backing, isize num_blocks, isize block_size, isize block_align) {
  isize actual_block_size, pool_size, block_index;
  void *data, *curr;
  uintptr *end;

  gb_zero_item(pool);

  pool->backing = backing;
  pool->block_size = block_size;
  pool->block_align = block_align;

  actual_block_size = block_size + block_align;
  pool_size = num_blocks * actual_block_size;

  data = gb_alloc_align(backing, pool_size, block_align);

  // NOTE(bill): Init intrusive freelist
  curr = data;
  for (block_index = 0; block_index < num_blocks - 1; block_index++) {
    uintptr *next = cast(uintptr *) curr;
    *next = cast(uintptr) curr + actual_block_size;
    curr = gb_pointer_add(curr, actual_block_size);
  }

  end = cast(uintptr *) curr;
  *end = cast(uintptr)NULL;

  pool->physical_start = data;
  pool->free_list = data;
}

gb_inline void gb_pool_free(gbPool *pool) {
  if (pool->backing.proc) {
    gb_free(pool->backing, pool->physical_start);
  }
}

gb_inline gbAllocator gb_pool_allocator(gbPool *pool) {
  gbAllocator allocator;
  allocator.proc = gb_pool_allocator_proc;
  allocator.data = pool;
  return allocator;
}

GB_ALLOCATOR_PROC(gb_pool_allocator_proc) {
  gbPool *pool = cast(gbPool *) allocator_data;
  void *ptr = NULL;

  gb_unused(old_size);

  switch (type) {
    case gbAllocation_Alloc: {
      uintptr next_free;
      GB_ASSERT(size == pool->block_size);
      GB_ASSERT(alignment == pool->block_align);
      GB_ASSERT(pool->free_list != NULL);

      next_free = *cast(uintptr *) pool->free_list;
      ptr = pool->free_list;
      pool->free_list = cast(void *) next_free;
      pool->total_size += pool->block_size;
      if (flags & gbAllocatorFlag_ClearToZero)
        gb_zero_size(ptr, size);
    }
      break;

    case gbAllocation_Free: {
      uintptr *next;
      if (old_memory == NULL) return NULL;

      next = cast(uintptr *) old_memory;
      *next = cast(uintptr) pool->free_list;
      pool->free_list = old_memory;
      pool->total_size -= pool->block_size;
    }
      break;

    case gbAllocation_FreeAll:
      // TODO(bill):
      break;

    case gbAllocation_Resize:
      // NOTE(bill): Cannot resize
      GB_PANIC("You cannot resize something allocated by with a pool.");
      break;
  }

  return ptr;
}

gb_inline gbAllocationHeader *gb_allocation_header(void *data) {
  isize *p = cast(isize *) data;
  while (p[-1] == cast(isize) (-1))
    p--;
  return cast(gbAllocationHeader *) p - 1;
}

gb_inline void gb_allocation_header_fill(gbAllocationHeader *header, void *data, isize size) {
  isize *ptr;
  header->size = size;
  ptr = cast(isize *) (header + 1);
  while (cast(void *) ptr < data)
    *ptr++ = cast(isize) (-1);
}



//
// Free List Allocator
//

gb_inline void gb_free_list_init(gbFreeList *fl, void *start, isize size) {
  GB_ASSERT(size > gb_size_of(gbFreeListBlock));

  fl->physical_start = start;
  fl->total_size = size;
  fl->curr_block = cast(gbFreeListBlock *) start;
  fl->curr_block->size = size;
  fl->curr_block->next = NULL;
}

gb_inline void gb_free_list_init_from_allocator(gbFreeList *fl, gbAllocator backing, isize size) {
  void *start = gb_alloc(backing, size);
  gb_free_list_init(fl, start, size);
}

gb_inline gbAllocator gb_free_list_allocator(gbFreeList *fl) {
  gbAllocator a;
  a.proc = gb_free_list_allocator_proc;
  a.data = fl;
  return a;
}

GB_ALLOCATOR_PROC(gb_free_list_allocator_proc) {
  gbFreeList *fl = cast(gbFreeList *) allocator_data;
  void *ptr = NULL;

  GB_ASSERT_NOT_NULL(fl);

  switch (type) {
    case gbAllocation_Alloc: {
      gbFreeListBlock *prev_block = NULL;
      gbFreeListBlock *curr_block = fl->curr_block;

      while (curr_block) {
        isize total_size;
        gbAllocationHeader *header;

        total_size = size + alignment + gb_size_of(gbAllocationHeader);

        if (curr_block->size < total_size) {
          prev_block = curr_block;
          curr_block = curr_block->next;
          continue;
        }

        if (curr_block->size - total_size <= gb_size_of(gbAllocationHeader)) {
          total_size = curr_block->size;

          if (prev_block)
            prev_block->next = curr_block->next;
          else
            fl->curr_block = curr_block->next;
        } else {
          // NOTE(bill): Create a new block for the remaining memory
          gbFreeListBlock *next_block;
          next_block = cast(gbFreeListBlock *) gb_pointer_add(curr_block, total_size);

          GB_ASSERT(cast(
                      void *)next_block < gb_pointer_add(fl->physical_start, fl->total_size));

          next_block->size = curr_block->size - total_size;
          next_block->next = curr_block->next;

          if (prev_block)
            prev_block->next = next_block;
          else
            fl->curr_block = next_block;
        }


        // TODO(bill): Set Header Info
        header = cast(gbAllocationHeader *) curr_block;
        ptr = gb_align_forward(header + 1, alignment);
        gb_allocation_header_fill(header, ptr, size);

        fl->total_allocated += total_size;
        fl->allocation_count++;

        if (flags & gbAllocatorFlag_ClearToZero)
          gb_zero_size(ptr, size);
        return ptr;
      }
      // NOTE(bill): if ptr == NULL, ran out of free list memory! FUCK!
      return NULL;
    }
      break;

    case gbAllocation_Free: {
      gbAllocationHeader *header = gb_allocation_header(old_memory);
      isize block_size = header->size;
      uintptr block_start, block_end;
      gbFreeListBlock *prev_block = NULL;
      gbFreeListBlock *curr_block = fl->curr_block;

      block_start = cast(uintptr) header;
      block_end = cast(uintptr) block_start + block_size;

      while (curr_block) {
        if (cast(uintptr) curr_block >= block_end)
          break;
        prev_block = curr_block;
        curr_block = curr_block->next;
      }

      if (prev_block == NULL) {
        prev_block = cast(gbFreeListBlock *) block_start;
        prev_block->size = block_size;
        prev_block->next = fl->curr_block;

        fl->curr_block = prev_block;
      } else if ((cast(uintptr) prev_block + prev_block->size) == block_start) {
        prev_block->size += block_size;
      } else {
        gbFreeListBlock *tmp = cast(gbFreeListBlock *) block_start;
        tmp->size = block_size;
        tmp->next = prev_block->next;
        prev_block->next = tmp;

        prev_block = tmp;
      }

      if (curr_block && (cast(uintptr) curr_block == block_end)) {
        prev_block->size += curr_block->size;
        prev_block->next = curr_block->next;
      }

      fl->allocation_count--;
      fl->total_allocated -= block_size;
    }
      break;

    case gbAllocation_FreeAll:
      gb_free_list_init(fl, fl->physical_start, fl->total_size);
      break;

    case gbAllocation_Resize:
      ptr = gb_default_resize_align(gb_free_list_allocator(fl), old_memory, old_size, size, alignment);
      break;
  }

  return ptr;
}

void gb_scratch_memory_init(gbScratchMemory *s, void *start, isize size) {
  s->physical_start = start;
  s->total_size = size;
  s->alloc_point = start;
  s->free_point = start;
}

b32 gb_scratch_memory_is_in_use(gbScratchMemory *s, void *ptr) {
  if (s->free_point == s->alloc_point) return false;
  if (s->alloc_point > s->free_point)
    return ptr >= s->free_point && ptr < s->alloc_point;
  return ptr >= s->free_point || ptr < s->alloc_point;
}

gbAllocator gb_scratch_allocator(gbScratchMemory *s) {
  gbAllocator a;
  a.proc = gb_scratch_allocator_proc;
  a.data = s;
  return a;
}

GB_ALLOCATOR_PROC(gb_scratch_allocator_proc) {
  gbScratchMemory *s = cast(gbScratchMemory *) allocator_data;
  void *ptr = NULL;
  GB_ASSERT_NOT_NULL(s);

  switch (type) {
    case gbAllocation_Alloc: {
      void *pt = s->alloc_point;
      gbAllocationHeader *header = cast(gbAllocationHeader *) pt;
      void *data = gb_align_forward(header + 1, alignment);
      void *end = gb_pointer_add(s->physical_start, s->total_size);

      GB_ASSERT(alignment % 4 == 0);
      size = ((size + 3) / 4) * 4;
      pt = gb_pointer_add(pt, size);

      // NOTE(bill): Wrap around
      if (pt > end) {
        header->size = gb_pointer_diff(header, end) | GB_ISIZE_HIGH_BIT;
        pt = s->physical_start;
        header = cast(gbAllocationHeader *) pt;
        data = gb_align_forward(header + 1, alignment);
        pt = gb_pointer_add(pt, size);
      }

      if (!gb_scratch_memory_is_in_use(s, pt)) {
        gb_allocation_header_fill(header, pt, gb_pointer_diff(header, pt));
        s->alloc_point = cast(u8 *) pt;
        ptr = data;
      }

      if (flags & gbAllocatorFlag_ClearToZero)
        gb_zero_size(ptr, size);
    }
      break;

    case gbAllocation_Free: {
      if (old_memory) {
        void *end = gb_pointer_add(s->physical_start, s->total_size);
        if (old_memory < s->physical_start || old_memory >= end) {
          GB_ASSERT(false);
        } else {
          // NOTE(bill): Mark as free
          gbAllocationHeader *h = gb_allocation_header(old_memory);
          GB_ASSERT((h->size & GB_ISIZE_HIGH_BIT) == 0);
          h->size = h->size | GB_ISIZE_HIGH_BIT;

          while (s->free_point != s->alloc_point) {
            gbAllocationHeader *header = cast(gbAllocationHeader *) s->free_point;
            if ((header->size & GB_ISIZE_HIGH_BIT) == 0)
              break;

            s->free_point = gb_pointer_add(s->free_point, h->size & (~GB_ISIZE_HIGH_BIT));
            if (s->free_point == end)
              s->free_point = s->physical_start;
          }
        }
      }
    }
      break;

    case gbAllocation_FreeAll:
      s->alloc_point = s->physical_start;
      s->free_point = s->physical_start;
      break;

    case gbAllocation_Resize:
      ptr = gb_default_resize_align(gb_scratch_allocator(s), old_memory, old_size, size, alignment);
      break;
  }

  return ptr;
}
