/*
 * M*LIB - Fixed size (Bounded) QUEUE & STACK interface
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
#ifndef MSTARLIB_BUFFER_H
#define MSTARLIB_BUFFER_H

#include "m-core.h"
#include "m-mutex.h"
#include "m-atomic.h"

/* Define the different kind of policy a lock-based buffer can have:
 * - the buffer can be either a queue (policy is FIFO) or a stack (policy is FILO),
 * - if the push method is by default blocking (waiting for the buffer to has some space) or not, *** deprecated ***
 * - if the pop method is by default blocking (waiting for the buffer to has some data) or not, *** deprecated ***
 * - if both methods are blocking, *** deprecated ***
 * - if it shall be thread safe or not (i.e. remove the mutex lock and atomic costs),
 * - if the buffer has to be init with empty elements, or if it shall init an element when it is pushed (and moved when popped),
 * - if the buffer has to overwrite the last element if the buffer is full,
 * - if the pop of an element is not complete until the call to pop_release (preventing push until this call).
 */
typedef enum {
  M_BUFFER_QUEUE = 0,    M_BUFFER_STACK = 1,
  M_BUFFER_BLOCKING_PUSH = 0, M_BUFFER_UNBLOCKING_PUSH = 2,
  M_BUFFER_BLOCKING_POP = 0, M_BUFFER_UNBLOCKING_POP = 4,
  M_BUFFER_BLOCKING = 0, M_BUFFER_UNBLOCKING = 6,
  M_BUFFER_THREAD_SAFE = 0, M_BUFFER_THREAD_UNSAFE = 8,
  M_BUFFER_PUSH_INIT_POP_MOVE = 16,
  M_BUFFER_PUSH_OVERWRITE = 32,
  M_BUFFER_DEFERRED_POP = 64
} m_buffer_policy_e;


/* Define a lock based buffer.
   If size is 0, then the size will only be defined at run-time when initializing the buffer,
   otherwise the size will be a compile time constant.
   USAGE: BUFFER_DEF(name, type, size_of_buffer_or_0, policy[, oplist]) */
#define M_BUFFER_DEF(name, type, m_size, ... )                                \
  M_BUFFER_DEF_AS(name, M_C(name, _t), type, m_size, __VA_ARGS__)


/* Define a lock based buffer
   as the provided type name_t.
   USAGE: BUFFER_DEF_AS(name, name_t, type, size_of_buffer_or_0, policy[, oplist of type]) */
#define M_BUFFER_DEF_AS(name, name_t, type, m_size, ... )                     \
  M_BEGIN_PROTECTED_CODE                                                      \
  M_BUFF3R_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                                 \
              ((name, type, m_size,__VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(type)(), name_t ), \
               (name, type, m_size,__VA_ARGS__,                                 name_t ))) \
  M_END_PROTECTED_CODE


/* Define the oplist of a lock based buffer given its name and its oplist.
   USAGE: BUFFER_OPLIST(name[, oplist of the type]) */
#define M_BUFFER_OPLIST(...)                                                  \
  M_BUFF3R_OPLIST_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                              \
                 ((__VA_ARGS__, M_BASIC_OPLIST),                              \
                  (__VA_ARGS__ )))


/* Define a nearly lock-free queue for Many Producer Many Consummer.
   Much faster than queue of BUFFER_DEF in heavy communication scenario
   but without any blocking features (this is let to the user).
   Size of created queue shall be a power of 2 and is defined at run-time.
   USAGE: QUEUE_MPMC_DEF(name, type, policy, [oplist of type])
*/
#define M_QUEUE_MPMC_DEF(name, type, ...)                                     \
  M_QUEUE_MPMC_DEF_AS(name, M_C(name,_t), type, __VA_ARGS__)


/* Define a nearly lock-free queue for Many Producer Many Consummer
   as the provided type name_t.
   Much faster than queue of BUFFER_DEF in heavy communication scenario
   but without any blocking features (this is let to the user).
   Size of created queue shall be a power of 2 and is defined at run-time.
   USAGE: QUEUE_MPMC_DEF_AS(name, name_t, type, policy, [oplist of type])
*/
#define M_QUEUE_MPMC_DEF_AS(name, name_t, type, ...)                          \
  M_BEGIN_PROTECTED_CODE                                                      \
  M_QU3UE_MPMC_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                             \
                  ((name, type, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(type)(), name_t ), \
                   (name, type, __VA_ARGS__,                                 name_t ))) \
  M_END_PROTECTED_CODE


/* Define a wait-free queue for Single Producer Single Consummer
   Much faster than queue of BUFFER_DEF or QUEUE_MPMC in heavy communication scenario
   but without any blocking features (this is let to the user).
   Size of created queue shall be a power of 2 and is defined at run-time.
   USAGE: QUEUE_SPSC_DEF(name, type, policy, [oplist of type])
*/
#define M_QUEUE_SPSC_DEF(name, type, ...)                                     \
  M_QUEUE_SPSC_DEF_AS(name, M_C(name, _t), type, __VA_ARGS__)


/* Define a wait-free queue for Single Producer Single Consummer
   as the provided type name_t.
   Much faster than queue of BUFFER_DEF in heavy communication scenario
   but without any blocking features (this is let to the user).
   Size of created queue shall be a power of 2 and is defined at run-time.
   USAGE: QUEUE_SPSC_DEF_AS(name, name_t, type, policy, [oplist of type])
*/
#define M_QUEUE_SPSC_DEF_AS(name, name_t, type, ...)                          \
  M_BEGIN_PROTECTED_CODE                                                      \
  M_QU3UE_SPSC_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                             \
                  ((name, type, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(type)(), name_t ), \
                   (name, type, __VA_ARGS__,                                 name_t ))) \
  M_END_PROTECTED_CODE



/*****************************************************************************/
/********************************** INTERNAL *********************************/
/*****************************************************************************/

/* Test if the given policy is true or not.
   WARNING: The policy shall be a non zero value (i.e. not a default). */
#define M_BUFF3R_POLICY_P(policy, val)                                        \
  (((policy) & (val)) != 0)

/* Handle either atomic integer or normal integer in function of the policy
   parameter of the buffer BUFFER_THREAD_UNSAFE (BUFFER_THREAD_SAFE is the
   default). This enables avoiding to pay the cost of atomic operations if not
   applicable.
 */
typedef union m_buff3r_number_s {
  unsigned int  u;
  atomic_uint   a;
#ifdef __cplusplus
  // Not sure why, but C++ needs an explicit default constructor for this union.
  m_buff3r_number_s() : u(0) {};
#endif
} m_buff3r_number_ct[1];

static inline void
m_buff3r_number_init(m_buff3r_number_ct n, unsigned int policy)
{
  if (!M_BUFF3R_POLICY_P(policy, M_BUFFER_THREAD_UNSAFE))
    atomic_init(&n->a, 0U);
  else
    n->u = 0UL;
}

static inline unsigned int
m_buff3r_number_load(m_buff3r_number_ct n, unsigned int policy)
{
  if (!M_BUFF3R_POLICY_P(policy, M_BUFFER_THREAD_UNSAFE))
    // Perform a memory acquire so that further usage of the buffer
    // is synchronized.
    return atomic_load_explicit(&n->a, memory_order_acquire);
  else
    return n->u;
}

static inline void
m_buff3r_number_store(m_buff3r_number_ct n, unsigned int v, unsigned int policy)
{
  if (!M_BUFF3R_POLICY_P(policy, M_BUFFER_THREAD_UNSAFE))
    // This function is used in context where a relaxed access is sufficient.
    atomic_store_explicit(&n->a, v, memory_order_relaxed);
  else
    n->u = v;
}

static inline void
m_buff3r_number_set(m_buff3r_number_ct n, m_buff3r_number_ct v, unsigned int policy)
{
  if (!M_BUFF3R_POLICY_P(policy, M_BUFFER_THREAD_UNSAFE))
    // This function is used in context where a relaxed access is sufficient.
    atomic_store_explicit(&n->a, atomic_load_explicit(&v->a, memory_order_relaxed), memory_order_relaxed);
  else
    n->u = v->u;
}

