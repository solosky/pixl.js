/*
 * M*LIB - Basic Protected Concurrent module over container.
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
#ifndef MSTARLIB_CONCURRENT_H
#define MSTARLIB_CONCURRENT_H

#include "m-core.h"
#include "m-mutex.h"
#include "m-atomic.h"

/* Define a protected concurrent container and its associated functions
   based on the given container.
   USAGE: CONCURRENT_DEF(name, type [, oplist_of_the_type]) */
#define M_CONCURRENT_DEF(name, ...)                                           \
  M_CONCURRENT_DEF_AS(name, M_C(name,_t), __VA_ARGS__)


/* Define a protected concurrent container and its associated functions
   based on the given container as the given name name_t
   USAGE: CONCURRENT_DEF_AS(name, name_t, type [, oplist_of_the_type]) */
#define M_CONCURRENT_DEF_AS(name, name_t, ...)                                \
  M_BEGIN_PROTECTED_CODE                                                      \
  M_C0NCURRENT_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                             \
               ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), name_t ), \
                (name, __VA_ARGS__,                                        name_t ))) \
  M_END_PROTECTED_CODE


/* Define a protected concurrent container and its associated functions
   based on its given container. Operations that perform only read of the container
   can be done in parallel.
   USAGE: CONCURRENT_RP_DEF(name, type [, oplist_of_the_type]) */
#define M_CONCURRENT_RP_DEF(name, ...)                                        \
  M_CONCURRENT_RP_DEF_AS(name, M_C(name,_t), __VA_ARGS__)


/* Define a protected concurrent container and its associated functions
   as the given name name_t
   based on its given container. Operations that perform only read of the container
   can be done in parallel.
   USAGE: CONCURRENT_RP_DEF_AS(name, name_t, type [, oplist_of_the_type]) */
#define M_CONCURRENT_RP_DEF_AS(name, name_t, ...)                             \
  M_BEGIN_PROTECTED_CODE                                                      \
  M_C0NCURRENT_RP_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                          \
               ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), name_t ), \
                (name, __VA_ARGS__,                                        name_t ))) \
  M_END_PROTECTED_CODE


/* Define the oplist of a protected concurrent container given its name and its oplist.
   USAGE: CONCURRENT_OPLIST(name[, oplist of the type]) */
#define M_CONCURRENT_OPLIST(...)                                              \
  M_C0NCURRENT_OPLIST_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                          \
                     ((__VA_ARGS__, M_BASIC_OPLIST),                          \
                      (__VA_ARGS__ )))


/********************************** INTERNAL ************************************/

/* Deferred evaluation for the oplist definition,
   so that all arguments are evaluated before further expansion */
#define M_C0NCURRENT_OPLIST_P1(arg) M_C0NCURRENT_OPLIST_P2 arg

/* Validation of the given oplist */
#define M_C0NCURRENT_OPLIST_P2(name, oplist)                                  \
  M_IF_OPLIST(oplist)(M_C0NCURRENT_OPLIST_P3, M_C0NCURRENT_OPLIST_FAILURE)(name, oplist)

/* Prepare a clean compilation failure */
#define M_C0NCURRENT_OPLIST_FAILURE(name, oplist)                             \
  ((M_LIB_ERROR(ARGUMENT_OF_CONCURRENT_OPLIST_IS_NOT_AN_OPLIST, name, oplist)))

