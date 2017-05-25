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

#ifndef  GB_HTABLE_H__
# define GB_HTABLE_H__

#include "gb/hash.h"

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
}

#endif /* GB_HTABLE_H__ */