static inline unsigned int
m_buff3r_number_inc(m_buff3r_number_ct n, unsigned int policy)
{
  if (!M_BUFF3R_POLICY_P(policy, M_BUFFER_THREAD_UNSAFE))
    return atomic_fetch_add(&n->a, 1U);
  else
    return n->u ++;
}

static inline unsigned int
m_buff3r_number_dec(m_buff3r_number_ct n, unsigned int policy)
{
  if (!M_BUFF3R_POLICY_P(policy, M_BUFFER_THREAD_UNSAFE))
    return atomic_fetch_sub(&n->a, 1U);
  else
    return n->u --;
}

/*****************************************************************************/

/* Test if the size is only run-time or build time */
#define M_BUFF3R_IF_CTE_SIZE(m_size)  M_IF(M_BOOL(m_size))

/* Return the size (run time or build time).
   NOTE: It assumed that the buffer variable name is 'v' */
#define M_BUFF3R_SIZE(m_size)                                                 \
  M_BUFF3R_IF_CTE_SIZE(m_size) (m_size, v->size)

/* Contract of a buffer.
   Nothing particular since we cannot test much without locking it.
*/
#define M_BUFF3R_CONTRACT(buffer, size)        do {                           \
    M_ASSERT (buffer != NULL);                                                \
    M_ASSERT (buffer->data != NULL);                                          \
  }while (0)

/* Contract of a buffer within a protected section */
#define M_BUFF3R_PROTECTED_CONTRACT(policy, buffer, size) do {                \
    M_ASSERT (m_buff3r_number_load(buffer->number[0], policy) <= M_BUFF3R_SIZE(size)); \
  } while (0)

/* Deferred evaluation for the definition,
   so that all arguments are evaluated before further expansion */
#define M_BUFF3R_DEF_P1(arg) M_ID( M_BUFF3R_DEF_P2 arg )

/* Validate the value oplist before going further */
#define M_BUFF3R_DEF_P2(name, type, m_size, policy, oplist, buffer_t)         \
  M_IF_OPLIST(oplist)(M_BUFF3R_DEF_P3, M_BUFF3R_DEF_FAILURE)(name, type, m_size, policy, oplist, buffer_t)

/* Stop processing with a compilation failure */
#define M_BUFF3R_DEF_FAILURE(name, type, m_size, policy, oplist, buffer_t)    \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(M_BUFFER_DEF): the given argument is not a valid oplist: " M_AS_STR(oplist))

/* Define the buffer type using mutex lock and its functions.
  - name: main prefix of the container
  - type: type of an element of the buffer
  - m_size: constant to 0 if variable runtime size, or else the fixed size of the buffer
  - policy: the policy of the buffer
  - oplist: the oplist of the type of an element of the buffer
  - buffer_t: name of the buffer
  */
#define M_BUFF3R_DEF_P3(name, type, m_size, policy, oplist, buffer_t)         \
                                                                              \
  /* Put each data in a separate cache line to avoid false sharing            \
     by multiple writing threads. No need to align if there is no thread */   \
  typedef union M_C(name, _el_s) {                                            \
    type x;                                                                   \
    char align[M_BUFF3R_POLICY_P(policy, M_BUFFER_THREAD_UNSAFE) ? 1 : M_ALIGN_FOR_CACHELINE_EXCLUSION]; \
  } M_C(name, _el_ct);                                                        \
                                                                              \
  typedef struct M_C(name, _s) {                                              \
    /* Data for a producer */                                                 \
    m_mutex_t mutexPush;    /* MUTEX used for pushing elements */             \
    size_t    idx_prod;     /* Index of the production threads  */            \
    size_t    overwrite;    /* Number of overwritten values */                \
    m_cond_t there_is_data; /* condition raised when there is data */         \
    /* Read only Data */                                                      \
    M_BUFF3R_IF_CTE_SIZE(m_size)( ,size_t size;) /* Size of the buffer */     \
    /* Data for a consummer */                                                \
    m_cond_t there_is_room_for_data; /* Cond. raised when there is room */    \
    m_mutex_t mutexPop;     /* MUTEX used for popping elements */             \
    size_t    idx_cons;     /* Index of the consumption threads */            \
    /* number[0] := Number of elements in the buffer */                       \
    /* number[1] := [OPTION] Number of elements being deferred in the buffer */ \
    m_buff3r_number_ct number[1 + M_BUFF3R_POLICY_P(policy, M_BUFFER_DEFERRED_POP)]; \
    /* If fixed size, array of elements, otherwise pointer to element */      \
    M_C(name, _el_ct)  M_BUFF3R_IF_CTE_SIZE(m_size)(data[m_size], *data);     \
  } buffer_t[1];                                                              \
                                                                              \
  typedef struct M_C(name, _s) *M_C(name, _ptr);                              \
  typedef const struct M_C(name, _s) *M_C(name, _srcptr);                     \
  /* Internal type used to unconst the buffer */                              \
  typedef union { M_C(name, _srcptr) cptr; M_C(name, _ptr) ptr; } M_C(name, _uptr_ct); \
  /* Internal types used by the oplist */                                     \
  typedef type M_C(name, _subtype_ct);                                        \
  typedef buffer_t M_C(name, _ct);                                            \
                                                                              \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, type, oplist)                            \
                                                                              \
