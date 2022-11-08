/*
 * M*LIB - MEMPOOL module
 *
 * Copyright (c) 2017-2022, Patrick Pelissier
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * + Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * + Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef MSTARLIB_MEMPOOL_H
#define MSTARLIB_MEMPOOL_H

#include "m-core.h"

/* Fast, fixed size, thread unsafe allocator based on memory regions.
   No oplist is needed.
   USAGE:
     MEMPOOL_DEF(name, type)
   Example:
     MEMPOOL_DEF(mempool_uint, unsigned int)
     ...
     mempool_uint_t m;
     mempool_uint_init(m);
     unsigned int *ptr = mempool_uint_alloc(m);
     *ptr = 17;
     mempool_uint_free(m, ptr);
     mempool_uint_clear(m); // Give back memory to system
   Technically, it uses a list of memory regions, where multiple
   allocations are performed in each region. However, it
   can not use m-list since it may be expanded from LIST_DEF
   (recursive dependency problem).
*/
#define M_MEMPOOL_DEF(name, type)                                             \
  M_MEMPOOL_DEF_AS(name, M_C(name,_t), type)


/* Fast, fixed Size, thread unsafe allocator based on memory region.
   USAGE:
     MEMPOOL_DEF_AS(name, name_t, type)
*/
#define M_MEMPOOL_DEF_AS(name, name_t, type)                                  \
  M_BEGIN_PROTECTED_CODE                                                      \
  M_M3MPOOL_DEF_P2(name, type, name_t )                                       \
  M_END_PROTECTED_CODE


/* User shall be able to cutomize the size of the region segment and/or
   the minimun number of elements.
   The default is the number of elements that fits in 16KB, or 256
   is the size of the type is too big.
*/
#ifndef M_USE_MEMPOOL_MAX_PER_SEGMENT
#define M_USE_MEMPOOL_MAX_PER_SEGMENT(type)                                   \
  M_MAX((16*1024-sizeof(unsigned int) - 2*sizeof(void*)) / sizeof (type), 256U)
#endif


/********************************** INTERNAL ************************************/

#define M_M3MPOOL_DEF_P2(name, type, name_t)                                  \
                                                                              \
  /* Define the type of element in a segment of the mempool.                  \
    Either it is the basic type or a pointer to another one.  */              \
  typedef union M_C(name,_union_s) {                                          \
    type t;                                                                   \
    union M_C(name,_union_s) *next;                                           \
  } M_C(name,_union_ct);                                                      \
                                                                              \
  /* Define a segment of a mempool.                                           \
    It is an array of basic type, each segment is in a linked list */         \
  typedef struct M_C(name,_segment_s) {                                       \
    unsigned int count;                                                       \
    struct M_C(name,_segment_s) *next;                                        \
    M_C(name,_union_ct)        tab[M_USE_MEMPOOL_MAX_PER_SEGMENT(type)];      \
  } M_C(name,_segment_ct);                                                    \
                                                                              \
  /* Define a mempool.                                                        \
    It is a pointer to the first free object within the segments              \
    and the segments themselves           */                                  \
  typedef struct M_C(name, _s) {                                              \
    M_C(name,_union_ct)   *free_list;                                         \
    M_C(name,_segment_ct) *current_segment;                                   \
  } name_t[1];                                                                \
                                                                              \
  static inline void                                                          \
  M_C(name,_init)(name_t mem)                                                 \
  {                                                                           \
    mem->free_list = NULL;                                                    \
    mem->current_segment = M_MEMORY_ALLOC(M_C(name,_segment_ct));             \
    if (mem->current_segment == NULL) {                                       \
      M_MEMORY_FULL(sizeof (M_C(name,_segment_ct)));                          \
      return;                                                                 \
    }                                                                         \
    mem->current_segment->next = NULL;                                        \
    mem->current_segment->count = 0;                                          \
    M_M3MPOOL_CONTRACT(mem, type);                                            \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name,_clear)(name_t mem)                                                \
  {                                                                           \
    M_M3MPOOL_CONTRACT(mem, type);                                            \
    M_C(name,_segment_ct) *segment = mem->current_segment;                    \
    while (segment != NULL) {                                                 \
      M_C(name,_segment_ct) *next = segment->next;                            \
      M_MEMORY_DEL (segment);                                                 \
      segment = next;                                                         \
    }                                                                         \
    /* Clean pointers to be safer */                                          \
    mem->free_list = NULL;                                                    \
    mem->current_segment = NULL;                                              \
  }                                                                           \
                                                                              \
  static inline type *                                                        \
  M_C(name,_alloc)(name_t mem)                                                \
  {                                                                           \
    M_M3MPOOL_CONTRACT(mem, type);                                            \
    /* Test if one object is in the free list */                              \
    M_C(name,_union_ct) *ret = mem->free_list;                                \
    if (ret != NULL) {                                                        \
      /* Yes, so return it, and pop it from the free list */                  \
      mem->free_list = ret->next;                                             \
      return &ret->t;                                                         \
    }                                                                         \
    /* No cheap free object exist. Test within a segment */                   \
    M_C(name,_segment_ct) *segment = mem->current_segment;                    \
    M_ASSERT(segment != NULL);                                                \
    unsigned int count = segment->count;                                      \
    /* If segment is full, allocate a new one from the system */              \
    if (M_UNLIKELY (count >= M_USE_MEMPOOL_MAX_PER_SEGMENT(type))) {          \
      M_C(name,_segment_ct) *new_segment = M_MEMORY_ALLOC (M_C(name,_segment_ct)); \
      if (M_UNLIKELY (new_segment == NULL)) {                                 \
        M_MEMORY_FULL(sizeof (M_C(name,_segment_ct)));                        \
        return NULL;                                                          \
      }                                                                       \
      new_segment->next = segment;                                            \
      new_segment->count = 0;                                                 \
      mem->current_segment = new_segment;                                     \
      segment = new_segment;                                                  \
      count = 0;                                                              \
    }                                                                         \
    /* Return the object as the last element of the current segment */        \
    ret = &segment->tab[count];                                               \
    segment->count = count + 1;                                               \
    M_M3MPOOL_CONTRACT(mem, type);                                            \
    return &ret->t;                                                           \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name,_free)(name_t mem, type *ptr)                                      \
  {                                                                           \
    M_M3MPOOL_CONTRACT(mem, type);                                            \
    /* NOTE: Unsafe cast: suppose that the given pointer                      \
       was allocated by the previous alloc function. */                       \
    M_C(name,_union_ct) *ret = (M_C(name,_union_ct) *)(uintptr_t)ptr;         \
    /* Add the object back in the free list */                                \
    ret->next = mem->free_list;                                               \
    mem->free_list = ret;                                                     \
    /* NOTE: the objects are NOT given back to the system until the mempool   \
    is fully cleared */                                                       \
    M_M3MPOOL_CONTRACT(mem, type);                                            \
  }                                                                           \

/* MEMPOOL contract. We only control the current segment. */
#define M_M3MPOOL_CONTRACT(mempool, type) do {                                \
    M_ASSERT((mempool) != NULL);                                              \
    M_ASSERT((mempool)->current_segment != NULL);                             \
    M_ASSERT((mempool)->current_segment->count <= M_USE_MEMPOOL_MAX_PER_SEGMENT(type)); \
  } while (0)


#if M_USE_SMALL_NAME
#define MEMPOOL_DEF M_MEMPOOL_DEF
#define MEMPOOL_DEF_AS M_MEMPOOL_DEF_AS
#endif

#endif
