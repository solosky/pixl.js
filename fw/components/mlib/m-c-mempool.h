/*
 * M*LIB - Concurrent memory pool allocator
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
#ifndef MSTARLIB_CONCURRENT_MEMPOOL_H
#define MSTARLIB_CONCURRENT_MEMPOOL_H

#include "m-core.h"
#include "m-atomic.h"
#include "m-genint.h"

M_BEGIN_PROTECTED_CODE

/* Minimum number of nodes per group of nodes */
#define M_CMEMP00L_MIN_NODE_PER_GROUP 16

#define M_C_MEMPOOL_DEF(name, type_t)                                         \
  M_BEGIN_PROTECTED_CODE                                                      \
  M_CMEMP00L_DEF_SINGLY_LIST(name, type_t)                                    \
  M_CMEMP00L_DEF_LF_QUEUE(name, type_t)                                       \
  M_CMEMP00L_DEF_LFMP_THREAD_MEMPOOL(name, type_t)                            \
  M_CMEMP00L_DEF_SYSTEM_ALLOC(name, type_t)                                   \
  M_CMEMP00L_DEF_LF_MEMPOOL(name, type_t)                                     \
  M_END_PROTECTED_CODE

/* Classic internal Singly List without allocation */
#define M_CMEMP00L_DEF_SINGLY_LIST(name, type_t)                              \
                                                                              \
  typedef struct M_C(name, _slist_node_s) {                                   \
    struct M_C(name, _slist_node_s) *next;                                    \
    type_t               data;                                                \
  } M_C(name, _slist_node_ct);                                                \
                                                                              \
  typedef struct M_C(name, _slist_node_s) *M_C(name, _slist_ct)[1];           \
                                                                              \
  static inline void                                                          \
  M_C(name, _slist_init)(M_C(name, _slist_ct) list)                           \
  {                                                                           \
    *list = NULL;                                                             \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _slist_push)(M_C(name, _slist_ct) list,                           \
                        M_C(name, _slist_node_ct) *node)                      \
  {                                                                           \
    node->next = *list;                                                       \
    *list = node;                                                             \
  }                                                                           \
                                                                              \
  static inline M_C(name, _slist_node_ct) *                                   \
  M_C(name, _slist_pop)(M_C(name, _slist_ct) list)                            \
  {                                                                           \
    M_ASSERT (*list != NULL);                                                 \
    M_C(name, _slist_node_ct) *node = *list;                                  \
    *list = node->next;                                                       \
    M_IF_DEBUG(node->next = NULL;)                                            \
    return node;                                                              \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_C(name, _slist_empty_p)(M_C(name, _slist_ct) list)                        \
  {                                                                           \
    return *list == NULL;                                                     \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _slist_move)(M_C(name, _slist_ct) list,                           \
                         M_C(name, _slist_ct) src)                            \
  {                                                                           \
    *list = *src;                                                             \
    *src  = NULL;                                                             \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _slist_clear)(M_C(name, _slist_ct) list)                          \
  {                                                                           \
    M_C(name, _slist_node_ct) *it = *list, *next;                             \
    while (it) {                                                              \
      next = it->next;                                                        \
      M_MEMORY_DEL(it);                                                       \
      it = next;                                                              \
    }                                                                         \
    *list = NULL;                                                             \
  }                                                                           \


/* Lock Free free queue list (not generic one) of lists without allocation
   Based on Michael & Scott Lock Free Queue List algorithm.
   Each list is considered empty if there is only one node within.
   This LF Queue List doesn't try to prevent the ABA problem. It is up to the 
   caller to avoid recycling the nodes too fast.
   Each list has its own unique NIL ptr in order to avoid issues when 
   migrating a node from a Q to another: in the following scenario,
   - Thread 1 performs a PUSH of N in Q1 with Q1 empty (only node is NA)
   NA.next is NIL.
   - Thread 1 is interrupted just before the CAS on NA.next
   - Thread 2 performs a sucessfull push of NB in Q1. NA.next is set to NB.
   - Thread 2 performs a sucessfull pop of NA in Q1
   - Thread 2 performs a sucessfull push of NA in Q2. NA.next is set to NIL.
   - Thread 1 is restored and will succeed as NA.next is once again NIL.
   In order to prevent the last CAS to succeed, each queue uses its own NIL pointer.
   It is a derived problem of the ABA problem.
 */
/* TODO: Optimize alignement to reduce memory consumption. NIL object can use [] 
   to reduce memory consumption too (non compatible with C++ ...) */
#define M_CMEMP00L_DEF_LF_QUEUE(name, type_t)                                 \
                                                                              \
  typedef struct M_C(name, _lf_node_s) {                                      \
    M_ATTR_EXTENSION _Atomic(struct M_C(name, _lf_node_s) *) next;            \
    m_gc_atomic_ticket_ct                    cpt;                             \
    M_C(name, _slist_ct)                     list;                            \
  } M_C(name, _lf_node_t);                                                    \
                                                                              \
  typedef struct M_C(name, _lflist_s) {                                       \
    M_ATTR_EXTENSION _Atomic(M_C(name, _lf_node_t) *) head;                   \
    char                       align1[M_ALIGN_FOR_CACHELINE_EXCLUSION];       \
    M_ATTR_EXTENSION _Atomic(M_C(name, _lf_node_t) *) tail;                   \
    char                       align2[M_ALIGN_FOR_CACHELINE_EXCLUSION];       \
    M_C(name, _lf_node_t)            nil;                                     \
  } M_C(name, _lflist_ct)[1];                                                 \
                                                                              \
  static inline void                                                          \
  M_C(name, _lflist_init)(M_C(name, _lflist_ct) list,                         \
                          M_C(name, _lf_node_t) *node)                        \
  {                                                                           \
    atomic_init(&list->head, node);                                           \
    atomic_init(&list->tail, node);                                           \
    atomic_store_explicit(&node->next, &list->nil, memory_order_relaxed);     \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_C(name, _lflist_empty_p)(M_C(name, _lflist_ct) list)                      \
  {                                                                           \
    return atomic_load(&list->tail) == atomic_load(&list->head);              \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _lflist_push)(M_C(name, _lflist_ct) list,                         \
                          M_C(name, _lf_node_t) *node, m_core_backoff_ct bkoff) \
  {                                                                           \
    M_C(name, _lf_node_t) *tail;                                              \
    M_C(name, _lf_node_t) *next;                                              \
                                                                              \
    atomic_store_explicit(&node->next, &list->nil, memory_order_relaxed);     \
    m_core_backoff_reset(bkoff);                                              \
    while (true) {                                                            \
      tail = atomic_load(&list->tail);                                        \
      next = atomic_load_explicit(&tail->next, memory_order_acquire);         \
      if (M_UNLIKELY(next != &list->nil)) {                                   \
        /* Tail was not pointing to the last node                             \
           Try to swing Tail to the next node */                              \
        atomic_compare_exchange_weak_explicit(&list->tail,                    \
                                              &tail, next,                    \
                                              memory_order_release,           \
                                              memory_order_relaxed);          \
      } else {                                                                \
        /* Try to link node at the end of the linked list */                  \
        if (atomic_compare_exchange_strong_explicit(&tail->next,              \
                                                    &next, node,              \
                                                    memory_order_release,     \
                                                    memory_order_relaxed))    \
          break;                                                              \
        m_core_backoff_wait(bkoff);                                           \
      }                                                                       \
    }                                                                         \
    /* Enqueue is done.  Try to swing Tail to the inserted node               \
       If it fails, someone else will do it or has already did it. */         \
    atomic_compare_exchange_strong_explicit(&list->tail, &tail, node,         \
                                            memory_order_acq_rel,             \
                                            memory_order_relaxed);            \
  }                                                                           \
                                                                              \
  static inline M_C(name, _lf_node_t) *                                       \
  M_C(name, _lflist_pop)(M_C(name, _lflist_ct) list, m_core_backoff_ct bkoff) \
  {                                                                           \
    M_C(name, _lf_node_t) *head;                                              \
    M_C(name, _lf_node_t) *tail;                                              \
    M_C(name, _lf_node_t) *next;                                              \
                                                                              \
    /* Reinitialize backoff */                                                \
    m_core_backoff_reset(bkoff);                                              \
    while (true) {                                                            \
      head = atomic_load(&list->head);                                        \
      tail = atomic_load(&list->tail);                                        \
      next = atomic_load(&head->next);                                        \
      /* Are head, tail, and next consistent?*/                               \
      if (M_LIKELY(head ==                                                    \
                   atomic_load_explicit(&list->head, memory_order_relaxed)))  \
        {                                                                     \
          /* Is queue empty or Tail falling behind? */                        \
          if (head == tail) {                                                 \
            /* Is queue empty? */                                             \
            if (next == &list->nil)                                           \
              return NULL;                                                    \
            /* Tail is falling behind.  Try to advance it */                  \
            atomic_compare_exchange_strong_explicit(&list->tail, &tail,       \
                                                    next,                     \
                                                    memory_order_release,     \
                                                    memory_order_relaxed);    \
          } else {                                                            \
            /* Try to swing Head to the next node */                          \
            if (atomic_compare_exchange_strong_explicit(&list->head,          \
                                                        &head, next,          \
                                                        memory_order_release, \
                                                        memory_order_relaxed)) { \
              break;                                                          \
            }                                                                 \
            /* Failure: perform a random exponential backoff */               \
            m_core_backoff_wait(bkoff);                                       \
          }                                                                   \
        }                                                                     \
    }                                                                         \
    /* dequeue returns an element that becomes the new dummy element (the new head), \
       and the former dummy element (the former head) is removed:             \
       Since we want a link of free list, and we don't care about the content itsef, \
       provided that the node we return is older than the one we should return, \
       Therefore, we return the previous dummy head.                          \
       As such, it is not the original MSqueue algorithm. */                  \
    M_IF_DEBUG(atomic_store(&head->next, (M_C(name, _lf_node_t) *) 0);)       \
   return head;                                                               \
  }                                                                           \
                                                                              \
  /* Dequeue a node if the node is old enough */                              \
  static inline M_C(name, _lf_node_t) *                                       \
  M_C(name, _lflist_pop_if)(M_C(name, _lflist_ct) list,                       \
                            m_gc_ticket_ct age, m_core_backoff_ct bkoff)      \
  {                                                                           \
    M_C(name, _lf_node_t) *head;                                              \
    M_C(name, _lf_node_t) *tail;                                              \
    M_C(name, _lf_node_t) *next;                                              \
                                                                              \
    m_core_backoff_reset(bkoff);                                              \
    while (true) {                                                            \
      head = atomic_load(&list->head);                                        \
      tail = atomic_load(&list->tail);                                        \
      next = atomic_load(&head->next);                                        \
      if (M_LIKELY(head == atomic_load_explicit(&list->head, memory_order_relaxed))) \
        {                                                                     \
          if (head == tail) {                                                 \
            if (next == &list->nil)                                           \
              return NULL;                                                    \
            atomic_compare_exchange_strong_explicit(&list->tail, &tail, next, \
                                                    memory_order_release,     \
                                                    memory_order_relaxed);    \
          } else {                                                            \
            /* Test if the node is old enought to be popped */                \
            if (atomic_load_explicit(&next->cpt, memory_order_relaxed) >= age) \
              return NULL;                                                    \
            /* Try to swing Head to the next node */                          \
            if (atomic_compare_exchange_strong_explicit(&list->head,          \
                                                        &head, next,          \
                                                        memory_order_release, \
                                                        memory_order_relaxed)) { \
              break;                                                          \
            }                                                                 \
            m_core_backoff_wait(bkoff);                                       \
          }                                                                   \
        }                                                                     \
    }                                                                         \
    M_IF_DEBUG(atomic_store(&head->next, (M_C(name, _lf_node_t) *) 0);)       \
   return head;                                                               \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _lflist_clear)(M_C(name, _lflist_ct) list)                        \
  {                                                                           \
    m_core_backoff_ct bkoff;                                                  \
    m_core_backoff_init(bkoff);                                               \
    while (true) {                                                            \
      M_C(name, _lf_node_t) *node = M_C(name, _lflist_pop)(list, bkoff);      \
      if (node == NULL) break;                                                \
      M_C(name, _lf_node_t) *next = atomic_load_explicit(&node->next,         \
                                                         memory_order_relaxed); \
      M_C(name, _slist_clear)(node->list);                                    \
      M_MEMORY_DEL(node);                                                     \
      node = next;                                                            \
    }                                                                         \
    /* Dummy node to free too */                                              \
    M_C(name, _lf_node_t) *dummy;                                             \
    dummy = atomic_load_explicit(&list->head, memory_order_relaxed);          \
    M_C(name, _slist_clear)(dummy->list);                                     \
    M_MEMORY_DEL(dummy);                                                      \
  }                                                                           \

/* System node allocator: request memory to the system.
   As such it is a non Lock-Free path. */
#define M_CMEMP00L_DEF_SYSTEM_ALLOC(name, type_t)                             \
                                                                              \
  static inline M_C(name, _lf_node_t) *                                       \
       M_C(name, _alloc_node)(unsigned int initial)                           \
  {                                                                           \
    M_C(name, _lf_node_t) * node;                                             \
    node = M_MEMORY_ALLOC(M_C(name, _lf_node_t));                             \
    if (M_UNLIKELY(node == NULL)) {                                           \
      M_MEMORY_FULL(sizeof(M_C(name, _lf_node_t)));                           \
      return NULL;                                                            \
    }                                                                         \
    atomic_init(&node->next, (M_C(name, _lf_node_t) *) 0);                    \
    atomic_init(&node->cpt, 0UL);                                             \
    M_C(name, _slist_init)(node->list);                                       \
    for(unsigned i = 0; i < initial; i++) {                                   \
      M_C(name, _slist_node_ct) *n;                                           \
      n = M_MEMORY_ALLOC(M_C(name, _slist_node_ct));                          \
      if (M_UNLIKELY(n == NULL)) {                                            \
        M_MEMORY_FULL(sizeof(M_C(name, _lf_node_t)));                         \
        return NULL;                                                          \
      }                                                                       \
      M_C(name, _slist_push)(node->list, n);                                  \
    }                                                                         \
    return node;                                                              \
  }                                                                           \

/* Concurrent Memory pool
   The data structure is the following.
   Each thread has its own pool of nodes (local) that only it can
   access (it is a singly list). If there is no longer any node in this
   pool, it requests a new pool to the lock free queue of pool (group of
   nodes). If it fails, it requests a new pool to the system allocator
   (and from there it is no longer lock free).
   This memory pool can only be lock free if the initial state is 
   sufficiently dimensionned to avoid calling the system allocator during
   the normal processing.
   Then each thread pushs its deleted node into another pool of nodes,
   where the node is logically deleted (no contain of the node is destroyed
   at this point and the node can be freely accessed by other threads).
   Once the thread mempool is put to sleep, the age of the pool of logical 
   deleted nodes is computed and this pool is move to the Lock Free Queue 
   List of pools to be reclaimed. Then A Garbage Collector is performed
   on this Lock Free Queue list to reclaim all pools thare are sufficiently
   aged (taking into account the grace period of the pool) to be moved back
   to the Lock Free Queue of the free pools.

   Each pool of nodes can be in the following state:
   * FREE state if it is present in the Lock Free Queue of free pools.
   * EMPTY state if it is present in the Lock Free Queue of empty pools
   which means that the nodes present in it has been USED directly by a thread,
   * TO_BE_RECLAIMED state if it is present in the Lock Free Queue of TBR pools

   A pool of nodes will go to the following state:
    FREE --> EMPTY --> TO_BE_RECLAIMED
     ^                      |
     +----------------------+

   The ABA problem is taken into account as a node cannot be reused in the 
   same queue without performing a full cycle of its state. Moreover
   it can only move from TO_BE_RECLAIMED to FREE if and only if a grace 
   period is finished (and then we are sure that no thread references any
   older node).

   Each thread has its own backoff structure (with local pseudo-random
   generator).

   The grace period is detected through a global age counter (ticket)
   that is incremented each time a thread is awaken / sleep.
   Each thread has its own age that is set to the global ticket on sleep/awaken.
   The age of the pool to be reclaimed is also set to this global age counter.

   To ensure that the grace period is finished, it tests if all threads
   are younger than the age of the pool to be reclaimed.

   From a performance point of view, this puts a bottleneck on the global
   age counter that is shared and incremented by all threads. However,
   the sleep/awaken operations are much less frequent than other operations.
   Thus, it shall not have a huge impact on the performance if the user
   code is intelligent with the sleep/awaken operations.

   As such it won't support more than ULONG_MAX sleep for all threads.
*/
#define M_CMEMP00L_DEF_LFMP_THREAD_MEMPOOL(name, type_t)                      \
                                                                              \
  typedef struct M_C(name, _lfmp_thread_s) {                                  \
    M_C(name, _slist_ct)  free;                                               \
    M_C(name, _slist_ct)  to_be_reclaimed;                                    \
    M_CACHELINE_ALIGN(align1, M_C(name, _slist_ct), M_C(name, _slist_ct));    \
  } M_C(name, _lfmp_thread_ct);                                               \
                                                                              \
  static inline void                                                          \
  M_C(name, _lfmp_thread_init)(M_C(name, _lfmp_thread_ct) *t)                 \
  {                                                                           \
    M_C(name, _slist_init)(t->free);                                          \
    M_C(name, _slist_init)(t->to_be_reclaimed);                               \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _lfmp_thread_clear)(M_C(name, _lfmp_thread_ct) *t)                \
  {                                                                           \
    M_ASSERT(M_C(name, _slist_empty_p)(t->to_be_reclaimed));                  \
    M_C(name, _slist_clear)(t->free);                                         \
    M_C(name, _slist_clear)(t->to_be_reclaimed);                              \
  }                                                                           \

/* NOTE: once a node is deleted, its data are kept readable until the future GC */
#define M_CMEMP00L_DEF_LF_MEMPOOL(name, type_t)                               \
                                                                              \
  typedef struct M_C(name, _s) {                                              \
    unsigned                  initial;                                        \
    M_C(name, _lfmp_thread_ct) *thread_data;                                  \
    M_C(name, _lflist_ct)      free;                                          \
    M_C(name, _lflist_ct)      to_be_reclaimed;                               \
    M_C(name, _lflist_ct)      empty;                                         \
    m_cmemp00l_list_ct       mempool_node;                                    \
    struct m_gc_s            *gc_mem;                                         \
  } M_C(name, _t)[1];                                                         \
                                                                              \
  /* Garbage collect of the nodes of the mempool on sleep */                  \
  static inline void                                                          \
  M_C3(m_cmemp00l_,name,_gc_on_sleep)(m_gc_t gc_mem, m_cmemp00l_list_ct *data, \
         m_gc_tid_t id, m_gc_ticket_ct ticket, m_gc_ticket_ct min_ticket)     \
  {                                                                           \
    /* Get back the mempool from the node */                                  \
    struct M_C(name, _s) *mempool =                                           \
      M_TYPE_FROM_FIELD(struct M_C(name, _s), data, m_cmemp00l_list_ct, mempool_node); \
                                                                              \
    /* Move the local nodes of the mempool to be reclaimed to the thread into the global pool */ \
    if (!M_C(name, _slist_empty_p)(mempool->thread_data[id].to_be_reclaimed)) { \
      M_C(name, _lf_node_t) *node;                                            \
      /* Get a new empty group of nodes */                                    \
      node = M_C(name, _lflist_pop)(mempool->empty, gc_mem->thread_data[id].bkoff); \
      if (M_UNLIKELY (node == NULL)) {                                        \
        /* Fail to get an empty group of node.                                \
           Alloc a new one from the system */                                 \
        node = M_C(name, _alloc_node)(0);                                     \
        M_ASSERT(node != NULL);                                               \
      }                                                                       \
      M_ASSERT(M_C(name, _slist_empty_p)(node->list));                        \
      M_C(name, _slist_move)(node->list, mempool->thread_data[id].to_be_reclaimed); \
      atomic_store_explicit(&node->cpt, ticket, memory_order_relaxed);        \
      M_C(name, _lflist_push)(mempool->to_be_reclaimed, node, gc_mem->thread_data[id].bkoff); \
    }                                                                         \
                                                                              \
    /* Perform a GC of the freelist of nodes */                               \
    while (true) {                                                            \
      M_C(name, _lf_node_t) *node;                                            \
      node = M_C(name, _lflist_pop_if)(mempool->to_be_reclaimed,              \
                                       min_ticket, gc_mem->thread_data[id].bkoff); \
      if (node == NULL) break;                                                \
      M_C(name, _lflist_push)(mempool->free, node, gc_mem->thread_data[id].bkoff); \
    }                                                                         \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _init)(M_C(name, _t) mem, m_gc_t gc_mem,                          \
                   unsigned init_node_count, unsigned init_group_count)       \
  {                                                                           \
    const size_t max_thread =  gc_mem->max_thread;                            \
    /* Initialize the thread data of the mempool */                           \
    mem->thread_data = M_MEMORY_REALLOC(M_C(name, _lfmp_thread_ct), NULL, max_thread); \
    if (mem->thread_data == NULL) {                                           \
      M_MEMORY_FULL(max_thread * sizeof(M_C(name, _lfmp_thread_ct)));         \
      return;                                                                 \
    }                                                                         \
    for(unsigned i = 0; i < max_thread;i++) {                                 \
      M_C(name, _lfmp_thread_init)(&mem->thread_data[i]);                     \
    }                                                                         \
    /* Preallocate some group of nodes for the mempool */                     \
    mem->initial = M_MAX(M_CMEMP00L_MIN_NODE_PER_GROUP, init_node_count);     \
    M_C(name, _lflist_init)(mem->free, M_C(name, _alloc_node)(init_node_count)); \
    M_C(name, _lflist_init)(mem->to_be_reclaimed, M_C(name, _alloc_node)(init_node_count)); \
    M_C(name, _lflist_init)(mem->empty, M_C(name, _alloc_node)(0));           \
    for(unsigned i = 1; i < init_group_count; i++) {                          \
      M_C(name, _lflist_push)(mem->free, M_C(name, _alloc_node)(init_node_count), \
                              gc_mem->thread_data[0].bkoff);                  \
      M_C(name, _lflist_push)(mem->empty, M_C(name, _alloc_node)(0),          \
                              gc_mem->thread_data[0].bkoff);                  \
    }                                                                         \
    /* Register the mempool in the GC */                                      \
    mem->mempool_node.gc_on_sleep = M_C3(m_cmemp00l_,name,_gc_on_sleep);      \
    mem->mempool_node.next = gc_mem->mempool_list;                            \
    gc_mem->mempool_list = &mem->mempool_node;                                \
    mem->gc_mem = gc_mem;                                                     \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _clear)(M_C(name, _t) mem)                                        \
  {                                                                           \
    const unsigned max_thread = mem->gc_mem->max_thread;                      \
    for(unsigned i = 0; i < max_thread;i++) {                                 \
      M_C(name, _lfmp_thread_clear)(&mem->thread_data[i]);                    \
    }                                                                         \
    M_MEMORY_FREE(mem->thread_data);                                          \
    mem->thread_data = NULL;                                                  \
    M_C(name, _lflist_clear)(mem->empty);                                     \
    M_C(name, _lflist_clear)(mem->free);                                      \
    M_ASSERT(M_C(name, _lflist_empty_p)(mem->to_be_reclaimed));               \
    M_C(name, _lflist_clear)(mem->to_be_reclaimed);                           \
    /* TODO: Unregister from the GC? */                                       \
  }                                                                           \
                                                                              \
  static inline type_t *                                                      \
  M_C(name, _new)(M_C(name, _t) mem, m_gc_tid_t id)                           \
  {                                                                           \
    M_C(name, _slist_node_ct) *snode;                                         \
    M_C(name, _lf_node_t) *node;                                              \
    while (true) {                                                            \
      /* Fast & likely path where we access the thread pool of nodes */       \
      if (M_LIKELY(!M_C(name, _slist_empty_p)(mem->thread_data[id].free))) {  \
        snode = M_C(name, _slist_pop)(mem->thread_data[id].free);             \
        return &snode->data;                                                  \
      }                                                                       \
      /* Request a group node to the freelist of groups */                    \
      node = M_C(name, _lflist_pop)(mem->free, mem->gc_mem->thread_data[id].bkoff); \
      if (M_UNLIKELY (node == NULL)) {                                        \
        /* Request a new group to the system. Non Lock Free path */           \
        M_ASSERT(mem->initial > 0);                                           \
        node = M_C(name, _alloc_node)(mem->initial);                          \
        M_ASSERT(node != NULL);                                               \
        M_ASSERT(!M_C(name, _slist_empty_p)(node->list));                     \
      }                                                                       \
      M_C(name, _slist_move)(mem->thread_data[id].free, node->list);          \
      /* Push back the empty group */                                         \
      M_ASSERT (M_C(name, _slist_empty_p)(node->list));                       \
      M_C(name, _lflist_push)(mem->empty, node, mem->gc_mem->thread_data[id].bkoff); \
    }                                                                         \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _del)(M_C(name, _t) mem, type_t *d, m_gc_tid_t id)                \
  {                                                                           \
    M_C(name, _slist_node_ct) *snode;                                         \
    M_ASSERT( d != NULL);                                                     \
    snode = M_TYPE_FROM_FIELD(M_C(name, _slist_node_ct), d, type_t, data);    \
    M_C(name, _slist_push)(mem->thread_data[id].to_be_reclaimed, snode);      \
  }                                                                           \


/***********************************************************************/

/* Define the ID of a thread */
typedef unsigned int m_gc_tid_t;

/* Define the age of a node */
/* TODO: Compute if sufficient (worst cast ULONG_MAX is 32 bits) */
typedef unsigned long m_gc_ticket_ct;
typedef atomic_ulong  m_gc_atomic_ticket_ct;

/* Define the Linked List of mempools that are registered in the GC */
struct m_gc_s;
typedef struct m_cmemp00l_list_s {
  struct m_cmemp00l_list_s *next;
  void (*gc_on_sleep)(struct m_gc_s *gc_mem,
                      struct m_cmemp00l_list_s *data, m_gc_tid_t id,
                      m_gc_ticket_ct ticket, m_gc_ticket_ct min_ticket);
  void *data;
} m_cmemp00l_list_ct;

/* Define the Garbage collector thread data */
typedef struct m_gc_lfmp_thread_s {
  m_gc_atomic_ticket_ct      ticket;
  m_core_backoff_ct          bkoff;
  M_CACHELINE_ALIGN(align1, atomic_ulong, m_core_backoff_ct);
} m_gc_lfmp_thread_ct;

/* Define the Garbage collector coordinator */
typedef struct m_gc_s {
  m_gc_atomic_ticket_ct      ticket;
  m_gc_tid_t                 max_thread;
  m_genint_t                 thread_alloc;
  m_gc_lfmp_thread_ct       *thread_data;
  m_cmemp00l_list_ct     *mempool_list;
} m_gc_t[1];

static inline void
m_gc_init(m_gc_t gc_mem, size_t max_thread)
{
  M_ASSERT(gc_mem != NULL);
  M_ASSERT(max_thread > 0 && max_thread < INT_MAX);

  atomic_init(&gc_mem->ticket, 0UL);
  m_genint_init(gc_mem->thread_alloc, (unsigned int) max_thread);
  gc_mem->thread_data = M_MEMORY_REALLOC(m_gc_lfmp_thread_ct, NULL, max_thread);
  if (gc_mem->thread_data == NULL) {
    M_MEMORY_FULL(max_thread * sizeof(m_gc_lfmp_thread_ct));
    return;
  }
  for(unsigned i = 0; i < max_thread;i++) {
    atomic_init(&gc_mem->thread_data[i].ticket, ULONG_MAX);
    m_core_backoff_init(gc_mem->thread_data[i].bkoff);
  }
  gc_mem->max_thread   = (unsigned int) max_thread;
  gc_mem->mempool_list = NULL;
}

static inline void
m_gc_clear(m_gc_t gc_mem)
{
  M_ASSERT(gc_mem != NULL && gc_mem->max_thread > 0);
  
  for(m_gc_tid_t i = 0; i < gc_mem->max_thread;i++) {
    m_core_backoff_clear(gc_mem->thread_data[i].bkoff);
  }
  M_MEMORY_FREE(gc_mem->thread_data);
  gc_mem->thread_data = NULL;
  m_genint_clear(gc_mem->thread_alloc);
}

static inline m_gc_tid_t
m_gc_attach_thread(m_gc_t gc_mem)
{
  M_ASSERT(gc_mem != NULL && gc_mem->max_thread > 0);
  
  unsigned id = m_genint_pop(gc_mem->thread_alloc);
  return M_ASSIGN_CAST(m_gc_tid_t, id);
}
  
static inline void
m_gc_detach_thread(m_gc_t gc_mem, m_gc_tid_t id)
{
  M_ASSERT(gc_mem != NULL && gc_mem->max_thread > 0);
  M_ASSERT(id < gc_mem->max_thread);
  M_ASSERT(atomic_load(&gc_mem->thread_data[id].ticket) == ULONG_MAX);

  m_genint_push(gc_mem->thread_alloc, id);
}
  
static inline void
m_gc_awake(m_gc_t gc_mem, m_gc_tid_t id)
{
  M_ASSERT(gc_mem != NULL && gc_mem->max_thread > 0);
  M_ASSERT(id < gc_mem->max_thread);
  M_ASSERT(atomic_load(&gc_mem->thread_data[id].ticket) == ULONG_MAX);

  m_gc_ticket_ct t = atomic_fetch_add(&gc_mem->ticket, 1UL) + 1;
  atomic_store(&gc_mem->thread_data[id].ticket, t);
}
  
static inline m_gc_ticket_ct
m_cmemp00l_gc_min_ticket(m_gc_t gc_mem)
{
  m_gc_ticket_ct min = atomic_load(&gc_mem->thread_data[0].ticket);
  for(m_gc_tid_t i = 1; i < gc_mem->max_thread; i++) {
    m_gc_ticket_ct t = atomic_load(&gc_mem->thread_data[i].ticket);
    min = M_MIN(t, min);
  }
  return min;
}

static inline void
m_gc_sleep(m_gc_t gc_mem, m_gc_tid_t id)
{
  /* Increase life time of the thread */
  m_gc_ticket_ct t = atomic_fetch_add(&gc_mem->ticket, 1UL);
  atomic_store(&gc_mem->thread_data[id].ticket, t+1);
  const m_gc_ticket_ct min_ticket = m_cmemp00l_gc_min_ticket(gc_mem);
  /* Iterate over all registered mempools */
  m_cmemp00l_list_ct *it = gc_mem->mempool_list;

  while (it) {
    /* Perform a garbage collect of the mempool */
    it->gc_on_sleep(gc_mem, it, id, t, min_ticket);
    /* Next mempool to scan for GC */
    it = it->next;
  }
  /* Sleep the thread */
  atomic_store(&gc_mem->thread_data[id].ticket, ULONG_MAX);
}


/***********************************************************************/
/*                                                                     */
/*                    Variable Length Array  MEMPOOL                   */
/*                                                                     */
/***********************************************************************/

M_CMEMP00L_DEF_SINGLY_LIST(m_vlapool, char)
M_CMEMP00L_DEF_LF_QUEUE(m_vlapool, char)
M_CMEMP00L_DEF_SYSTEM_ALLOC(m_vlapool, char)

typedef struct m_vlapool_lfmp_thread_s {
  m_vlapool_slist_ct  to_be_reclaimed;
  M_CACHELINE_ALIGN(align1, m_vlapool_slist_ct);
} m_vlapool_lfmp_thread_ct;

static inline void
m_vlapool_lfmp_thread_init(m_vlapool_lfmp_thread_ct *t)
{
  m_vlapool_slist_init(t->to_be_reclaimed);
}

static inline void
m_vlapool_lfmp_thread_clear(m_vlapool_lfmp_thread_ct *t)
{
  M_ASSERT(m_vlapool_slist_empty_p(t->to_be_reclaimed));
  m_vlapool_slist_clear(t->to_be_reclaimed);
}

typedef struct m_vlapool_s {
  m_vlapool_lflist_ct        to_be_reclaimed;
  m_vlapool_lflist_ct        empty;
  m_vlapool_lfmp_thread_ct  *thread_data;
  m_cmemp00l_list_ct      mvla_node;
  struct m_gc_s             *gc_mem;
} m_vlapool_t[1];

/* Garbage collect of the nodes of the vla mempool on sleep */
static inline void
m_cmemp00l_vlapool_on_sleep(m_gc_t gc_mem, m_cmemp00l_list_ct *data,
                          m_gc_tid_t id, m_gc_ticket_ct ticket, m_gc_ticket_ct min_ticket)
{
  /* Get back the mempool from the node */
  struct m_vlapool_s *vlapool =
    M_TYPE_FROM_FIELD(struct m_vlapool_s, data, m_cmemp00l_list_ct, mvla_node);

  /* Move the local nodes of the vlapool to be reclaimed to the thread into the global pool */
  if (!m_vlapool_slist_empty_p(vlapool->thread_data[id].to_be_reclaimed)) {
    m_vlapool_lf_node_t *node;
    /* Get a new empty group of nodes */
    node = m_vlapool_lflist_pop(vlapool->empty, gc_mem->thread_data[id].bkoff);
    if (M_UNLIKELY (node == NULL)) {
      /* Fail to get an empty group of node.
         Alloc a new one from the system */
      node = m_vlapool_alloc_node(0);
      M_ASSERT(node != NULL);
    }
    M_ASSERT(m_vlapool_slist_empty_p(node->list));
    m_vlapool_slist_move(node->list, vlapool->thread_data[id].to_be_reclaimed);
    atomic_store_explicit(&node->cpt, ticket, memory_order_relaxed);
    m_vlapool_lflist_push(vlapool->to_be_reclaimed, node, gc_mem->thread_data[id].bkoff);
  }

  /* Perform a GC of the freelist of nodes */
  while (true) {
    m_vlapool_lf_node_t *node;
    node = m_vlapool_lflist_pop_if(vlapool->to_be_reclaimed,
                                   min_ticket, gc_mem->thread_data[id].bkoff);
    if (node == NULL) break;
    // No reuse of VLA nodes. Free physically the node back to the system
    m_vlapool_slist_clear(node->list);
    // Add back the empty group of nodes
    m_vlapool_slist_init(node->list);
    m_vlapool_lflist_push(vlapool->empty, node, gc_mem->thread_data[id].bkoff);
  }
}

static inline void
m_vlapool_init(m_vlapool_t mem, m_gc_t gc_mem)
{
  const size_t max_thread =  gc_mem->max_thread;

  /* Initialize the thread data of the vlapool */
  mem->thread_data = M_MEMORY_REALLOC(m_vlapool_lfmp_thread_ct, NULL, max_thread);
  if (mem->thread_data == NULL) {
    M_MEMORY_FULL(max_thread * sizeof(m_vlapool_lfmp_thread_ct));
    return;
  }
  for(unsigned i = 0; i < max_thread;i++) {
    m_vlapool_lfmp_thread_init(&mem->thread_data[i]);
  }

  /* Initialize the lists */
  m_vlapool_lflist_init(mem->to_be_reclaimed, m_vlapool_alloc_node(0));
  m_vlapool_lflist_init(mem->empty, m_vlapool_alloc_node(0));

  /* Register the mempool in the GC */
  mem->mvla_node.gc_on_sleep = m_cmemp00l_vlapool_on_sleep;
  mem->mvla_node.next = gc_mem->mempool_list;
  gc_mem->mempool_list = &mem->mvla_node;
  mem->gc_mem = gc_mem;
}

static inline void
m_vlapool_clear(m_vlapool_t mem)
{
  const unsigned max_thread = mem->gc_mem->max_thread;
  for(unsigned i = 0; i < max_thread;i++) {
    m_vlapool_lfmp_thread_clear(&mem->thread_data[i]);
  }
  M_MEMORY_FREE(mem->thread_data);
  mem->thread_data = NULL;
  m_vlapool_lflist_clear(mem->empty);
  M_ASSERT(m_vlapool_lflist_empty_p(mem->to_be_reclaimed));
  m_vlapool_lflist_clear(mem->to_be_reclaimed);
  /* TODO: Unregister from the GC? */
}

static inline void *
m_vlapool_new(m_vlapool_t mem, m_gc_tid_t id, size_t size)
{
  M_ASSERT(mem != NULL && mem->gc_mem != NULL);
  M_ASSERT(id < mem->gc_mem->max_thread);
  M_ASSERT( atomic_load(&mem->gc_mem->thread_data[id].ticket) != ULONG_MAX);

  // Nothing to do with theses parameters yet
  (void) mem;
  (void) id;

  // Ensure the size is big enough to also represent a node
  size += offsetof(struct m_vlapool_slist_node_s, data);

  // Simply wrap around a system call to get the memory
  char *ptr = M_MEMORY_REALLOC(char, NULL, size);
  return (ptr == NULL) ? NULL : M_ASSIGN_CAST(void *, ptr + offsetof(struct m_vlapool_slist_node_s, data));
}

static inline void
m_vlapool_del(m_vlapool_t mem, void *d, m_gc_tid_t id)
{
  M_ASSERT(mem != NULL && mem->gc_mem != NULL);
  M_ASSERT(id < mem->gc_mem->max_thread);
  M_ASSERT(atomic_load(&mem->gc_mem->thread_data[id].ticket) != ULONG_MAX);
  M_ASSERT(d != NULL);

  // Get back the pointer to a struct m_vlapool_slist_node_s.
  d = M_ASSIGN_CAST(void *, M_ASSIGN_CAST(char *, d) - offsetof(struct m_vlapool_slist_node_s, data));
  m_vlapool_slist_node_ct *snode = M_ASSIGN_CAST(m_vlapool_slist_node_ct *, d);
  // Push the logicaly free memory into the list of the nodes to be reclaimed.
  m_vlapool_slist_push(mem->thread_data[id].to_be_reclaimed, snode);
}

M_END_PROTECTED_CODE

#if M_USE_SMALL_NAME
#define C_MEMPOOL_DEF M_C_MEMPOOL_DEF
#endif

#endif