static inline void                                                            \
M_C(name, _init)(buffer_t v, size_t size)                                     \
{                                                                             \
  M_BUFF3R_IF_CTE_SIZE(m_size)(M_ASSERT(size == m_size), v->size = size);     \
  M_ASSERT(size <= UINT_MAX);                                                 \
  v->idx_prod = v->idx_cons = v->overwrite = 0;                               \
  m_buff3r_number_init (v->number[0], policy);                                \
  if (M_BUFF3R_POLICY_P(policy, M_BUFFER_DEFERRED_POP))                       \
    m_buff3r_number_init (v->number[1], policy);                              \
  if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_THREAD_UNSAFE)) {                 \
    m_mutex_init(v->mutexPush);                                               \
    m_mutex_init(v->mutexPop);                                                \
    m_cond_init(v->there_is_data);                                            \
    m_cond_init(v->there_is_room_for_data);                                   \
  } else {                                                                    \
    M_ASSERT(M_BUFF3R_POLICY_P((policy), M_BUFFER_UNBLOCKING));               \
  }                                                                           \
                                                                              \
  M_BUFF3R_IF_CTE_SIZE(m_size)( /* Statically allocated */ ,                  \
    v->data = M_CALL_REALLOC(oplist, M_C(name, _el_ct), NULL, M_BUFF3R_SIZE(m_size)); \
    if (v->data == NULL) {                                                    \
      M_MEMORY_FULL (M_BUFF3R_SIZE(m_size)*sizeof(M_C(name, _el_ct)));        \
      return;                                                                 \
    }                                                                         \
  )                                                                           \
  if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_INIT_POP_MOVE)) {            \
    for(size_t i = 0; i < size; i++) {                                        \
      M_CALL_INIT(oplist, v->data[i].x);                                      \
    }                                                                         \
  }                                                                           \
  M_BUFF3R_CONTRACT(v,m_size);                                                \
}                                                                             \
                                                                              \
 M_BUFF3R_IF_CTE_SIZE(m_size)(                                                \
 static inline void                                                           \
 M_C3(m_buff3r_,name,_init)(buffer_t v)                                       \
 {                                                                            \
   M_C(name, _init)(v, m_size);                                               \
 }                                                                            \
 , )                                                                          \
                                                                              \
 static inline void                                                           \
 M_C3(m_buff3r_,name,_clear_obj)(buffer_t v)                                  \
 {                                                                            \
   M_BUFF3R_CONTRACT(v,m_size);                                               \
   if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_INIT_POP_MOVE)) {           \
     for(size_t i = 0; i < M_BUFF3R_SIZE(m_size); i++) {                      \
       M_CALL_CLEAR(oplist, v->data[i].x);                                    \
     }                                                                        \
   } else {                                                                   \
     size_t i = M_BUFF3R_POLICY_P((policy), M_BUFFER_STACK) ? 0 : v->idx_cons; \
     while (i != v->idx_prod) {                                               \
       M_CALL_CLEAR(oplist, v->data[i].x);                                    \
       i++;                                                                   \
       if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_STACK) && i >= M_BUFF3R_SIZE(m_size)) \
         i = 0;                                                               \
     }                                                                        \
   }                                                                          \
   v->idx_prod = v->idx_cons = 0;                                             \
   m_buff3r_number_store (v->number[0], 0U, policy);                          \
   if (M_BUFF3R_POLICY_P(policy, M_BUFFER_DEFERRED_POP))                      \
     m_buff3r_number_store(v->number[1], 0U, policy);                         \
   M_BUFF3R_CONTRACT(v,m_size);                                               \
 }                                                                            \
                                                                              \
 static inline void                                                           \
 M_C(name, _clear)(buffer_t v)                                                \
 {                                                                            \
   M_BUFF3R_CONTRACT(v,m_size);                                               \
   M_C3(m_buff3r_,name,_clear_obj)(v);                                        \
   M_BUFF3R_IF_CTE_SIZE(m_size)( ,                                            \
     M_CALL_FREE(oplist, v->data);                                            \
     v->data = NULL;                                                          \
   )                                                                          \
   v->overwrite = 0;                                                          \
   if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_THREAD_UNSAFE)) {                \
     m_mutex_clear(v->mutexPush);                                             \
     m_mutex_clear(v->mutexPop);                                              \
     m_cond_clear(v->there_is_data);                                          \
     m_cond_clear(v->there_is_room_for_data);                                 \
   }                                                                          \
 }                                                                            \
                                                                              \
 static inline void                                                           \
 M_C(name, _reset)(buffer_t v)                                                \
 {                                                                            \
   M_BUFF3R_CONTRACT(v,m_size);                                               \
   if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_THREAD_UNSAFE)) {                \
     m_mutex_lock(v->mutexPush);                                              \
     m_mutex_lock(v->mutexPop);                                               \
   }                                                                          \
   M_BUFF3R_PROTECTED_CONTRACT(policy, v, m_size);                            \
   if (M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_INIT_POP_MOVE))              \
     M_C3(m_buff3r_,name,_clear_obj)(v);                                      \
   v->idx_prod = v->idx_cons = 0;                                             \
   m_buff3r_number_store (v->number[0], 0U, policy);                          \
   if (M_BUFF3R_POLICY_P(policy, M_BUFFER_DEFERRED_POP))                      \
     m_buff3r_number_store(v->number[1], 0U, policy);                         \
   if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_THREAD_UNSAFE)) {                \
     m_cond_broadcast(v->there_is_room_for_data);                             \
     m_mutex_unlock(v->mutexPop);                                             \
     m_mutex_unlock(v->mutexPush);                                            \
   }                                                                          \
   M_BUFF3R_CONTRACT(v,m_size);                                               \
 }                                                                            \
                                                                              \
 static inline void M_ATTR_DEPRECATED                                         \
 M_C(name, _clean)(buffer_t v)                                                \
 {                                                                            \
   M_C(name,_reset)(v);                                                       \
 }                                                                            \
                                                                              \
 static inline void                                                           \
 M_C(name, _init_set)(buffer_t dest, const buffer_t src)                      \
 {                                                                            \
   /* unconst 'src', so that we can lock it (semantically it is const) */     \
   M_C(name, _uptr_ct) vu;                                                    \
   vu.cptr = src;                                                             \
   M_C(name, _ptr) v = vu.ptr;                                                \
   M_ASSERT (dest != v);                                                      \
   M_BUFF3R_CONTRACT(v,m_size);                                               \
   M_C(name, _init)(dest, M_BUFF3R_SIZE(m_size));                             \
   if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_THREAD_UNSAFE)) {                \
     m_mutex_lock(v->mutexPush);                                              \
     m_mutex_lock(v->mutexPop);                                               \
   }                                                                          \
                                                                              \
   M_BUFF3R_PROTECTED_CONTRACT(policy, v, m_size);                            \
   if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_INIT_POP_MOVE)) {           \
     for(size_t i = 0; i < M_BUFF3R_SIZE(m_size); i++) {                      \
       M_CALL_INIT_SET(oplist, dest->data[i].x, v->data[i].x);                \
     }                                                                        \
   } else {                                                                   \
     size_t i = M_BUFF3R_POLICY_P((policy), M_BUFFER_STACK) ? 0 : v->idx_cons; \
     while (i != v->idx_prod) {                                               \
       M_CALL_INIT_SET(oplist, dest->data[i].x, v->data[i].x);                \
       i++;                                                                   \
       if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_STACK) && i >= M_BUFF3R_SIZE(m_size)) \
         i = 0;                                                               \
     }                                                                        \
   }                                                                          \
                                                                              \
   dest->idx_prod = v->idx_prod;                                              \
   dest->idx_cons = v->idx_cons;                                              \
   m_buff3r_number_set (dest->number[0], v->number[0], policy);               \
   if (M_BUFF3R_POLICY_P(policy, M_BUFFER_DEFERRED_POP))                      \
     m_buff3r_number_set(dest->number[1], v->number[1], policy);              \
                                                                              \
   if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_THREAD_UNSAFE)) {                \
     m_mutex_unlock(v->mutexPop);                                             \
     m_mutex_unlock(v->mutexPush);                                            \
   }                                                                          \
   M_BUFF3R_CONTRACT(v,m_size);                                               \
   M_BUFF3R_CONTRACT(dest, m_size);                                           \
 }                                                                            \
                                                                              \
 static inline void                                                           \
 M_C(name, _set)(buffer_t dest, const buffer_t src)                           \
 {                                                                            \
   /* unconst 'src', so that we can lock it (semantically it is const) */     \
   M_C(name, _uptr_ct) vu;                                                    \
   vu.cptr = src;                                                             \
   M_C(name, _ptr) v = vu.ptr;                                                \
   M_BUFF3R_CONTRACT(dest,m_size);                                            \
   M_BUFF3R_CONTRACT(v,m_size);                                               \
                                                                              \
   if (dest == v) return;                                                     \
   if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_THREAD_UNSAFE)) {                \
     /* Case of deadlock: A := B, B:=C, C:=A (all in //)                      \
        Solution: order the lock by increasing memory */                      \
     if (dest < v) {                                                          \
       m_mutex_lock(dest->mutexPush);                                         \
       m_mutex_lock(dest->mutexPop);                                          \
       m_mutex_lock(v->mutexPush);                                            \
       m_mutex_lock(v->mutexPop);                                             \
     } else {                                                                 \
       m_mutex_lock(v->mutexPush);                                            \
       m_mutex_lock(v->mutexPop);                                             \
       m_mutex_lock(dest->mutexPush);                                         \
       m_mutex_lock(dest->mutexPop);                                          \
     }                                                                        \
   }                                                                          \
                                                                              \
   M_BUFF3R_PROTECTED_CONTRACT(policy, v, m_size);                            \
   M_C3(m_buff3r_,name,_clear_obj)(dest);                                     \
                                                                              \
   if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_INIT_POP_MOVE)) {           \
     for(size_t i = 0; i < M_BUFF3R_SIZE(m_size); i++) {                      \
       M_CALL_INIT_SET(oplist, dest->data[i].x, v->data[i].x);                \
     }                                                                        \
   } else {                                                                   \
     size_t i = M_BUFF3R_POLICY_P((policy), M_BUFFER_STACK) ? 0 : v->idx_cons; \
     while (i != v->idx_prod) {                                               \
       M_CALL_INIT_SET(oplist, dest->data[i].x, v->data[i].x);                \
       i++;                                                                   \
       if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_STACK) && i >= M_BUFF3R_SIZE(m_size)) \
         i = 0;                                                               \
     }                                                                        \
   }                                                                          \
                                                                              \
   dest->idx_prod = v->idx_prod;                                              \
   dest->idx_cons = v->idx_cons;                                              \
   m_buff3r_number_set (dest->number[0], v->number[0], policy);               \
   if (M_BUFF3R_POLICY_P(policy, M_BUFFER_DEFERRED_POP))                      \
     m_buff3r_number_set(dest->number[1], v->number[1], policy);              \
                                                                              \
   if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_THREAD_UNSAFE)) {                \
     /* It may be false, but it is not wrong! */                              \
     m_cond_broadcast(v->there_is_room_for_data);                             \
     m_cond_broadcast(v->there_is_data);                                      \
     if (dest < v) {                                                          \
       m_mutex_unlock(v->mutexPop);                                           \
       m_mutex_unlock(v->mutexPush);                                          \
       m_mutex_unlock(dest->mutexPop);                                        \
       m_mutex_unlock(dest->mutexPush);                                       \
     } else {                                                                 \
       m_mutex_unlock(dest->mutexPop);                                        \
       m_mutex_unlock(dest->mutexPush);                                       \
       m_mutex_unlock(v->mutexPop);                                           \
       m_mutex_unlock(v->mutexPush);                                          \
     }                                                                        \
   }                                                                          \
   M_BUFF3R_CONTRACT(v,m_size);                                               \
   M_BUFF3R_CONTRACT(dest, m_size);                                           \
 }                                                                            \
                                                                              \
 static inline bool                                                           \
 M_C(name, _empty_p)(buffer_t v)                                              \
 {                                                                            \
   M_BUFF3R_CONTRACT(v,m_size);                                               \
   /* If the buffer has been configured with deferred pop                     \
      we considered the queue as empty when the number of                     \
      deferred pop has reached 0, not the number of items in the              \
      buffer is 0. */                                                         \
   if (M_BUFF3R_POLICY_P(policy, M_BUFFER_DEFERRED_POP))                      \
     return m_buff3r_number_load (v->number[1], policy) == 0;                 \
   else                                                                       \
     return m_buff3r_number_load (v->number[0], policy) == 0;                 \
 }                                                                            \
                                                                              \
 static inline bool                                                           \
 M_C(name, _full_p)(buffer_t v)                                               \
 {                                                                            \
   M_BUFF3R_CONTRACT(v,m_size);                                               \
   return m_buff3r_number_load (v->number[0], policy)                         \
     == M_BUFF3R_SIZE(m_size);                                                \
 }                                                                            \
                                                                              \
 static inline size_t                                                         \
 M_C(name, _size)(buffer_t v)                                                 \
 {                                                                            \
   M_BUFF3R_CONTRACT(v,m_size);                                               \
   return m_buff3r_number_load (v->number[0], policy);                        \
 }                                                                            \
                                                                              \
 static inline bool                                                           \
 M_C(name, _push_blocking)(buffer_t v, type const data, bool blocking)        \
 {                                                                            \
   M_BUFF3R_CONTRACT(v,m_size);                                               \
                                                                              \
   /* Producer Mutex lock (mutex lock performs an acquire memory barrier) */  \
   if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_THREAD_UNSAFE)) {                \
     m_mutex_lock(v->mutexPush);                                              \
     while (!M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_OVERWRITE)             \
            && M_C(name, _full_p)(v)) {                                       \
       if (!blocking) {                                                       \
         m_mutex_unlock(v->mutexPush);                                        \
         return false;                                                        \
       }                                                                      \
       m_cond_wait(v->there_is_room_for_data, v->mutexPush);                  \
     }                                                                        \
   } else if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_OVERWRITE)           \
              && M_C(name, _full_p)(v))                                       \
     return false;                                                            \
   M_BUFF3R_PROTECTED_CONTRACT(policy, v, m_size);                            \
                                                                              \
   size_t previousSize, idx = v->idx_prod;                                    \
   /* INDEX computation if we have to overwrite the last element */           \
   if (M_UNLIKELY (M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_OVERWRITE)       \
                   && M_C(name, _full_p)(v))) {                               \
     v->overwrite++;                                                          \
     /* Let's overwrite the last element */                                   \
     /* Compute the index of the last push element */                         \
     idx--;                                                                   \
     if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_STACK)) {                      \
       idx = idx >= M_BUFF3R_SIZE(m_size) ? M_BUFF3R_SIZE(m_size)-1 : idx;    \
     }                                                                        \
     /* Update data in the buffer */                                          \
     M_CALL_SET(oplist, v->data[idx].x, data);                                \
     previousSize = M_BUFF3R_SIZE(m_size);                                    \
   } else {                                                                   \
     /* Add a new item in the buffer */                                       \
     /* PUSH data in the buffer */                                            \
     if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_INIT_POP_MOVE)) {         \
       M_CALL_SET(oplist, v->data[idx].x, data);                              \
     } else {                                                                 \
       M_CALL_INIT_SET(oplist, v->data[idx].x, data);                         \
     }                                                                        \
                                                                              \
     /* Increment production INDEX of the buffer */                           \
     idx++;                                                                   \
     if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_STACK)) {                      \
       idx = (idx == M_BUFF3R_SIZE(m_size)) ? 0 : idx;                        \
     }                                                                        \
     v->idx_prod = idx;                                                       \
                                                                              \
     /* number[] is the only variable which can be modified by both           \
        the consummer thread which has the pop lock and the producer          \
        thread which has the push lock. As such, it is an atomic variable     \
        that performs a release memory barrier. */                            \
     /* Increment number of elements of the buffer */                         \
     previousSize = m_buff3r_number_inc (v->number[0], policy);               \
     if (M_BUFF3R_POLICY_P((policy), M_BUFFER_DEFERRED_POP)) {                \
       previousSize = m_buff3r_number_inc (v->number[1], policy);             \
     }                                                                        \
     /* From this point, consummer may read the data in the table */          \
   }                                                                          \
                                                                              \
   /* Producer unlock (mutex unlock performs a release memory barrier) */     \
   if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_THREAD_UNSAFE)) {                \
     m_mutex_unlock(v->mutexPush);                                            \
     /* If the number of items in the buffer was 0, some consummer            \
        may be waiting. Signal to them the availibility of the data           \
        We cannot only signal one thread. */                                  \
     if (previousSize == 0) {                                                 \
       m_mutex_lock(v->mutexPop);                                             \
       m_cond_broadcast(v->there_is_data);                                    \
       m_mutex_unlock(v->mutexPop);                                           \
     }                                                                        \
   }                                                                          \
                                                                              \
   M_BUFF3R_CONTRACT(v,m_size);                                               \
   return true;                                                               \
 }                                                                            \
                                                                              \
 static inline bool                                                           \
 M_C(name, _pop_blocking)(type *data, buffer_t v, bool blocking)              \
 {                                                                            \
   M_BUFF3R_CONTRACT(v,m_size);                                               \
   M_ASSERT (data != NULL);                                                   \
                                                                              \
   /* Consummer lock (mutex lock performs an acquire memory barrier) */       \
   if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_THREAD_UNSAFE)) {                \
     m_mutex_lock(v->mutexPop);                                               \
     while (M_C(name, _empty_p)(v)) {                                         \
       if (!blocking) {                                                       \
         m_mutex_unlock(v->mutexPop);                                         \
         return false;                                                        \
       }                                                                      \
       m_cond_wait(v->there_is_data, v->mutexPop);                            \
     }                                                                        \
   } else if (M_C(name, _empty_p)(v))                                         \
     return false;                                                            \
   M_BUFF3R_PROTECTED_CONTRACT(policy, v, m_size);                            \
                                                                              \
   /* POP data from the buffer and update INDEX */                            \
   if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_STACK)) {                        \
     /* FIFO queue */                                                         \
     if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_INIT_POP_MOVE)) {         \
       M_CALL_SET(oplist, *data, v->data[v->idx_cons].x);                     \
     } else {                                                                 \
       M_DO_INIT_MOVE (oplist, *data, v->data[v->idx_cons].x);                \
     }                                                                        \
     v->idx_cons = (v->idx_cons == M_BUFF3R_SIZE(m_size)-1) ? 0 : (v->idx_cons + 1); \
   } else {                                                                   \
     /* STACK queue */                                                        \
     v->idx_prod --;                                                          \
     if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_INIT_POP_MOVE)) {         \
       M_CALL_SET(oplist, *data, v->data[v->idx_prod].x);                     \
     } else {                                                                 \
       M_DO_INIT_MOVE (oplist, *data, v->data[v->idx_prod].x);                \
     }                                                                        \
   }                                                                          \
                                                                              \
   /* number[] is the only variable which can be modified by both             \
      the consummer thread which has the pop lock and the producer            \
      thread which has the push lock. As such, it is an atomic variable       \
      that performs a release memory barrier. */                              \
   /* Decrement number of elements in the buffer */                           \
   size_t previousSize;                                                       \
   if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_DEFERRED_POP)) {                 \
     previousSize = m_buff3r_number_dec (v->number[0], policy);               \
   } else {                                                                   \
     m_buff3r_number_dec (v->number[1], policy);                              \
   }                                                                          \
   /* Space may be reused by a producer thread from this point */             \
                                                                              \
   /* Consummer unlock (mutex unlock perfoms a release memory barrier) */     \
   if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_THREAD_UNSAFE)) {                \
     m_mutex_unlock(v->mutexPop);                                             \
     /* If the number of items in the buffer was the max, some producer       \
        may be waiting. Signal to them the availibility of the free room      \
        We cannot only signal one thread. */                                  \
     if ((!M_BUFF3R_POLICY_P((policy), M_BUFFER_DEFERRED_POP))                \
         && previousSize == M_BUFF3R_SIZE(m_size)) {                          \
       m_mutex_lock(v->mutexPush);                                            \
       m_cond_broadcast(v->there_is_room_for_data);                           \
       m_mutex_unlock(v->mutexPush);                                          \
     }                                                                        \
   }                                                                          \
                                                                              \
   M_BUFF3R_CONTRACT(v,m_size);                                               \
   return true;                                                               \
 }                                                                            \
                                                                              \
                                                                              \
 static inline bool                                                           \
 M_C(name, _push)(buffer_t v, type const data)                                \
 {                                                                            \
   return M_C(name, _push_blocking)(v, data,                                  \
                             !M_BUFF3R_POLICY_P((policy), M_BUFFER_UNBLOCKING_PUSH)); \
 }                                                                            \
                                                                              \
 static inline bool                                                           \
 M_C(name, _pop)(type *data, buffer_t v)                                      \
 {                                                                            \
   return M_C(name, _pop_blocking)(data, v,                                   \
                            !M_BUFF3R_POLICY_P((policy), M_BUFFER_UNBLOCKING_POP)); \
 }                                                                            \
                                                                              \
 static inline size_t                                                         \
 M_C(name, _overwrite)(const buffer_t v)                                      \
 {                                                                            \
   return v->overwrite;                                                       \
 }                                                                            \
                                                                              \
 static inline size_t                                                         \
 M_C(name, _capacity)(const buffer_t v)                                       \
 {                                                                            \
   (void) v; /* may be unused */                                              \
   return M_BUFF3R_SIZE(m_size);                                              \
 }                                                                            \
                                                                              \
 static inline void                                                           \
 M_C(name, _pop_release)(buffer_t v)                                          \
 {                                                                            \
   /* Decrement the effective number of elements in the buffer */             \
   if (M_BUFF3R_POLICY_P((policy), M_BUFFER_DEFERRED_POP)) {                  \
     size_t previousSize = m_buff3r_number_dec (v->number[0], policy);        \
     if (previousSize == M_BUFF3R_SIZE(m_size)) {                             \
       m_mutex_lock(v->mutexPush);                                            \
       m_cond_broadcast(v->there_is_room_for_data);                           \
       m_mutex_unlock(v->mutexPush);                                          \
     }                                                                        \
   }                                                                          \
 }                                                                            \