/* OPLIST definition
   GET_KEY is not present as its interface is not compatible with a concurrent
   container (_get returns a pointer to an internal data, data that may be 
   destroyed by another thread).
*/
#define M_C0NCURRENT_OPLIST_P3(name, oplist)                                  \
  (M_IF_METHOD(INIT, oplist)(INIT(M_C(name, _init)),)                         \
   ,M_IF_METHOD(INIT_SET, oplist)(INIT_SET(M_C(name, _init_set)),)            \
   ,M_IF_METHOD(SET, oplist)(SET(M_C(name, _set)),)                           \
   ,M_IF_METHOD(CLEAR, oplist)(CLEAR(M_C(name, _clear)),)                     \
   ,M_IF_METHOD(INIT_MOVE, oplist)(INIT_MOVE(M_C(name, _init_move)),)         \
   ,M_IF_METHOD(MOVE, oplist)(MOVE(M_C(name, _move)),)                        \
   ,M_IF_METHOD(SWAP,oplist)(SWAP(M_C(name, _swap)),)                         \
   ,NAME(name)                                                                \
   ,TYPE(M_C(name,_ct))                                                       \
   ,SUBTYPE(M_C(name, _subtype_ct))                                           \
   ,OPLIST(oplist)                                                            \
   ,M_IF_METHOD(EMPTY_P, oplist)(EMPTY_P(M_C(name,_empty_p)),)                \
   ,M_IF_METHOD(GET_SIZE, oplist)(GET_SIZE(M_C(name,_size)),)                 \
   ,M_IF_METHOD(RESET, oplist)(RESET(M_C(name,_reset)),)                      \
   ,M_IF_METHOD(KEY_TYPE, oplist)(KEY_TYPE(M_GET_KEY_TYPE oplist),)           \
   ,M_IF_METHOD(VALUE_TYPE, oplist)(VALUE_TYPE(M_GET_VALUE_TYPE oplist),)     \
   ,M_IF_METHOD(KEY_TYPE, oplist)(KEY_OPLIST(M_GET_KEY_OPLIST oplist),)       \
   ,M_IF_METHOD(VALUE_TYPE, oplist)(VALUE_OPLIST(M_GET_VALUE_OPLIST oplist), ) \
   ,M_IF_METHOD(SET_KEY, oplist)(SET_KEY(M_C(name, _set_at)),)                \
   ,M_IF_METHOD(ERASE_KEY, oplist)(ERASE_KEY(M_C(name, _erase)),)             \
   ,M_IF_METHOD(PUSH, oplist)(PUSH(M_C(name,_push)),)                         \
   ,M_IF_METHOD(POP, oplist)(POP(M_C(name,_pop)),)                            \
   ,M_IF_METHOD(PUSH_MOVE, oplist)(PUSH_MOVE(M_C(name,_push_move)),)          \
   ,M_IF_METHOD(POP_MOVE, oplist)(POP_MOVE(M_C(name,_pop_move)),)             \
   ,M_IF_METHOD(GET_STR, oplist)(GET_STR(M_C(name, _get_str)),)               \
   ,M_IF_METHOD(PARSE_STR, oplist)(PARSE_STR(M_C(name, _parse_str)),)         \
   ,M_IF_METHOD(OUT_STR, oplist)(OUT_STR(M_C(name, _out_str)),)               \
   ,M_IF_METHOD(IN_STR, oplist)(IN_STR(M_C(name, _in_str)),)                  \
   ,M_IF_METHOD(OUT_SERIAL, oplist)(OUT_SERIAL(M_C(name, _out_serial)),)      \
   ,M_IF_METHOD(IN_SERIAL, oplist)(IN_SERIAL(M_C(name, _in_serial)),)         \
   ,M_IF_METHOD(EQUAL, oplist)(EQUAL(M_C(name, _equal_p)),)                   \
   ,M_IF_METHOD(HASH, oplist)(HASH(M_C(name, _hash)),)                        \
   ,M_IF_METHOD(NEW, oplist)(NEW(M_GET_NEW oplist),)                          \
   ,M_IF_METHOD(REALLOC, oplist)(REALLOC(M_GET_REALLOC oplist),)              \
   ,M_IF_METHOD(DEL, oplist)(DEL(M_GET_DEL oplist),)                          \
   )

/* Internal contract 
   NOTE: Can't check too much without locking the container itself
*/
#define M_C0NCURRENT_CONTRACT(c) do {                                         \
    M_ASSERT ((c) != NULL);                                                   \
    M_ASSERT ( (c)->self == (c));                                             \
  } while (0)

/* Deferred evaluation for the concurrent definition,
   so that all arguments are evaluated before further expansion */
#define M_C0NCURRENT_DEF_P1(arg) M_ID( M_C0NCURRENT_DEF_P2 arg )

/* Validate the value oplist before going further */
#define M_C0NCURRENT_DEF_P2(name, type, oplist, concurrent_t)                 \
  M_IF_OPLIST(oplist)(M_C0NCURRENT_DEF_P3, M_C0NCURRENT_DEF_FAILURE)(name, type, oplist, concurrent_t)

/* Stop processing with a compilation failure */
#define M_C0NCURRENT_DEF_FAILURE(name, type, oplist, concurrent_t)            \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(CONCURRENT_DEF): the given argument is not a valid oplist: " M_AS_STR(oplist))

/* Internal concurrent definition
   - name: prefix to be used
   - type: type of the sub container
   - oplist: oplist of the type of the sub container
   - concurrent_t: alias for M_C(name, _t) [ type of the container ]
 */