/* Definition of a a QUEUE for Many Produccer / Many Consummer
   for high bandwidth scenario:
   * nearly lock-free,
   * quite fast
   * no blocking calls.
   * only queue (no stack)
   * size of queue is always a power of 2
   * no overwriting.
   */

/* Deferred evaluation for the definition,
   so that all arguments are evaluated before further expansion */
#define M_QU3UE_MPMC_DEF_P1(arg) M_ID( M_QU3UE_MPMC_DEF_P2 arg )

/* Validate the value oplist before going further */
#define M_QU3UE_MPMC_DEF_P2(name, type, policy, oplist, buffer_t)             \
  M_IF_OPLIST(oplist)(M_QU3UE_MPMC_DEF_P3, M_QU3UE_MPMC_DEF_FAILURE)(name, type, policy, oplist, buffer_t)

/* Stop processing with a compilation failure */
#define M_QU3UE_MPMC_DEF_FAILURE(name, type, policy, oplist, buffer_t)        \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(QUEUE_MPMC_DEF): the given argument is not a valid oplist: " M_AS_STR(oplist))

#ifdef NDEBUG
# define M_QU3UE_MPMC_CONTRACT(v) /* nothing */
#else
# define M_QU3UE_MPMC_CONTRACT(v) do {                                        \
    M_ASSERT (v != 0);                                                        \
    M_ASSERT (v->Tab != NULL);                                                \
    unsigned int _r = atomic_load(&v->ConsoIdx);                              \
    unsigned int _w = atomic_load(&v->ProdIdx);                               \
    _r = atomic_load(&v->ConsoIdx);                                           \
    M_ASSERT (_r > _w || _w-_r <= v->size);                                   \
    M_ASSERT (M_POWEROF2_P(v->size));                                         \
  } while (0)
#endif


/* Define the buffer type MPMC using atomics and its functions.
  - name: main prefix of the container
  - type: type of an element of the buffer
  - policy: the policy of the buffer
  - oplist: the oplist of the type of an element of the buffer
  - buffer_t: name of the buffer
  */
#define M_QU3UE_MPMC_DEF_P3(name, type, policy, oplist, buffer_t)             \
                                                                              \
  /* The sequence number of an element will be equal to either                \
     - 2* the index of the production which creates it,                       \
     - 1 + 2* the index of the consumption which consummes it                 \
     In case of wrapping, as there is no order comparison but only            \
     equal comparison, there is no special issue.                             \
     Each element is put in a separate cache line to avoid false              \
     sharing by multiple writing threads.                                     \
  */                                                                          \
  typedef struct M_C(name, _el_s) {                                           \
    atomic_uint  seq;        /* Can only increase until wrapping */           \
    type         x;                                                           \
    M_CACHELINE_ALIGN(align, atomic_uint, type);                              \
  } M_C(name, _el_ct);                                                        \
                                                                              \
  /* If there is only one producer and one consummer, then they won't         \
     typically use the same cache line, increasing performance. */            \
  typedef struct M_C(name, _s) {                                              \
    atomic_uint ProdIdx; /* Can only increase until wrapping */               \
    M_CACHELINE_ALIGN(align1, atomic_uint);                                   \
    atomic_uint ConsoIdx; /* Can only increase until wrapping */              \
    M_CACHELINE_ALIGN(align2, atomic_uint);                                   \
    M_C(name, _el_ct) *Tab;                                                   \
    unsigned int size;                                                        \
  } buffer_t[1];                                                              \
                                                                              \
  typedef type M_C(name, _subtype_ct);                                        \
  typedef buffer_t M_C(name, _ct);                                            \
                                                                              \
  static inline bool                                                          \
  M_C(name, _push)(buffer_t table, type const x)                              \
  {                                                                           \
    M_QU3UE_MPMC_CONTRACT(table);                                             \
    unsigned int idx = atomic_load_explicit(&table->ProdIdx,                  \
                                            memory_order_relaxed);            \
    const unsigned int i = idx & (table->size -1);                            \
    const unsigned int seq = atomic_load_explicit(&table->Tab[i].seq,         \
                                                  memory_order_acquire);      \
    if (M_UNLIKELY (2*(idx - table->size) + 1 != seq))        {               \
      /* Buffer full (or unlikely preemption). Can not push */                \
      return false;                                                           \
    }                                                                         \
    if (M_UNLIKELY (!atomic_compare_exchange_strong_explicit(&table->ProdIdx, \
           &idx, idx+1, memory_order_relaxed, memory_order_relaxed))) {       \
      /* Thread has been preempted by another one. */                         \
      return false;                                                           \
    }                                                                         \
    /* If it is interrupted here, it may block all pop methods (not push)     \
       even if there is other threads that have pushed data later in the      \
       queue as all pop threads will try to enqueue this particular element   \
       but always fail. The won't try to enqueue other elements.              \
       As such, this queue is not strictly lock-free.*/                       \
    if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_INIT_POP_MOVE)) {          \
      M_CALL_SET(oplist, table->Tab[i].x, x);                                 \
    } else {                                                                  \
      M_CALL_INIT_SET(oplist, table->Tab[i].x, x);                            \
    }                                                                         \
    /* Finish transaction */                                                  \
    atomic_store_explicit(&table->Tab[i].seq, 2*idx, memory_order_release);   \
    M_QU3UE_MPMC_CONTRACT(table);                                             \
    return true;                                                              \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_C(name, _pop)(type *ptr, buffer_t table)                                  \
  {                                                                           \
    M_QU3UE_MPMC_CONTRACT(table);                                             \
    M_ASSERT (ptr != NULL);                                                   \
    unsigned int iC = atomic_load_explicit(&table->ConsoIdx,                  \
                                           memory_order_relaxed);             \
    const unsigned int i = (iC & (table->size -1));                           \
    const unsigned int seq = atomic_load_explicit(&table->Tab[i].seq,         \
                                                  memory_order_acquire);      \
    if (seq != 2 * iC) {                                                      \
      /* Nothing in buffer to consumme (or unlikely preemption) */            \
      return false;                                                           \
    }                                                                         \
    if (M_UNLIKELY (!atomic_compare_exchange_strong_explicit(&table->ConsoIdx, \
             &iC, iC+1, memory_order_relaxed, memory_order_relaxed))) {       \
      /* Thread has been preempted by another one */                          \
      return false;                                                           \
    }                                                                         \
    if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_INIT_POP_MOVE)) {          \
      M_CALL_SET(oplist, *ptr, table->Tab[i].x);                              \
    } else {                                                                  \
      M_DO_INIT_MOVE (oplist, *ptr, table->Tab[i].x);                         \
    }                                                                         \
    atomic_store_explicit(&table->Tab[i].seq, 2*iC + 1, memory_order_release); \
    M_QU3UE_MPMC_CONTRACT(table);                                             \
    return true;                                                              \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _init)(buffer_t buffer, size_t size)                              \
  {                                                                           \
    M_ASSERT (buffer != NULL);                                                \
    M_ASSERT( M_POWEROF2_P(size));                                            \
    M_ASSERT (0 < size && size <= UINT_MAX);                                  \
    M_ASSERT(((policy) & (M_BUFFER_STACK|M_BUFFER_THREAD_UNSAFE|M_BUFFER_PUSH_OVERWRITE)) == 0); \
    atomic_init(&buffer->ProdIdx, (unsigned int) size);                       \
    atomic_init(&buffer->ConsoIdx, (unsigned int) size);                      \
    buffer->size = (unsigned int) size;                                       \
    buffer->Tab = M_CALL_REALLOC(oplist, M_C(name, _el_ct), NULL, size);      \
    if (buffer->Tab == NULL) {                                                \
      M_MEMORY_FULL (size*sizeof(M_C(name, _el_ct) ));                        \
      return;                                                                 \
    }                                                                         \
    for(unsigned int j = 0; j < (unsigned int) size; j++) {                   \
      atomic_init(&buffer->Tab[j].seq, 2*j+1U);                               \
      if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_INIT_POP_MOVE)) {        \
        M_CALL_INIT(oplist, buffer->Tab[j].x);                                \
      }                                                                       \
    }                                                                         \
    M_QU3UE_MPMC_CONTRACT(buffer);                                            \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _clear)(buffer_t buffer)                                          \
  {                                                                           \
    M_QU3UE_MPMC_CONTRACT(buffer);                                            \
    if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_INIT_POP_MOVE)) {          \
      for(unsigned int j = 0; j < buffer->size; j++) {                        \
        M_CALL_CLEAR(oplist, buffer->Tab[j].x);                               \
      }                                                                       \
    } else {                                                                  \
      unsigned int iP = atomic_load_explicit(&buffer->ProdIdx, memory_order_relaxed); \
      unsigned int i  = iP & (buffer->size -1);                               \
      unsigned int iC = atomic_load_explicit(&buffer->ConsoIdx, memory_order_relaxed); \
      unsigned int j  = iC & (buffer->size -1);                               \
      while (i != j) {                                                        \
        M_CALL_CLEAR(oplist, buffer->Tab[j].x);                               \
        j++;                                                                  \
        if (j >= buffer->size)                                                \
          j = 0;                                                              \
      }                                                                       \
    }                                                                         \
    M_CALL_FREE(oplist, buffer->Tab);                                         \
    buffer->Tab = NULL; /* safer */                                           \
    buffer->size = 3;                                                         \
  }                                                                           \
                                                                              \
  static inline size_t                                                        \
  M_C(name, _size)(buffer_t table)                                            \
  {                                                                           \
    M_QU3UE_MPMC_CONTRACT(table);                                             \
    const unsigned int iC = atomic_load_explicit(&table->ConsoIdx, memory_order_relaxed); \
    const unsigned int iP = atomic_load_explicit(&table->ProdIdx, memory_order_acquire); \
    /* We return an approximation as we can't read both iC & iP atomically    \
       As we read producer index after consummer index,                       \
       and they are atomic variables without reordering                       \
       producer index is always greater or equal than consumer index          \
       (or on overflow occurs, in which case as we compute with modulo        \
       arithmetic, the right result is computed).                             \
       We may return a result which is greater than the size of the queue     \
       if the function is interrupted a long time between reading iC &        \
       iP. the function is not protected against it.                          \
    */                                                                        \
    return iP-iC;                                                             \
  }                                                                           \
                                                                              \
  static inline size_t                                                        \
  M_C(name, _capacity)(buffer_t v)                                            \
  {                                                                           \
    M_QU3UE_MPMC_CONTRACT(v);                                                 \
    return v->size;                                                           \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_C(name, _empty_p)(buffer_t v)                                             \
  {                                                                           \
    return M_C(name, _size) (v) == 0;                                         \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_C(name, _full_p)(buffer_t v)                                              \
  {                                                                           \
    return M_C(name, _size)(v) >= v->size;                                    \
  }                                                                           \
  


/* Definition of a a QUEUE for Single Producer / Single Consummer
   for high bandwidth scenario:
   * wait-free,
   * quite fast
   * no blocking calls.
   * only queue (no stack)
   * size of queue is always a power of 2
   * no overwriting.
   */

/* Deferred evaluation for the definition,
   so that all arguments are evaluated before further expansion */
#define M_QU3UE_SPSC_DEF_P1(arg) M_ID( M_QU3UE_SPSC_DEF_P2 arg )

/* Validate the value oplist before going further */
#define M_QU3UE_SPSC_DEF_P2(name, type, policy, oplist, buffer_t)             \
  M_IF_OPLIST(oplist)(M_QU3UE_SPSC_DEF_P3, M_QU3UE_SPSC_DEF_FAILURE)(name, type, policy, oplist, buffer_t)

/* Stop processing with a compilation failure */
#define M_QU3UE_SPSC_DEF_FAILURE(name, type, policy, oplist, buffer_t)        \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(QUEUE_SPSC_DEF): the given argument is not a valid oplist: " M_AS_STR(oplist))

#ifdef NDEBUG
#define M_QU3UE_SPSC_CONTRACT(table) do { } while (0)
#else
#define M_QU3UE_SPSC_CONTRACT(table) do {                                     \
    M_ASSERT (table != NULL);                                                 \
    unsigned int _r = atomic_load(&table->consoIdx);                          \
    unsigned int _w = atomic_load(&table->prodIdx);                           \
    /* Due to overflow we don't have M_ASSERT (_r <= _w); */                  \
    _r = atomic_load(&table->consoIdx);                                       \
    M_ASSERT (_r > _w || _w-_r <= table->size);                               \
    M_ASSERT (M_POWEROF2_P(table->size));                                     \
  } while (0)
#endif

/* Define the buffer type SPSC using atomics and its functions.
  - name: main prefix of the container
  - type: type of an element of the buffer
  - policy: the policy of the buffer
  - oplist: the oplist of the type of an element of the buffer
  - buffer_t: name of the buffer
  */
#define M_QU3UE_SPSC_DEF_P3(name, type, policy, oplist, buffer_t)             \
                                                                              \
  /* Single producer / Single consummer                                       \
     So, only one thread will write in this table. The other thread           \
     will only read. As such, there is no concurrent write, and no            \
     need to align the structure for best performance. */                     \
  typedef struct M_C(name, _el_s) {                                           \
    type         x;                                                           \
  } M_C(name, _el_ct);                                                        \
                                                                              \
  typedef struct M_C(name, _s) {                                              \
    atomic_uint  consoIdx; /* Can only increase until overflow */             \
    unsigned int size;                                                        \
    M_C(name, _el_ct) *Tab;                                                   \
    M_CACHELINE_ALIGN(align, atomic_uint, size_t, M_C(name, _el_ct) *);       \
    atomic_uint prodIdx;  /* Can only increase until overflow */              \
  } buffer_t[1];                                                              \
                                                                              \
  typedef type M_C(name, _subtype_ct);                                        \
  typedef buffer_t M_C(name, _ct);                                            \
                                                                              \
  static inline bool                                                          \
  M_C(name, _push)(buffer_t table, type const x)                              \
  {                                                                           \
    M_QU3UE_SPSC_CONTRACT(table);                                             \
    unsigned int r = atomic_load_explicit(&table->consoIdx,                   \
                                          memory_order_relaxed);              \
    unsigned int w = atomic_load_explicit(&table->prodIdx,                    \
                                          memory_order_acquire);              \
    if (w-r >= table->size)                                                   \
      return false;                                                           \
    unsigned int i = w & (table->size -1);                                    \
    if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_INIT_POP_MOVE)) {          \
      M_CALL_SET(oplist, table->Tab[i].x, x);                                 \
    } else {                                                                  \
      M_CALL_INIT_SET(oplist, table->Tab[i].x, x);                            \
    }                                                                         \
    atomic_store_explicit(&table->prodIdx, w+1, memory_order_release);        \
    M_QU3UE_SPSC_CONTRACT(table);                                             \
    return true;                                                              \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_C(name, _push_move)(buffer_t table, type *x)                              \
  {                                                                           \
    M_QU3UE_SPSC_CONTRACT(table);                                             \
    unsigned int r = atomic_load_explicit(&table->consoIdx,                   \
                                          memory_order_relaxed);              \
    unsigned int w = atomic_load_explicit(&table->prodIdx,                    \
                                          memory_order_acquire);              \
    if (w-r >= table->size)                                                   \
      return false;                                                           \
    unsigned int i = w & (table->size -1);                                    \
    if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_INIT_POP_MOVE)) {          \
      M_DO_MOVE(oplist, table->Tab[i].x, *x);                                 \
    } else {                                                                  \
      M_DO_INIT_MOVE(oplist, table->Tab[i].x, *x);                            \
    }                                                                         \
    atomic_store_explicit(&table->prodIdx, w+1, memory_order_release);        \
    M_QU3UE_SPSC_CONTRACT(table);                                             \
    return true;                                                              \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_C(name, _pop)(type *ptr, buffer_t table)                                  \
  {                                                                           \
    M_QU3UE_SPSC_CONTRACT(table);                                             \
    M_ASSERT (ptr != NULL);                                                   \
    unsigned int w = atomic_load_explicit(&table->prodIdx,                    \
                                          memory_order_relaxed);              \
    unsigned int r = atomic_load_explicit(&table->consoIdx,                   \
                                          memory_order_acquire);              \
    if (w-r == 0)                                                             \
      return false;                                                           \
    unsigned int i = r & (table->size -1);                                    \
    if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_INIT_POP_MOVE)) {          \
      M_CALL_SET(oplist, *ptr , table->Tab[i].x);                             \
    } else {                                                                  \
      M_DO_INIT_MOVE (oplist, *ptr, table->Tab[i].x);                         \
    }                                                                         \
    atomic_store_explicit(&table->consoIdx, r+1, memory_order_release);       \
    M_QU3UE_SPSC_CONTRACT(table);                                             \
    return true;                                                              \
  }                                                                           \
                                                                              \
  static inline unsigned                                                      \
  M_C(name, _push_bulk)(buffer_t table, unsigned n, type const x[])           \
  {                                                                           \
    M_QU3UE_SPSC_CONTRACT(table);                                             \
    M_ASSERT (x != NULL);                                                     \
    M_ASSERT (n <= table->size);                                              \
    unsigned int r = atomic_load_explicit(&table->consoIdx,                   \
                                          memory_order_relaxed);              \
    unsigned int w = atomic_load_explicit(&table->prodIdx,                    \
                                          memory_order_acquire);              \
    unsigned int max = M_MIN(n, table->size - (w-r) );                        \
    if (max == 0)                                                             \
      return 0;                                                               \
    for(unsigned int k = 0; k < max; k++) {                                   \
      unsigned int i = (w+k) & (table->size -1);                              \
      if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_INIT_POP_MOVE)) {        \
        M_CALL_SET(oplist, table->Tab[i].x, x[k]);                            \
      } else {                                                                \
        M_CALL_INIT_SET(oplist, table->Tab[i].x, x[k]);                       \
      }                                                                       \
    }                                                                         \
    atomic_store_explicit(&table->prodIdx, w+max, memory_order_release);      \
    M_QU3UE_SPSC_CONTRACT(table);                                             \
    return max;                                                               \
  }                                                                           \
                                                                              \
  static inline unsigned                                                      \
  M_C(name, _pop_bulk)(unsigned int n, type ptr[], buffer_t table)            \
  {                                                                           \
    M_QU3UE_SPSC_CONTRACT(table);                                             \
    M_ASSERT (ptr != NULL);                                                   \
    M_ASSERT (n <= table->size);                                              \
    unsigned int w = atomic_load_explicit(&table->prodIdx,                    \
                                          memory_order_relaxed);              \
    unsigned int r = atomic_load_explicit(&table->consoIdx,                   \
                                          memory_order_acquire);              \
    if (w-r == 0)                                                             \
      return 0;                                                               \
    unsigned int max = M_MIN(w-r, n);                                         \
    for(unsigned int k = 0; k < max; k++) {                                   \
      unsigned int i = (r+k) & (table->size -1);                              \
      if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_INIT_POP_MOVE)) {        \
        M_CALL_SET(oplist, ptr[k], table->Tab[i].x);                          \
      } else {                                                                \
        M_DO_INIT_MOVE (oplist, ptr[k], table->Tab[i].x);                     \
      }                                                                       \
    }                                                                         \
    atomic_store_explicit(&table->consoIdx, r+max, memory_order_release);     \
    M_QU3UE_SPSC_CONTRACT(table);                                             \
    return max;                                                               \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _push_force)(buffer_t table, type const x)                        \
  {                                                                           \
    M_QU3UE_SPSC_CONTRACT(table);                                             \
    unsigned int r = atomic_load_explicit(&table->consoIdx,                   \
                                          memory_order_relaxed);              \
    unsigned int w = atomic_load_explicit(&table->prodIdx,                    \
                                          memory_order_acquire);              \
    /* If no place in queue, try to skip the last one */                      \
    while (w-r >= table->size) {                                              \
      bool b = atomic_compare_exchange_strong(&table->consoIdx, &r, r+1);     \
      r += b;                                                                 \
    }                                                                         \
    unsigned int i = w & (table->size -1);                                    \
    if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_INIT_POP_MOVE)) {          \
      M_CALL_SET(oplist, table->Tab[i].x, x);                                 \
    } else {                                                                  \
      M_CALL_INIT_SET(oplist, table->Tab[i].x, x);                            \
    }                                                                         \
    atomic_store_explicit(&table->prodIdx, w+1, memory_order_release);        \
    M_QU3UE_SPSC_CONTRACT(table);                                             \
  }                                                                           \
                                                                              \
  static inline size_t                                                        \
  M_C(name, _size)(buffer_t table)                                            \
  {                                                                           \
    M_QU3UE_SPSC_CONTRACT(table);                                             \
    unsigned int r = atomic_load_explicit(&table->consoIdx,                   \
                                          memory_order_relaxed);              \
    unsigned int w = atomic_load_explicit(&table->prodIdx,                    \
                                          memory_order_acquire);              \
    /* We return an approximation as we can't read both r & w atomically      \
       As we read producer index after consummer index,                       \
       and they are atomic variables without reordering                       \
       producer index is always greater or equal than consumer index          \
       (or on overflow occurs, in which case as we compute with modulo        \
       arithmetic, the right result is computed).                             \
       We may return a result which is greater than the size of the queue     \
       if the function is interrupted a long time between reading the         \
       indexs. The function is not protected against it.                      \
    */                                                                        \
    return w-r;                                                               \
  }                                                                           \
                                                                              \
 static inline size_t                                                         \
 M_C(name, _capacity)(buffer_t v)                                             \
 {                                                                            \
   return v->size;                                                            \
 }                                                                            \
                                                                              \
  static inline bool                                                          \
  M_C(name, _empty_p)(buffer_t v)                                             \
  {                                                                           \
    return M_C(name, _size) (v) == 0;                                         \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_C(name, _full_p)(buffer_t v)                                              \
  {                                                                           \
    return M_C(name, _size)(v) >= v->size;                                    \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _init)(buffer_t buffer, size_t size)                              \
  {                                                                           \
    M_ASSERT (buffer != NULL);                                                \
    M_ASSERT( M_POWEROF2_P(size));                                            \
    M_ASSERT (0 < size && size <= UINT_MAX);                                  \
    M_ASSERT(((policy) & (M_BUFFER_STACK|M_BUFFER_THREAD_UNSAFE|M_BUFFER_PUSH_OVERWRITE)) == 0); \
    atomic_init(&buffer->prodIdx, (unsigned int) size);                       \
    atomic_init(&buffer->consoIdx, (unsigned int) size);                      \
    buffer->size = (unsigned int) size;                                       \
    buffer->Tab = M_CALL_REALLOC(oplist, M_C(name, _el_ct), NULL, size);      \
    if (buffer->Tab == NULL) {                                                \
      M_MEMORY_FULL (size*sizeof(M_C(name, _el_ct) ));                        \
      return;                                                                 \
    }                                                                         \
    if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_INIT_POP_MOVE)) {          \
      for(unsigned int j = 0; j < (unsigned int) size; j++) {                 \
        M_CALL_INIT(oplist, buffer->Tab[j].x);                                \
      }                                                                       \
    }                                                                         \
    M_QU3UE_SPSC_CONTRACT(buffer);                                            \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _clear)(buffer_t buffer)                                          \
  {                                                                           \
    M_QU3UE_SPSC_CONTRACT(buffer);                                            \
    if (!M_BUFF3R_POLICY_P((policy), M_BUFFER_PUSH_INIT_POP_MOVE)) {          \
      for(unsigned int j = 0; j < buffer->size; j++) {                        \
        M_CALL_CLEAR(oplist, buffer->Tab[j].x);                               \
      }                                                                       \
    } else {                                                                  \
      unsigned int iP = atomic_load_explicit(&buffer->prodIdx, memory_order_relaxed); \
      unsigned int i  = iP & (buffer->size -1);                               \
      unsigned int iC = atomic_load_explicit(&buffer->consoIdx, memory_order_relaxed); \
      unsigned int j  = iC & (buffer->size -1);                               \
      while (i != j) {                                                        \
        M_CALL_CLEAR(oplist, buffer->Tab[j].x);                               \
        j++;                                                                  \
        if (j >= buffer->size)                                                \
          j = 0;                                                              \
      }                                                                       \
    }                                                                         \
    M_CALL_FREE(oplist, buffer->Tab);                                         \
    buffer->Tab = NULL; /* safer */                                           \
    buffer->size = 3;                                                         \
  }                                                                           \
                                                                              \