#define M_C0NCURRENT_DEF_P3(name, type, oplist, concurrent_t)                 \
                                                                              \
  /* Define a concurrent container using a lock */                            \
  typedef struct M_C(name, _s) {                                              \
    struct M_C(name, _s) *self;                                               \
    m_mutex_t lock;                                                           \
    m_cond_t  there_is_data; /* condition raised when there is data */        \
    type      data;                                                           \
  } concurrent_t[1];                                                          \
                                                                              \
  /* Define alias for pointer types */                                        \
  typedef struct M_C(name, _s) *M_C(name, _ptr);                              \
  typedef const struct M_C(name, _s) *M_C(name, _srcptr);                     \
                                                                              \
  /* Internal types for oplist */                                             \
  typedef concurrent_t M_C(name, _ct);                                        \
  typedef type         M_C(name, _subtype_ct);                                \
                                                                              \
  /* Don't define iterator as it cannot be reliable in a concurrent type */   \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, type, oplist)                            \
                                                                              \
  /* Define the internal services used for the lock strategy  */              \
                                                                              \
  /* Initial the fields of the concurrent object not associated to the        \
  sub-container. */                                                           \
  static inline void                                                          \
  M_C(name, _internal_init)(concurrent_t out)                                 \
  {                                                                           \
    m_mutex_init(out->lock);                                                  \
    m_cond_init(out->there_is_data);                                          \
    out->self = out;                                                          \
    M_C0NCURRENT_CONTRACT(out);                                               \
  }                                                                           \
                                                                              \
  /* Clear the fields of the concurrent object not associated to the          \
  sub-container. */                                                           \
  static inline void                                                          \
  M_C(name, _internal_clear)(concurrent_t out)                                \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    m_mutex_clear(out->lock);                                                 \
    m_cond_clear(out->there_is_data);                                         \
    out->self = NULL;                                                         \
  }                                                                           \
                                                                              \
  /* Get the read lock. Multiple threads can get it, but only for reading.    \
     write lock is exclusive.                                                 \
     NOTE: This instance doesn't implement the read/write strategy,           \
      and only get the lock */                                                \
  static inline void                                                          \
  M_C(name, _read_lock)(const concurrent_t out)                               \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    m_mutex_lock (out->self->lock);                                           \
  }                                                                           \
                                                                              \
  /* Free the read lock. See above.                                           \
     NOTE: This instance doesn't implement the read/write strategy,           \
      and only get the lock */                                                \
  static inline void                                                          \
  M_C(name, _read_unlock)(const concurrent_t out)                             \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    m_mutex_unlock (out->self->lock);                                         \
  }                                                                           \
                                                                              \
  /* Wait for a thread pushing some data in the container.                    \
     CONSTRAINT: the read lock shall be get before calling this service */    \
  static inline void                                                          \
  M_C(name, _read_wait)(const concurrent_t out)                               \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    m_cond_wait(out->self->there_is_data, out->self->lock);                   \
  }                                                                           \
                                                                              \
  /* Get the write lock. Only one threads can get it, and no other threads    \
     can get the read lock too.                                               \
     NOTE: This instance doesn't implement the read/write strategy,           \
      and only get the lock */                                                \
  static inline void                                                          \
  M_C(name, _write_lock)(concurrent_t out)                                    \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    m_mutex_lock (out->lock);                                                 \
  }                                                                           \
                                                                              \
  /* Free the write lock.                                                     \
     NOTE: This instance doesn't implement the read/write strategy,           \
      and only get the lock */                                                \
  static inline void                                                          \
  M_C(name, _write_unlock)(concurrent_t out)                                  \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    m_mutex_unlock (out->lock);                                               \
  }                                                                           \
                                                                              \
  /* Wait for a thread pushing some data in the container.                    \
     CONSTRAINT: the write lock shall be get before calling this service */   \
  static inline void                                                          \
  M_C(name, _write_wait)(const concurrent_t out)                              \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    m_cond_wait(out->self->there_is_data, out->self->lock);                   \
  }                                                                           \
                                                                              \
  /* Wait to all threads that some data are available in the container.       \
     CONSTRAINT: the write lock shall be get before calling this service */   \
  static inline void                                                          \
  M_C(name, _write_signal)(concurrent_t out)                                  \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    /* We need to signal this to ALL waiting threads as multiple threads      \
    may wait on a some data of this container. */                             \
    m_cond_broadcast(out->there_is_data);                                     \
  }                                                                           \
                                                                              \
  M_C0NCURRENT_DEF_FUNC_P3(name, type, oplist, concurrent_t)