/* Deferred evaluation for the definition,
   so that all arguments are evaluated before further expansion */
#define M_BUFF3R_OPLIST_P1(arg) M_BUFF3R_OPLIST_P2 arg

/* Validation of the given oplist */
#define M_BUFF3R_OPLIST_P2(name, oplist)                                      \
  M_IF_OPLIST(oplist)(M_BUFF3R_OPLIST_P3, M_BUFF3R_OPLIST_FAILURE)(name, oplist)

/* Prepare a clean compilation failure */
#define M_BUFF3R_OPLIST_FAILURE(name, oplist)                                 \
  ((M_LIB_ERROR(ARGUMENT_OF_BUFFER_OPLIST_IS_NOT_AN_OPLIST, name, oplist)))

/* OPLIST definition of a buffer */
#define M_BUFF3R_OPLIST_P3(name, oplist)                                      \
  (INIT(M_C3(m_buff3r_,name, _init))                                          \
   ,INIT_SET(M_C(name, _init_set))                                            \
   ,SET(M_C(name, _set))                                                      \
   ,CLEAR(M_C(name, _clear))                                                  \
   ,NAME(name)                                                                \
   ,TYPE(M_C(name,_ct))                                                       \
   ,SUBTYPE(M_C(name, _subtype_ct))                                           \
   ,RESET(M_C(name,_reset))                                                   \
   ,PUSH(M_C(name,_push))                                                     \
   ,POP(M_C(name,_pop))                                                       \
   ,OPLIST(oplist)                                                            \
   ,EMPTY_P(M_C(name, _empty_p)),                                             \
   ,GET_SIZE(M_C(name, _size))                                                \
   )