/* Internal definition of the functions commons to concurrent and rp-concurrent
   - name: prefix to be used
   - type: type of the sub container
   - oplist: oplist of the type of the sub container
   - concurrent_t: alias for M_C(name, _t) [ type of the container ]
  A function is defined only if the underlying container exports the needed
  services. It is usually one service declared per service exported.
*/
#define M_C0NCURRENT_DEF_FUNC_P3(name, type, oplist, concurrent_t)            \
                                                                              \
  M_IF_METHOD(INIT, oplist)(                                                  \
  static inline void                                                          \
  M_C(name, _init)(concurrent_t out)                                          \
  {                                                                           \
    M_C(name, _internal_init)(out);                                           \
    M_CALL_INIT(oplist, out->data);                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(INIT_SET, oplist)(                                              \
  static inline void                                                          \
  M_C(name, _init_set)(concurrent_t out, concurrent_t const src)              \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(src);                                               \
    M_ASSERT (out != src);                                                    \
    M_C(name, _internal_init)(out);                                           \
    M_C(name, _read_lock)(src);                                               \
    M_CALL_INIT_SET(oplist, out->data, src->data);                            \
    M_C(name, _read_unlock)(src);                                             \
    M_C0NCURRENT_CONTRACT(out);                                               \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(SET, oplist)(                                                   \
  static inline void                                                          \
  M_C(name, _set)(concurrent_t out, concurrent_t const src)                   \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    if (M_UNLIKELY (out == src)) return;                                      \
    /* Need to order the locks in a total way to avoid lock deadlock.         \
       Indeed, two call to _set can be done in two threads with :             \
       T1: A := B                                                             \
       T2: B := A                                                             \
       If we lock first the mutex of out, then the src, it could be possible  \
       in the previous scenario that both mutexs are locked: T1 has locked A  \
       and T2 has locked B, and T1 is waiting for locking B, and T2 is waiting \
       for locking A, resulting in a deadlock.                                \
       To avoid this problem, we **always** lock the mutex which address is   \
       the lowest. */                                                         \
    if (out < src) {                                                          \
      M_C(name, _write_lock)(out);                                            \
      M_C(name, _read_lock)(src);                                             \
    } else {                                                                  \
      M_C(name, _read_lock)(src);                                             \
      M_C(name, _write_lock)(out);                                            \
    }                                                                         \
    M_CALL_SET(oplist, out->data, src->data);                                 \
    if (out < src) {                                                          \
      M_C(name, _read_lock)(src);                                             \
      M_C(name, _write_unlock)(out);                                          \
    } else {                                                                  \
      M_C(name, _write_unlock)(out);                                          \
      M_C(name, _read_unlock)(src);                                           \
    }                                                                         \
    M_C0NCURRENT_CONTRACT(out);                                               \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(CLEAR, oplist)(                                                 \
  static inline void                                                          \
  M_C(name, _clear)(concurrent_t out)                                         \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    /* No need to lock. A clear is supposed to be called when all operations  \
    of the container in other threads are terminated */                       \
    M_CALL_CLEAR(oplist, out->data);                                          \
    M_C(name, _internal_clear)(out);                                          \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(INIT_MOVE, oplist)(                                             \
  static inline void                                                          \
  M_C(name, _init_move)(concurrent_t out, concurrent_t src)                   \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(src);                                               \
    M_ASSERT (out != src);                                                    \
    /* No need to lock 'src' ? */                                             \
    M_C(name, _internal_init)(out);                                           \
    M_CALL_INIT_MOVE(oplist, out->data, src->data);                           \
    M_C(name, _internal_clear)(src);                                          \
    M_C0NCURRENT_CONTRACT(out);                                               \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(MOVE, oplist)(                                                  \
  static inline void                                                          \
  M_C(name, _move)(concurrent_t out, concurrent_t src)                        \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_C0NCURRENT_CONTRACT(src);                                               \
    /* No need to lock 'src' ? */                                             \
    M_C(name, _write_lock)(out);                                              \
    M_CALL_MOVE(oplist, out->data, src->data);                                \
    M_C(name, _write_unlock)(out);                                            \
    M_C(name, _internal_clear)(src);                                          \
    M_C0NCURRENT_CONTRACT(out);                                               \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(SWAP, oplist)(                                                  \
  static inline void                                                          \
  M_C(name, _swap)(concurrent_t out, concurrent_t src)                        \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_C0NCURRENT_CONTRACT(src);                                               \
    if (M_UNLIKELY (out == src)) return;                                      \
    /* See comment above */                                                   \
    if (out < src) {                                                          \
      M_C(name, _write_lock)(out);                                            \
      M_C(name, _write_lock)(src);                                            \
    } else {                                                                  \
      M_C(name, _write_lock)(src);                                            \
      M_C(name, _write_lock)(out);                                            \
    }                                                                         \
    M_CALL_SWAP(oplist, out->data, src->data);                                \
    if (out < src) {                                                          \
      M_C(name, _write_unlock)(src);                                          \
      M_C(name, _write_unlock)(out);                                          \
    } else {                                                                  \
      M_C(name, _write_unlock)(out);                                          \
      M_C(name, _write_unlock)(src);                                          \
    }                                                                         \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(RESET, oplist)(                                                 \
  static inline void                                                          \
  M_C(name, _reset)(concurrent_t out)                                         \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_C(name, _write_lock)(out);                                              \
    M_CALL_RESET(oplist, out->data);                                          \
    M_C(name, _write_unlock)(out);                                            \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(EMPTY_P, oplist)(                                               \
  static inline bool                                                          \
  M_C(name, _empty_p)(concurrent_t const out)                                 \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_C(name, _read_lock)(out);                                               \
    bool b = M_CALL_EMPTY_P(oplist, out->data);                               \
    M_C(name, _read_unlock)(out);                                             \
    return b;                                                                 \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(GET_SIZE, oplist)(                                              \
  static inline size_t                                                        \
  M_C(name, _size)(concurrent_t const out)                                    \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_C(name, _read_lock)(out);                                               \
    size_t r = M_CALL_GET_SIZE(oplist, out->data);                            \
    M_C(name, _read_unlock)(out);                                             \
    return r;                                                                 \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(SET_KEY, oplist)(                                               \
  static inline void                                                          \
  M_C(name, _set_at)(concurrent_t out, M_GET_KEY_TYPE oplist const key, M_GET_VALUE_TYPE oplist const data) \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_C(name, _write_lock)(out);                                              \
    M_CALL_SET_KEY(oplist, out->data, key, data);                             \
    M_C(name, _write_signal)(out);                                            \
    M_C(name, _write_unlock)(out);                                            \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(GET_KEY, oplist)(                                               \
  static inline bool                                                          \
  M_C(name, _get_copy)(M_GET_VALUE_TYPE oplist *out_data, const concurrent_t out, M_GET_KEY_TYPE oplist const key) \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_ASSERT (out_data != NULL);                                              \
    M_C(name, _read_lock)(out);                                               \
    M_GET_VALUE_TYPE oplist *p = M_CALL_GET_KEY(oplist, out->data, key);      \
    if (p != NULL) {                                                          \
      M_CALL_SET(M_GET_VALUE_OPLIST oplist, *out_data, *p);                   \
    }                                                                         \
    M_C(name, _read_unlock)(out);                                             \
    return p != NULL;                                                         \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(SAFE_GET_KEY, oplist)(                                          \
  static inline void                                                          \
  M_C(name, _safe_get_copy)(M_GET_VALUE_TYPE oplist *out_data, concurrent_t out, M_GET_KEY_TYPE oplist const key) \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_ASSERT (out_data != NULL);                                              \
    M_C(name, _write_lock)(out);                                              \
    M_GET_VALUE_TYPE oplist *p = M_CALL_SAFE_GET_KEY(oplist, out->data, key); \
    M_ASSERT (p != NULL);                                                     \
    M_CALL_SET(M_GET_VALUE_OPLIST oplist, *out_data, *p);                     \
    M_C(name, _write_unlock)(out);                                            \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(ERASE_KEY, oplist)(                                             \
  static inline bool                                                          \
  M_C(name, _erase)(concurrent_t out, M_GET_KEY_TYPE oplist const key)        \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_C(name, _write_lock)(out);                                              \
    bool b = M_CALL_ERASE_KEY(oplist, out->data, key);                        \
    /* We suppose that the container has 'infinite' capacity, so              \
    we won't signal that a free space has been created */                     \
    M_C(name, _write_unlock)(out);                                            \
    return b;                                                                 \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(PUSH, oplist)(                                                  \
  static inline void                                                          \
  M_C(name, _push)(concurrent_t out, M_GET_SUBTYPE oplist const data)         \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_C(name, _write_lock)(out);                                              \
    M_CALL_PUSH(oplist, out->data, data);                                     \
    M_C(name, _write_signal)(out);                                            \
    M_C(name, _write_unlock)(out);                                            \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(POP, oplist)(                                                   \
  static inline void                                                          \
  M_C(name, _pop)(M_GET_SUBTYPE oplist *p, concurrent_t out)                  \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_C(name, _write_lock)(out);                                              \
    M_CALL_POP(oplist, p, out->data);                                         \
    /* See comment above */                                                   \
    M_C(name, _write_unlock)(out);                                            \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(PUSH_MOVE, oplist)(                                             \
  static inline void                                                          \
  M_C(name, _push_move)(concurrent_t out, M_GET_SUBTYPE oplist *data)         \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_C(name, _write_lock)(out);                                              \
    M_CALL_PUSH_MOVE(oplist, out->data, data);                                \
    M_C(name, _write_signal)(out);                                            \
    M_C(name, _write_unlock)(out);                                            \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(POP_MOVE, oplist)(                                              \
  static inline void                                                          \
  M_C(name, _pop_move)(M_GET_SUBTYPE oplist *p, concurrent_t out)             \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_C(name, _write_lock)(out);                                              \
    M_CALL_POP_MOVE(oplist, p, out->data);                                    \
    /* See comment above */                                                   \
    M_C(name, _write_unlock)(out);                                            \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(GET_STR, oplist)(                                               \
  static inline void                                                          \
  M_C(name, _get_str)(m_string_t str, concurrent_t const out, bool a)         \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_C(name, _read_lock)(out);                                               \
    M_CALL_GET_STR(oplist, str, out->data, a);                                \
    M_C(name, _read_unlock)(out);                                             \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(OUT_STR, oplist)(                                               \
  static inline void                                                          \
  M_C(name, _out_str)(FILE *f, concurrent_t const out)                        \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_C(name, _read_lock)(out);                                               \
    M_CALL_OUT_STR(oplist, f, out->data);                                     \
    M_C(name, _read_unlock)(out);                                             \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(PARSE_STR, oplist)(                                             \
  static inline bool                                                          \
  M_C(name, _parse_str)(concurrent_t out, const char str[], const char **e)   \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_C(name, _write_lock)(out);                                              \
    bool b = M_CALL_PARSE_STR(oplist, out->data, str, e);                     \
    M_C(name, _write_signal)(out);                                            \
    M_C(name, _write_unlock)(out);                                            \
    return b;                                                                 \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(IN_STR, oplist)(                                                \
  static inline bool                                                          \
  M_C(name, _in_str)(concurrent_t out, FILE *f)                               \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_C(name, _write_lock)(out);                                              \
    bool b = M_CALL_IN_STR(oplist, out->data, f);                             \
    M_C(name, _write_signal)(out);                                            \
    M_C(name, _write_unlock)(out);                                            \
    return b;                                                                 \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(OUT_SERIAL, oplist)(                                            \
  static inline m_serial_return_code_t                                        \
  M_C(name, _out_serial)(m_serial_write_t f, concurrent_t const out)          \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_C(name, _read_lock)(out);                                               \
    m_serial_return_code_t r = M_CALL_OUT_SERIAL(oplist, f, out->data);       \
    M_C(name, _read_unlock)(out);                                             \
    return r;                                                                 \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(IN_SERIAL, oplist)(                                             \
  static inline m_serial_return_code_t                                        \
  M_C(name, _in_serial)(concurrent_t out, m_serial_read_t f)                  \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_C(name, _write_lock)(out);                                              \
    m_serial_return_code_t r = M_CALL_IN_SERIAL(oplist, out->data, f);        \
    M_C(name, _write_signal)(out);                                            \
    M_C(name, _write_unlock)(out);                                            \
    return r;                                                                 \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(EQUAL, oplist)(                                                 \
  static inline bool                                                          \
  M_C(name, _equal_p)(concurrent_t const out1, concurrent_t const out2)       \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out1);                                              \
    M_C0NCURRENT_CONTRACT(out2);                                              \
    if (M_UNLIKELY (out1 == out2)) return true;                               \
    /* See comment above on mutal mutexs */                                   \
    if (out1 < out2) {                                                        \
      M_C(name, _read_lock)(out1);                                            \
      M_C(name, _read_lock)(out2);                                            \
    } else {                                                                  \
      M_C(name, _read_lock)(out2);                                            \
      M_C(name, _read_lock)(out1);                                            \
    }                                                                         \
    bool b = M_CALL_EQUAL(oplist, out1->data, out2->data);                    \
    if (out1 < out2) {                                                        \
      M_C(name, _read_unlock)(out2);                                          \
      M_C(name, _read_unlock)(out1);                                          \
    } else {                                                                  \
      M_C(name, _read_unlock)(out1);                                          \
      M_C(name, _read_unlock)(out2);                                          \
    }                                                                         \
    return b;                                                                 \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(GET_KEY, oplist)(                                               \
  static inline bool                                                          \
  M_C(name, _get_blocking)(M_GET_VALUE_TYPE oplist *out_data, const concurrent_t out, M_GET_KEY_TYPE oplist const key, bool blocking) \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_ASSERT (out_data != NULL);                                              \
    bool ret = false;                                                         \
    M_C(name, _read_lock)(out);                                               \
    while (true) {                                                            \
      M_GET_VALUE_TYPE oplist *p = M_CALL_GET_KEY(oplist, out->data, key);    \
      if (p != NULL) {                                                        \
        M_CALL_SET(M_GET_VALUE_OPLIST oplist, *out_data, *p);                 \
        ret = true;                                                           \
        break;                                                                \
      }                                                                       \
      if (blocking == false) break;                                           \
      /* No data: wait for a write to signal some data */                     \
      M_C(name, _read_wait)(out);                                             \
    }                                                                         \
    M_C(name, _read_unlock)(out);                                             \
    return ret;                                                               \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD2(POP, EMPTY_P, oplist)(                                         \
  static inline bool                                                          \
  M_C(name, _pop_blocking)(M_GET_SUBTYPE oplist *p, concurrent_t out, bool blocking) \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_ASSERT (p != NULL);                                                     \
    bool ret = false;                                                         \
    M_C(name, _write_lock)(out);                                              \
    while (true) {                                                            \
      if (!M_CALL_EMPTY_P(oplist, out->data)) {                               \
        M_CALL_POP(oplist, p, out->data);                                     \
        ret = true;                                                           \
        break;                                                                \
      }                                                                       \
      if (blocking == false) break;                                           \
      /* No data: wait for a write to signal some data */                     \
      M_C(name, _write_wait)(out);                                            \
    }                                                                         \
    M_C(name, _write_unlock)(out);                                            \
    return ret;                                                               \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD2(POP_MOVE, EMPTY_P, oplist)(                                    \
  static inline bool                                                          \
  M_C(name, _pop_move_blocking)(M_GET_SUBTYPE oplist *p, concurrent_t out, bool blocking) \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_ASSERT (p != NULL);                                                     \
    bool ret = false;                                                         \
    M_C(name, _write_lock)(out);                                              \
    while (true) {                                                            \
      if (!M_CALL_EMPTY_P(oplist, out->data)) {                               \
        M_CALL_POP_MOVE(oplist, p, out->data);                                \
        ret = true;                                                           \
        break;                                                                \
      }                                                                       \
      if (blocking == false) break;                                           \
      /* No data: wait for a write to signal some data */                     \
      M_C(name, _write_wait)(out);                                            \
    }                                                                         \
    M_C(name, _write_unlock)(out);                                            \
    return ret;                                                               \
  }                                                                           \
  ,)                                                                          \
                                                                              \
  M_IF_METHOD(HASH, oplist)(                                                  \
  static inline size_t                                                        \
  M_C(name, _hash)(concurrent_t const out)                                    \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    M_C(name, _read_lock)(out);                                               \
    size_t h = M_CALL_HASH(oplist, out->data);                                \
    M_C(name, _read_unlock)(out);                                             \
    /* The hash is unchanged by the concurrent container */                   \
    return h;                                                                 \
  }                                                                           \
  ,)                                                                          \



/* Deferred evaluation for the RP concurrent definition,
   so that all arguments are evaluated before further expansion */
#define M_C0NCURRENT_RP_DEF_P1(arg) M_ID( M_C0NCURRENT_RP_DEF_P2 arg )

/* Validate the value oplist before going further */
#define M_C0NCURRENT_RP_DEF_P2(name, type, oplist, concurrent_t)              \
  M_IF_OPLIST(oplist)(M_C0NCURRENT_RP_DEF_P3, M_C0NCURRENT_RP_DEF_FAILURE)(name, type, oplist, concurrent_t)

/* Stop processing with a compilation failure */
#define M_C0NCURRENT_RP_DEF_FAILURE(name, type, oplist, concurrent_t)         \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(CONCURRENT_RP_DEF): the given argument is not a valid oplist: " M_AS_STR(oplist))

/* Internal RP concurrent definition
   - name: prefix to be used
   - type: type of the sub container
   - oplist: oplist of the type of the sub container
   - concurrent_t: alias for M_C(name, _t) [ type of the container ]
 */
#define M_C0NCURRENT_RP_DEF_P3(name, type, oplist, concurrent_t)              \
                                                                              \
  typedef struct M_C(name, _s) {                                              \
    struct M_C(name, _s) *self;                                               \
    m_mutex_t lock;                                                           \
    m_cond_t  rw_done;                                                        \
    size_t    read_count;                                                     \
    bool      writer_waiting;                                                 \
    m_cond_t  there_is_data; /* condition raised when there is data */        \
    type      data;                                                           \
  } concurrent_t[1];                                                          \
                                                                              \
  typedef struct M_C(name, _s) *M_C(name, _ptr);                              \
  typedef const struct M_C(name, _s) *M_C(name, _srcptr);                     \
                                                                              \
  typedef type M_C(name, _subtype_ct);                                        \
                                                                              \
  /* Define the internal services for the lock strategy  */                   \
  static inline void                                                          \
  M_C(name, _internal_init)(concurrent_t out)                                 \
  {                                                                           \
    m_mutex_init(out->lock);                                                  \
    m_cond_init(out->rw_done);                                                \
    m_cond_init(out->there_is_data);                                          \
    out->self = out;                                                          \
    out->read_count = 0;                                                      \
    out->writer_waiting = false;                                              \
    M_C0NCURRENT_CONTRACT(out);                                               \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _internal_clear)(concurrent_t out)                                \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    m_mutex_clear(out->lock);                                                 \
    m_cond_clear(out->rw_done);                                               \
    m_cond_clear(out->there_is_data);                                         \
    out->self = NULL;                                                         \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _read_lock)(const concurrent_t out)                               \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    struct M_C(name, _s) *self = out->self;                                   \
    m_mutex_lock (self->lock);                                                \
    while (self->writer_waiting == true) {                                    \
      m_cond_wait(self->rw_done, self->lock);                                 \
    }                                                                         \
    self->read_count ++;                                                      \
    m_mutex_unlock (self->lock);                                              \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _read_unlock)(const concurrent_t out)                             \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    struct M_C(name, _s) *self = out->self;                                   \
    m_mutex_lock (self->lock);                                                \
    self->read_count --;                                                      \
    if (self->read_count == 0) {                                              \
      m_cond_broadcast (self->rw_done);                                       \
    }                                                                         \
    m_mutex_unlock (self->lock);                                              \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _write_lock)(concurrent_t out)                                    \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    m_mutex_lock (out->lock);                                                 \
    while (out->writer_waiting == true) {                                     \
      m_cond_wait(out->rw_done, out->lock);                                   \
    }                                                                         \
    out->writer_waiting = true;                                               \
    while (out->read_count > 0) {                                             \
      m_cond_wait(out->rw_done, out->lock);                                   \
    }                                                                         \
    m_mutex_unlock (out->lock);                                               \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _write_unlock)(concurrent_t out)                                  \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    m_mutex_lock (out->lock);                                                 \
    out->writer_waiting = false;                                              \
    m_cond_broadcast (out->rw_done);                                          \
    m_mutex_unlock (out->lock);                                               \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _read_wait)(const concurrent_t out)                               \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    struct M_C(name, _s) *self = out->self;                                   \
    M_ASSERT (self == out);                                                   \
    m_mutex_lock (out->self->lock);                                           \
    self->read_count --;                                                      \
    if (self->read_count == 0) {                                              \
      m_cond_broadcast (self->rw_done);                                       \
    }                                                                         \
    m_cond_wait(self->there_is_data, self->lock);                             \
    while (self->writer_waiting == true) {                                    \
      m_cond_wait(self->rw_done, self->lock);                                 \
    }                                                                         \
    self->read_count ++;                                                      \
    m_mutex_unlock (out->self->lock);                                         \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _write_wait)(concurrent_t out)                                    \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    m_mutex_lock (out->lock);                                                 \
    out->writer_waiting = false;                                              \
    m_cond_broadcast (out->rw_done);                                          \
    m_cond_wait(out->there_is_data, out->lock);                               \
    while (out->writer_waiting == true) {                                     \
      m_cond_wait(out->rw_done, out->lock);                                   \
    }                                                                         \
    out->writer_waiting = true;                                               \
    while (out->read_count > 0) {                                             \
      m_cond_wait(out->rw_done, out->lock);                                   \
    }                                                                         \
    m_mutex_unlock (out->lock);                                               \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _write_signal)(concurrent_t out)                                  \
  {                                                                           \
    M_C0NCURRENT_CONTRACT(out);                                               \
    m_mutex_lock (out->lock);                                                 \
    m_cond_broadcast(out->there_is_data);                                     \
    m_mutex_unlock (out->lock);                                               \
  }                                                                           \
                                                                              \
  M_C0NCURRENT_DEF_FUNC_P3(name, type, oplist, concurrent_t)

#if M_USE_SMALL_NAME
#define CONCURRENT_DEF M_CONCURRENT_DEF
#define CONCURRENT_DEF_AS M_CONCURRENT_DEF_AS
#define CONCURRENT_RP_DEF M_CONCURRENT_RP_DEF
#define CONCURRENT_RP_DEF_AS M_CONCURRENT_RP_DEF_AS
#define CONCURRENT_OPLIST M_CONCURRENT_OPLIST
#endif

#endif