#if M_USE_SMALL_NAME
#define BUFFER_DEF M_BUFFER_DEF
#define BUFFER_DEF_AS M_BUFFER_DEF_AS
#define BUFFER_OPLIST M_BUFFER_OPLIST
#define QUEUE_MPMC_DEF M_QUEUE_MPMC_DEF
#define QUEUE_MPMC_DEF_AS M_QUEUE_MPMC_DEF_AS
#define QUEUE_SPSC_DEF M_QUEUE_SPSC_DEF
#define QUEUE_SPSC_DEF_AS M_QUEUE_SPSC_DEF_AS

#define buffer_policy_e m_buffer_policy_e
#define BUFFER_QUEUE M_BUFFER_QUEUE
#define BUFFER_STACK M_BUFFER_STACK
#define BUFFER_BLOCKING_PUSH M_BUFFER_BLOCKING_PUSH
#define BUFFER_UNBLOCKING_PUSH M_BUFFER_UNBLOCKING_PUSH
#define BUFFER_BLOCKING_POP M_BUFFER_BLOCKING_POP
#define BUFFER_UNBLOCKING_POP M_BUFFER_UNBLOCKING_POP
#define BUFFER_BLOCKING M_BUFFER_BLOCKING
#define BUFFER_UNBLOCKING M_BUFFER_UNBLOCKING
#define BUFFER_THREAD_SAFE M_BUFFER_THREAD_SAFE
#define BUFFER_THREAD_UNSAFE M_BUFFER_THREAD_UNSAFE
#define BUFFER_PUSH_INIT_POP_MOVE M_BUFFER_PUSH_INIT_POP_MOVE
#define BUFFER_PUSH_OVERWRITE M_BUFFER_PUSH_OVERWRITE
#define BUFFER_DEFERRED_POP M_BUFFER_DEFERRED_POP

#endif

#endif
