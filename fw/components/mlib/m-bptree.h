/*
 * M*LIB - B+TREE module
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
#ifndef MSTARLIB_BPTREE_H
#define MSTARLIB_BPTREE_H

#include "m-core.h"

/* Define a B+tree of size 'N' that maps a 'key' to a 'value'
   with its associated functions.
   USAGE: 
   BPTREE_DEF2(name, N, key_t, key_oplist, value_t, value_oplist)
   OR
   BPTREE_DEF2(name, N, key_t, value_t)
*/
#define M_BPTREE_DEF2(name, N, key_type, ...)                                 \
  M_BPTREE_DEF2_AS(name, M_C(name,_t), M_C(name,_it_t), M_C(name, _itref_t), N, key_type, __VA_ARGS__)


/* Define a B+tree of size 'N' that maps a 'key' to a 'value'
   as the given name name_t with its associated functions.
   USAGE: 
   BPTREE_DEF2_AS(name, name_t, it_t, itref_t, N, key_t, key_oplist, value_t, value_oplist)
   OR
   BPTREE_DEF2_AS(name, name_t, it_t, itref_t, N, key_t, value_t)
*/
#define M_BPTREE_DEF2_AS(name, name_t, it_t, itref_t, N, key_type, ...)       \
  M_BEGIN_PROTECTED_CODE                                                      \
  M_BPTR33_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                                 \
                 ((name, N, key_type, M_GLOBAL_OPLIST_OR_DEF(key_type)(), __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), 1, 0, name_t, M_C(name, _node_ct), M_C(name, _pit_ct), it_t, itref_t ), \
                  (name, N, key_type,                                     __VA_ARGS__,                                        1, 0, name_t, M_C(name, _node_ct), M_C(name, _pit_ct), it_t, itref_t ))) \
  M_END_PROTECTED_CODE


/* Define a B+tree of a given type, of size N.
   with its associated functions
   USAGE: BPTREE_DEF(name, N, type, [, oplist_of_the_type]) */
#define M_BPTREE_DEF(name, N, ...)                                            \
  M_BPTREE_DEF_AS(name, M_C(name,_t), M_C(name,_it_t), N, __VA_ARGS__)


/* Define a B+tree of a given type, of size N.
   as the given name name_t with its associated functions
   USAGE: BPTREE_DEF_AS(name, name_t, it_t, N, type, [, oplist_of_the_type]) */
#define M_BPTREE_DEF_AS(name, name_t, it_t, N, ...)                           \
  M_BEGIN_PROTECTED_CODE                                                      \
  M_BPTR33_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                                 \
                 ((name, N, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), 0, 0, name_t, M_C(name, _node_ct), M_C(name, _pit_ct), it_t, M_C(name, _itref_ct)  ), \
                  (name, N, __VA_ARGS__,                                        __VA_ARGS__,                                        0, 0, name_t, M_C(name, _node_ct), M_C(name, _pit_ct), it_t, M_C(name, _itref_ct)  ))) \
  M_END_PROTECTED_CODE


/* Define a B+tree of size 'N' that maps a 'key' to a 'value',
   allowing multiple equal keys to exist,
   with its associated functions.
   USAGE: 
   BPTREE_MULTI_DEF2(name, N, key_t, key_oplist, value_t, value_oplist)
   OR
   BPTREE_MULTI_DEF2(name, N, key_t, value_t)
*/
#define M_BPTREE_MULTI_DEF2(name, N, key_type, ...)                           \
  M_BPTREE_MULTI_DEF2_AS(name, M_C(name,_t), M_C(name,_it_t), M_C(name,_itref_t), N, key_type, __VA_ARGS__)


/* Define a B+tree of size 'N' that maps a 'key' to a 'value',
   allowing multiple equal keys to exist,
   as the given name name_t with its associated functions.
   USAGE: 
   BPTREE_MULTI_DEF2_AS(name, name_t, it_t, itref_t, N, key_t, key_oplist, value_t, value_oplist)
   OR
   BPTREE_MULTI_DEF2_AS(name, name_t, it_t, itref_t, N, key_t, value_t)
*/
#define M_BPTREE_MULTI_DEF2_AS(name, name_t, it_t, itref_t, N, key_type, ...) \
  M_BEGIN_PROTECTED_CODE                                                      \
  M_BPTR33_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                                 \
                 ((name, N, key_type, M_GLOBAL_OPLIST_OR_DEF(key_type)(), __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), 1, 1, name_t, M_C(name, _node_ct), M_C(name, _pit_ct), it_t, itref_t ), \
                  (name, N, key_type, __VA_ARGS__,                                                                            1, 1, name_t, M_C(name, _node_ct), M_C(name, _pit_ct), it_t, itref_t ))) \
  M_END_PROTECTED_CODE


/* Define a B+tree of a given type, of size N.
   allowing multiple equal keys to exist,
   with its associated functions
   USAGE: BPTREE_MULTI_DEF(name, N, type, [, oplist_of_the_type]) */
#define M_BPTREE_MULTI_DEF(name, N, ...)                                      \
  M_BPTREE_MULTI_DEF_AS(name, M_C(name,_t), M_C(name,_it_t), N, __VA_ARGS__)


/* Define a B+tree of a given type, of size N.
   allowing multiple equal keys to exist,
   as the given name name_t with its associated functions
   USAGE: BPTREE_MULTI_DEF_AS(name, name_t, it_t, N, type, [, oplist_of_the_type]) */
#define M_BPTREE_MULTI_DEF_AS(name, name_t, it_t, N, ...)                     \
  M_BEGIN_PROTECTED_CODE                                                      \
  M_BPTR33_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                                 \
                 ((name, N, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), 0, 1, name_t, M_C(name, _node_ct), M_C(name, _pit_ct), it_t, M_C(name, _itref_ct)  ), \
                  (name, N, __VA_ARGS__,                                        __VA_ARGS__,                                        0, 1, name_t, M_C(name, _node_ct), M_C(name, _pit_ct), it_t, M_C(name, _itref_ct)  ))) \
  M_BEGIN_PROTECTED_CODE


/* Define the oplist of a B+TREE used as a set of type (from BPTREE_DEF).
   USAGE: BPTREE_OPLIST(name [, oplist_of_the_type])
   NOTE: IT_REF is not exported so that the container appears as not modifiable
   by algorithm. */
#define M_BPTREE_OPLIST(...)                                                  \
  M_BPTR33_KEY_OPLIST_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                          \
                        ((__VA_ARGS__, M_BASIC_OPLIST ),                      \
                         (__VA_ARGS__ )))


/* Define the oplist of a B+TREE used as a map of a key type to a value type (from BPTREE_DEF2).
   USAGE: BPTREE_OPLIST2(name[, key_oplist, value_oplist]) 
   NOTE: IT_REF is not exported so that the container appears as not modifiable
   by algorithm. */
#define M_BPTREE_OPLIST2(...)                                                 \
  M_BPTR33_OPLIST2_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                             \
                     ((__VA_ARGS__, M_BASIC_OPLIST, M_BASIC_OPLIST ),         \
                      (__VA_ARGS__ )))
  


/********************************** INTERNAL ************************************/

/* Deferred evaluation for the oplist definition,
   so that all arguments are evaluated before further expansion */
#define M_BPTR33_KEY_OPLIST_P1(arg) M_BPTR33_KEY_OPLIST_P2 arg

/* Validation of the given oplists */
#define M_BPTR33_KEY_OPLIST_P2(name, oplist)                                  \
  M_IF_OPLIST(oplist)(M_BPTR33_KEY_OPLIST_P3, M_BPTR33_KEY_OPLIST_FAILURE)(name, oplist)

/* Prepare a clean compilation failure */
#define M_BPTR33_KEY_OPLIST_FAILURE(name, oplist)                             \
  ((M_LIB_ERROR(ARGUMENT_OF_BPTREE_OPLIST_IS_NOT_AN_OPLIST, name, oplist)))

/* OPLIST definition of a b+tree (global tree) */
#define M_BPTR33_KEY_OPLIST_P3(name, oplist)                                  \
  (INIT(M_C(name, _init)),                                                    \
   INIT_SET(M_C(name, _init_set)),                                            \
   INIT_WITH(API_1(M_INIT_VAI)),                                              \
   SET(M_C(name, _set)),                                                      \
   CLEAR(M_C(name, _clear)),                                                  \
   INIT_MOVE(M_C(name, _init_move)),                                          \
   MOVE(M_C(name, _move)),                                                    \
   SWAP(M_C(name, _swap)),                                                    \
   NAME(name),                                                                \
   TYPE(M_C(name,_ct)),                                                       \
   SUBTYPE(M_C(name, _subtype_ct)),                                           \
   IT_TYPE(M_C(name, _it_ct)),                                                \
   IT_FIRST(M_C(name,_it)),                                                   \
   IT_SET(M_C(name,_it_set)),                                                 \
   IT_END(M_C(name,_it_end)),                                                 \
   IT_END_P(M_C(name,_end_p)),                                                \
   IT_EQUAL_P(M_C(name,_it_equal_p)),                                         \
   IT_NEXT(M_C(name,_next)),                                                  \
   IT_CREF(M_C(name,_cref)),                                                  \
   RESET(M_C(name,_reset)),                                                   \
   PUSH(M_C(name,_push)),                                                     \
   GET_MIN(M_C(name,_min)),                                                   \
   GET_MAX(M_C(name,_max)),                                                   \
   M_IF_METHOD(GET_STR, oplist)(GET_STR(M_C(name, _get_str)),),               \
   M_IF_METHOD(PARSE_STR, oplist)(PARSE_STR(M_C(name, _parse_str)),),         \
   M_IF_METHOD(OUT_STR, oplist)(OUT_STR(M_C(name, _out_str)),),               \
   M_IF_METHOD(IN_STR, oplist)(IN_STR(M_C(name, _in_str)),),                  \
   M_IF_METHOD(OUT_SERIAL, oplist)(OUT_SERIAL(M_C(name, _out_serial)),),      \
   M_IF_METHOD(IN_SERIAL, oplist)(IN_SERIAL(M_C(name, _in_serial)),),         \
   M_IF_METHOD(EQUAL, oplist)(EQUAL(M_C(name, _equal_p)),),                   \
   M_IF_METHOD(HASH, oplist)(HASH(M_C(name, _hash)),)                         \
   ,M_IF_METHOD(NEW, oplist)(NEW(M_GET_NEW oplist),)                          \
   ,M_IF_METHOD(REALLOC, oplist)(REALLOC(M_GET_REALLOC oplist),)              \
   ,M_IF_METHOD(DEL, oplist)(DEL(M_GET_DEL oplist),)                          \
   )

/* Deferred evaluation */
#define M_BPTR33_OPLIST2_P1(arg) M_BPTR33_OPLIST2_P2 arg

/* Validation of the given oplists (first the key oplist, then the value oplist) */
#define M_BPTR33_OPLIST2_P2(name, key_oplist, value_oplist)                   \
  M_IF_OPLIST(key_oplist)(M_BPTR33_OPLIST2_P3, M_BPTR33_OPLIST2_FAILURE)(name, key_oplist, value_oplist)
#define M_BPTR33_OPLIST2_P3(name, key_oplist, value_oplist)                   \
  M_IF_OPLIST(value_oplist)(M_BPTR33_OPLIST2_P4, M_BPTR33_OPLIST2_FAILURE)(name, key_oplist, value_oplist)

/* Prepare a clean compilation failure */
#define M_BPTR33_OPLIST2_FAILURE(name, key_oplist, value_oplist)              \
  ((M_LIB_ERROR(ARGUMENT_OF_BPTREE_OPLIST_IS_NOT_AN_OPLIST, name, key_oplist, value_oplist)))

/* Final definition of the oplist (associative array) */
#define M_BPTR33_OPLIST2_P4(name, key_oplist, value_oplist)                   \
  (INIT(M_C(name, _init)),                                                    \
   INIT_SET(M_C(name, _init_set)),                                            \
   INIT_WITH(API_1(M_INIT_KEY_VAI)),                                          \
   SET(M_C(name, _set)),                                                      \
   CLEAR(M_C(name, _clear)),                                                  \
   INIT_MOVE(M_C(name, _init_move)),                                          \
   MOVE(M_C(name, _move)),                                                    \
   SWAP(M_C(name, _swap)),                                                    \
   NAME(name),                                                                \
   TYPE(M_C(name,_ct)),                                                       \
   SUBTYPE(M_C(name, _subtype_ct)),                                           \
   EMPTY_P(M_C(name,_empty_p)),                                               \
   IT_TYPE(M_C(name, _it_ct)),                                                \
   IT_FIRST(M_C(name,_it)),                                                   \
   IT_SET(M_C(name,_it_set)),                                                 \
   IT_END(M_C(name,_it_end)),                                                 \
   IT_END_P(M_C(name,_end_p)),                                                \
   IT_EQUAL_P(M_C(name,_it_equal_p)),                                         \
   IT_NEXT(M_C(name,_next)),                                                  \
   IT_CREF(M_C(name,_cref)),                                                  \
   RESET(M_C(name,_reset)),                                                   \
   GET_MIN(M_C(name,_min)),                                                   \
   GET_MAX(M_C(name,_max)),                                                   \
   KEY_TYPE(M_C(name, _key_ct)),                                              \
   VALUE_TYPE(M_C(name, _value_ct)),                                          \
   SET_KEY(M_C(name, _set_at)),                                               \
   GET_KEY(M_C(name, _get)),                                                  \
   SAFE_GET_KEY(M_C(name, _safe_get))                                         \
   ERASE_KEY(M_C(name, _erase)),                                              \
   KEY_OPLIST(key_oplist),                                                    \
   VALUE_OPLIST(value_oplist),                                                \
   M_IF_METHOD_BOTH(PARSE_STR, key_oplist, value_oplist)(PARSE_STR(M_C(name, _parse_str)),), \
   M_IF_METHOD_BOTH(OUT_STR, key_oplist, value_oplist)(OUT_STR(M_C(name, _out_str)),), \
   M_IF_METHOD_BOTH(IN_STR, key_oplist, value_oplist)(IN_STR(M_C(name, _in_str)),), \
   M_IF_METHOD_BOTH(OUT_SERIAL, key_oplist, value_oplist)(OUT_SERIAL(M_C(name, _out_serial)),), \
   M_IF_METHOD_BOTH(IN_SERIAL, key_oplist, value_oplist)(IN_SERIAL(M_C(name, _in_serial)),), \
   M_IF_METHOD_BOTH(EQUAL, key_oplist, value_oplist)(EQUAL(M_C(name, _equal_p)),), \
   M_IF_METHOD_BOTH(HASH, key_oplist, value_oplist)(HASH(M_C(name, _hash)),)  \
   ,M_IF_METHOD(NEW, key_oplist)(NEW(M_GET_NEW oplist),)                      \
   ,M_IF_METHOD(REALLOC, key_oplist)(REALLOC(M_GET_REALLOC oplist),)          \
   ,M_IF_METHOD(DEL, key_oplist)(DEL(M_GET_DEL oplist),)                      \
   )



/* Internal contract of a B+TREE of size 'N' for a node 'node' or root 'root' */
#ifdef NDEBUG
# define M_BPTR33_NODE_CONTRACT(N, isMulti, key_oplist, node, root) do { } while (0)
#else
# define M_BPTR33_NODE_CONTRACT(N, isMulti, key_oplist, node, root) do {      \
    M_ASSERT ((node) != NULL);                                                \
    M_ASSERT ((root) != NULL);                                                \
    int  num2     = (node)->num;                                              \
    bool is_leaf2 = num2 <= 0;                                                \
    num2 = num2 < 0 ? -num2 : num2;                                           \
    if ((node) == (root)) {                                                   \
      /* Contract of the root node. num can be 0 */                           \
      M_ASSERT( 0 <= num2 && num2 <= N);                                      \
      if (num2 == 0) M_ASSERT (is_leaf2);                                     \
    } else {                                                                  \
      /* Contract of a non-root node. num cannot be 0 */                      \
      int c2 = N / 2;                                                         \
      M_ASSERT (c2 > 0);                                                      \
      M_ASSERT (c2 <= num2 && num2 <= N);                                     \
    }                                                                         \
    /* The node is sorted */                                                  \
    for(int i2 = 1; i2 < num2 ; i2++) {                                       \
      M_ASSERT (M_CALL_CMP(key_oplist, (node)->key[i2-1], (node)->key[i2]) M_IF(isMulti)(<=, <) 0); \
    }                                                                         \
    /* The chain node is also sorted */                                       \
    if ((node)->next != NULL) {                                               \
      M_ASSERT (num2 >= 1);                                                   \
      M_ASSERT (M_CALL_CMP(key_oplist, (node)->key[num2-1], (node)->next->key[0]) M_IF(isMulti)(<=, <) 0); \
    }                                                                         \
  } while (0)
#endif

/* Contract for a B+TREE of size N named 'b' */
#define M_BPTR33_CONTRACT(N, isMulti, key_oplist, b) do {                     \
    M_ASSERT (N >= 3);  /* TBC: 2 instead ? */                                \
    M_BPTR33_NODE_CONTRACT(N, isMulti, key_oplist, (b)->root, (b)->root);     \
    M_ASSERT ((b)->root->next == NULL);                                       \
    if ((b)->root->num <= 0) M_ASSERT (-(b)->root->num == (int) (b)->size);   \
  } while (0)

/* Max depth of any B+tree
    Worst case is when all nodes are only half full.
    Worst case is with the mininum size of a node (2)
    Maximum number of elements: SIZE_MAX = 2 ^ (CHAR_BIT*sizeof (size_t)) - 1
  Height of such a tree if inferior to:
    Ceil(Log2(2 ^ (CHAR_BIT*sizeof (size_t)))) + 1
    "+ 1" due to final line composed of nodes.
 */
#define M_BPTR33_MAX_STACK ((int)(1 + CHAR_BIT*sizeof (size_t)))

/* Deferred evaluation for the b+tree definition,
   so that all arguments are evaluated before further expansion */
#define M_BPTR33_DEF_P1(arg) M_ID( M_BPTR33_DEF_P2 arg )

/* Validate the key oplist before going further */
#define M_BPTR33_DEF_P2(name, N, key_t, key_oplist, value_t, value_oplist, isMap, isMulti, tree_t, node_t, pit_t, it_t, subtype_t) \
  M_IF_OPLIST(key_oplist)(M_BPTR33_DEF_P3, M_BPTR33_DEF_FAILURE)(name, N, key_t, key_oplist, value_t, value_oplist, isMap, isMulti, tree_t, node_t, pit_t, it_t, subtype_t)

/* Validate the value oplist before going further */
#define M_BPTR33_DEF_P3(name, N, key_t, key_oplist, value_t, value_oplist, isMap, isMulti, tree_t, node_t, pit_t, it_t, subtype_t) \
  M_IF_OPLIST(value_oplist)(M_BPTR33_DEF_P4, M_BPTR33_DEF_FAILURE)(name, N, key_t, key_oplist, value_t, value_oplist, isMap, isMulti, tree_t, node_t, pit_t, it_t, subtype_t)

/* Stop processing with a compilation failure */
#define M_BPTR33_DEF_FAILURE(name, N, key_t, key_oplist, value_t, value_oplist, isMap, isMulti, tree_t, node_t, pit_t, it_t, subtype_t) \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(BPTREE*_DEF): one of the given argument is not a valid oplist: " M_AS_STR(key_oplist) " / " M_AS_STR(value_oplist))

/* Internal b+tree definition
   - name: prefix to be used
   - N: size of the node
   - key_t: key type of the elements of the container
   - key_oplist: oplist of the key type of the elements of the container
   - value_t: value type of the elements of the container
   - value_oplist: oplist of the value type of the elements of the container
   - isMap: true if map, false if set
   - isMulti: true if multimap/multiset, false otherwise
   - tree_t: alias for the type of the container
   - it_t: alias for the iterator of the container
   - node_t: alias for internal node
   - pit_t: alias for internal parent iterator
   - subtype_t: alias for the type referenced by the iterator
 */
#define M_BPTR33_DEF_P4(name, N, key_t, key_oplist, value_t, value_oplist, isMap, isMulti, tree_t, node_t, pit_t, it_t, subtype_t) \
                                                                              \
  M_IF(isMap)(                                                                \
    /* Type returned by the iterator. Due to having key and value             \
       separated in their own array in the node, it is pointers to            \
       the objects, not a global pointer to both objects. */                  \
    typedef struct M_C(name, _pair_s) {                                       \
      key_t   *key_ptr;                                                       \
      value_t *value_ptr;                                                     \
    } subtype_t;                                                              \
    ,                                                                         \
    typedef key_t subtype_t;                                                  \
  )                                                                           \
                                                                              \
  /* Define a Node of a B+TREE                                                \
   * For a reason of simplicity, it allocates one more element than           \
   * needed so that the code can push one more element in the node and        \
   * then split the nodes (simplify the code)                                 \
   */                                                                         \
  typedef struct M_C(name, _node_s) {                                         \
    int    num;           /* Abs=Number of keys. Sign <0 is leaf */           \
    key_t  key[N+1];      /* We can temporary push one more key */            \
    struct M_C(name, _node_s) *next;  /* next node reference */               \
    union  M_C(name, _kind_s) {       /* either value or pointer to other nodes */ \
      M_IF(isMap)(value_t        value[N+1];,)                                \
      struct M_C(name, _node_s) *node[N+2];                                   \
    } kind;                                                                   \
  } *node_t;                                                                  \
                                                                              \
  /* A B+TREE is just a pointer to the root node */                           \
  typedef struct M_C(name, _s) {                                              \
    node_t root;                                                              \
    size_t size;                                                              \
  } tree_t[1];                                                                \
  typedef struct M_C(name, _s) *M_C(name, _ptr);                              \
  typedef const struct M_C(name, _s) *M_C(name, _srcptr);                     \
                                                                              \
  /* Definition of the alias used by the oplists */                           \
  typedef subtype_t M_C(name, _subtype_ct);                                   \
  typedef key_t     M_C(name, _key_ct);                                       \
  typedef value_t   M_C(name, _value_ct);                                     \
  typedef tree_t    M_C(name, _ct);                                           \
                                                                              \
  /* Define the Parent Tree Iterator */                                       \
  typedef struct M_C(name, _parent_it_s) {                                    \
    int num;                                                                  \
    node_t parent[M_BPTR33_MAX_STACK];                                        \
  } pit_t[1];                                                                 \
                                                                              \
  /* Define the Iterator */                                                   \
  typedef struct M_C(name, _it_s) {                                           \
    M_IF(isMap)(struct M_C(name, _pair_s) pair;,)                             \
    node_t node;                                                              \
    int    idx;                                                               \
  } it_t[1];                                                                  \
  typedef it_t M_C(name, _it_ct);                                             \
                                                                              \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, key_t, key_oplist)                       \
  M_CHECK_COMPATIBLE_OPLIST(name, 2, value_t, value_oplist)                   \
                                                                              \
  /* Allocate a new node */                                                   \
  /* TODO: Can be specialized to alloc for leaf or for non leaf */            \
  static inline node_t M_C(name, _new_node)(void)                             \
  {                                                                           \
    M_STATIC_ASSERT(N >= 2, M_LIB_ILLEGAL_PARAM,                              \
          "Number of items per node shall be >= 2.");                         \
    node_t n = M_CALL_NEW(key_oplist, struct M_C(name, _node_s));             \
    if (M_UNLIKELY (n == NULL)) {                                             \
      M_MEMORY_FULL(sizeof (node_t));                                         \
      M_ASSERT (0);                                                           \
    }                                                                         \
    n->next = NULL;                                                           \
    n->num = 0;                                                               \
    return n;                                                                 \
  }                                                                           \
                                                                              \
  static inline void M_C(name, _init)(tree_t b)                               \
  {                                                                           \
    b->root = M_C(name, _new_node)();                                         \
    b->size = 0;                                                              \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                             \
  }                                                                           \
                                                                              \
  static inline bool M_C(name, _is_leaf)(const node_t n)                      \
  {                                                                           \
    /* We consider the empty node as a leaf */                                \
    /* Only the root node can be empty */                                     \
    return n->num <= 0;                                                       \
  }                                                                           \
                                                                              \
  /* Return the number of keys of the node */                                 \
  static inline int M_C(name, _get_num)(const node_t n)                       \
  {                                                                           \
    int num = n->num;                                                         \
    num = num < 0 ? -num : num;                                               \
    M_ASSERT (num <= N);                                                      \
    return num;                                                               \
  }                                                                           \
                                                                              \
  static inline void M_C(name, _reset)(tree_t b)                              \
  {                                                                           \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                             \
    node_t next, n = b->root;                                                 \
    pit_t pit;                                                                \
    /* np is the heigh of the tree */                                         \
    int np = 0;                                                               \
    /* Scan down the nodes to the left down node */                           \
    while (!M_C(name, _is_leaf)(n)) {                                         \
      pit->parent[np++] = n;                                                  \
      M_ASSERT (np <= M_BPTR33_MAX_STACK);                                    \
      n = n->kind.node[0];                                                    \
    }                                                                         \
    pit->parent[np++] = n;                                                    \
    M_ASSERT (np <= M_BPTR33_MAX_STACK);                                      \
    /* Clean & free non root */                                               \
    for(int i = 0; i < np; i++) {                                             \
      n = pit->parent[i];                                                     \
      while (n != NULL) {                                                     \
        /* Clear key (& value for leaf) */                                    \
        int num = M_C(name, _get_num)(n);                                     \
        M_IF(isMap)(bool is_leaf = M_C(name, _is_leaf)(n);,)                  \
        for(int j = 0; j < num; j++) {                                        \
          M_CALL_CLEAR(key_oplist, n->key[j]);                                \
          M_IF(isMap)(if (is_leaf) {                                          \
              M_CALL_CLEAR(value_oplist, n->kind.value[j]);                   \
            },)                                                               \
        }                                                                     \
        /* Next node of the same height */                                    \
        next = n->next;                                                       \
        if (i != 0) {                                                         \
          /* Free the node if non root */                                     \
          M_CALL_DEL(key_oplist, n);                                          \
        }                                                                     \
        n = next;                                                             \
      }                                                                       \
    }                                                                         \
    /* Clean root */                                                          \
    b->root->num = 0;                                                         \
    b->size = 0;                                                              \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                             \
  }                                                                           \
                                                                              \
  static inline void M_C(name, _clear)(tree_t b)                              \
  {                                                                           \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                             \
    M_C(name, _reset)(b);                                                     \
    /* Once the tree is clean, only the root remains */                       \
    M_CALL_DEL(key_oplist, b->root);                                          \
    b->root = NULL;                                                           \
  }                                                                           \
                                                                              \
  /* Copy recursively the node 'o' of root node 'root' */                     \
  static inline node_t M_C(name, _copy_node)(const node_t o, const node_t root) \
  {                                                                           \
    node_t n = M_C(name, _new_node)();                                        \
    /* Set default number of keys and type to copy */                         \
    n->num = o->num;                                                          \
    /* By default it is not linked to its brother.                            \
       Only the parent of this node can do it. It is fixed by it */           \
    n->next = NULL;                                                           \
    /* Get number of keys in the node and copy them */                        \
    int num = M_C(name, _get_num)(o);                                         \
    for(int i = 0; i < num; i++) {                                            \
      M_CALL_INIT_SET(key_oplist, n->key[i], o->key[i]);                      \
    }                                                                         \
    if (M_C(name, _is_leaf)(o)) {                                             \
      /* Copy the associated values if it is a leaf and a MAP */              \
      M_IF(isMap)(                                                            \
        for(int i = 0; i < num; i++) {                                        \
          M_CALL_INIT_SET(value_oplist, n->kind.value[i], o->kind.value[i]);  \
        }                                                                     \
      , /* End of isMap */)                                                   \
    } else {                                                                  \
      /* Copy recursively the associated nodes if it is not a leaf */         \
      for(int i = 0; i <= num; i++) {                                         \
        M_ASSERT(o->kind.node[i] != root);                                    \
        n->kind.node[i] = M_C(name, _copy_node)(o->kind.node[i], root);       \
      }                                                                       \
      /* The copied nodes don't have their next field correct */              \
      /* Fix the next field for the copied nodes */                           \
      for(int i = 0; i < num; i++) {                                          \
        node_t current = n->kind.node[i];                                     \
        node_t next = n->kind.node[i+1];                                      \
        current->next = next;                                                 \
        /* Go down the tree up to the leaf                                    \
           and fix the final 'next' link with the copied node */              \
        while (!M_C(name, _is_leaf)(current)) {                               \
          M_ASSERT(!M_C(name, _is_leaf)(next));                               \
          current = current->kind.node[current->num];                         \
          next    = next->kind.node[0];                                       \
          current->next = next;                                               \
        }                                                                     \
      }                                                                       \
    }                                                                         \
    M_BPTR33_NODE_CONTRACT(N, isMulti, key_oplist, n, (o==root) ? n : root);  \
    return n;                                                                 \
  }                                                                           \
                                                                              \
  static inline void M_C(name, _init_set)(tree_t b, const tree_t o)           \
  {                                                                           \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, o);                             \
    M_ASSERT (b != NULL);                                                     \
    /* Just copy recursively the root node */                                 \
    b->root = M_C(name, _copy_node)(o->root, o->root);                        \
    b->size = o->size;                                                        \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                             \
  }                                                                           \
                                                                              \
  static inline void M_C(name, _set)(tree_t b, const tree_t o)                \
  {                                                                           \
    /* NOTE: We could reuse the already allocated nodes of 'b'.               \
       Not sure if it worth the effort */                                     \
    M_C(name, _clear)(b);                                                     \
    M_C(name, _init_set)(b, o);                                               \
  }                                                                           \
                                                                              \
  static inline bool M_C(name, _empty_p)(const tree_t b)                      \
  {                                                                           \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                             \
    /* root shall be an empty leaf */                                         \
    return b->size == 0;                                                      \
  }                                                                           \
                                                                              \
  static inline size_t M_C(name, _size)(const tree_t b)                       \
  {                                                                           \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                             \
    return b->size;                                                           \
  }                                                                           \
                                                                              \
  static inline node_t M_C(name, _search_for_leaf)(pit_t pit, const tree_t b, key_t const key) \
  {                                                                           \
    node_t n = b->root;                                                       \
    int np = 0;                                                               \
    M_BPTR33_NODE_CONTRACT(N, isMulti, key_oplist, n, b->root);               \
    /* Go down the tree while searching for key */                            \
    while (!M_C(name, _is_leaf)(n)) {                                         \
      M_ASSERT (np <= M_BPTR33_MAX_STACK);                                    \
      int i, hi = n->num;                                                     \
      M_ASSERT (hi > 0);                                                      \
      /* Linear search is usually faster than binary search for               \
         B+TREE (due to cache effect). If a binary tree is faster for         \
         the choosen type and size , it probably means that the               \
         size of B+TREE is too big and should be reduced. */                  \
      for(i = 0; i < hi; i++) {                                               \
        if (M_CALL_CMP(key_oplist, key, n->key[i]) <= 0)                      \
          break;                                                              \
      }                                                                       \
      /* Update the Parent iterator */                                        \
      pit->parent[np++] = n;                                                  \
      /* Select the new node to go down to */                                 \
      n = n->kind.node[i];                                                    \
      M_ASSERT (n != NULL);                                                   \
      M_BPTR33_NODE_CONTRACT(N, isMulti, key_oplist, n, b->root);             \
    }                                                                         \
    pit->num = np;                                                            \
    return n;                                                                 \
  }                                                                           \
                                                                              \
  static inline value_t *M_C(name, _get)(const tree_t b, key_t const key)     \
  {                                                                           \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                             \
    pit_t pit;                                                                \
    /* Get the leaf node where the key can be */                              \
    node_t n = M_C(name, _search_for_leaf)(pit, b, key);                      \
    int cmp = 0;                                                              \
    M_BPTR33_NODE_CONTRACT(N, isMulti, key_oplist, n, b->root);               \
    /* Search in the leaf for key */                                          \
    for(int i = 0; cmp >= 0 && i < -n->num; i++) {                            \
      cmp = M_CALL_CMP(key_oplist, key, n->key[i]);                           \
      if (cmp == 0) {                                                         \
        /* Return the value if MAP mode or the key if SET mode */             \
        return M_IF(isMap)(&n->kind.value[i],&n->key[i]);                     \
      }                                                                       \
    }                                                                         \
    /* Key not found */                                                       \
    return NULL;                                                              \
  }                                                                           \
                                                                              \
  static inline value_t const *M_C(name, _cget)(const tree_t b, key_t const key) \
  {                                                                           \
    return M_CONST_CAST(value_t, M_C(name, _get)(b, key));                    \
  }                                                                           \
                                                                              \
  static inline int                                                           \
  M_C(name, _search_and_insert_in_leaf)(node_t n, key_t const key             \
                                        M_IF(isMap)( M_DEFERRED_COMMA value_t const value,) ) \
  {                                                                           \
    M_ASSERT (M_C(name, _is_leaf)(n));                                        \
    int i, num = M_C(name, _get_num)(n);                                      \
    M_ASSERT (num <= N);                                                      \
    /* Search for the key in the node n (a leaf) for insertion */             \
    for(i = 0; i < num; i++) {                                                \
      int cmp = M_CALL_CMP(key_oplist, key, n->key[i]);                       \
      if (cmp <= 0) {                                                         \
        M_IF(isMulti)( /* Nothing to do : fallthrough */,                     \
          /* Update value if keys are equal */                                \
          if (M_UNLIKELY (cmp == 0)) {                                        \
            M_IF(isMap)(M_CALL_SET(value_oplist, n->kind.value[i], value);,)  \
            return -1;                                                        \
          }                                                                   \
        )                                                                     \
        /* Move tables to make space for insertion */                         \
        memmove(&n->key[i+1], &n->key[i], sizeof(key_t)*(unsigned int)(num-i)); \
        M_IF(isMap)(memmove(&n->kind.value[i+1], &n->kind.value[i], sizeof(value_t)*(unsigned int)(num-i));,) \
        break;                                                                \
      }                                                                       \
    }                                                                         \
    /* Insert key & value if MAP mode */                                      \
    M_CALL_INIT_SET(key_oplist, n->key[i], key);                              \
    M_IF(isMap)(M_CALL_INIT_SET(value_oplist, n->kind.value[i], value);,)     \
    /* Increase the number of key in the node */                              \
    n->num  += -1; /* Increase num as num<0 for leaf */                       \
    return i;                                                                 \
  }                                                                           \
                                                                              \
  static inline int                                                           \
  M_C(name, _search_and_insert_in_node)(node_t n, node_t l, key_t key)        \
  {                                                                           \
    M_ASSERT (!M_C(name, _is_leaf)(n));                                       \
    int i, num = M_C(name, _get_num)(n);                                      \
    M_ASSERT (num <= N);                                                      \
    /* Search for the key in the node n (not a leaf) for insertion */         \
    for(i = 0; i < num; i++) {                                                \
      if (n->kind.node[i] == l) {                                             \
        /* Move tables to make space for insertion */                         \
        memmove(&n->key[i+1], &n->key[i], sizeof(key_t)*(unsigned int)(num-i)); \
        memmove(&n->kind.node[i+1], &n->kind.node[i], sizeof(node_t)*(unsigned int)(num-i+1)); \
        break;                                                                \
      }                                                                       \
    }                                                                         \
    /* Insert key in node */                                                  \
    /* TBC: DO_INIT_MOVE instead ? If key was in a node !*/                   \
    M_CALL_INIT_SET(key_oplist, n->key[i], key);                              \
    /* Increase the number of key in the node */                              \
    n->num  += 1;                                                             \
    return i;                                                                 \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_IF(isMap)(M_C(name, _set_at),M_C(name,_push))(tree_t b, key_t const key   \
                                                  M_IF(isMap)(M_DEFERRED_COMMA value_t const value,)) \
  {                                                                           \
    pit_t pit;                                                                \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                             \
    node_t leaf = M_C(name, _search_for_leaf)(pit, b, key);                   \
    /* Insert key into the leaf.*/                                            \
    /* NOTE: Even if there is N elements, we can still add one more.*/        \
    int i = M_C(name, _search_and_insert_in_leaf)(leaf, key M_IF (isMap)(M_DEFERRED_COMMA value,)); \
    if (i < 0) {                                                              \
      /* Nothing to do anymore. key already exists in the tree.               \
         value has been updated if needed */                                  \
      M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                           \
      return;                                                                 \
    }                                                                         \
    b->size ++;                                                               \
    /* Most likely case: leaf can accept key */                               \
    int num = -leaf->num;                                                     \
    M_ASSERT (num > 0);                                                       \
    if (M_LIKELY (num <= N)) {                                                \
      /* nothing more to do */                                                \
      M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                           \
      return;                                                                 \
    }                                                                         \
    M_ASSERT (num == N+1);                                                    \
                                                                              \
    /* Needs to rebalance the B+TREE */                                       \
    /* leaf is full: need to slip the leaf in two */                          \
    int nnum = (N + 1) / 2;                                                   \
    num = N + 1 - nnum;                                                       \
    node_t nleaf = M_C(name, _new_node)();                                    \
    /* Move half objects to the new node */                                   \
    memmove(&nleaf->key[0], &leaf->key[num], sizeof(key_t)*(unsigned int)nnum); \
    M_IF(isMap)(memmove(&nleaf->kind.value[0], &leaf->kind.value[num], sizeof(value_t)*(unsigned int)nnum);,) \
    leaf->num = -num;                                                         \
    nleaf->num = -nnum;                                                       \
    nleaf->next = leaf->next;                                                 \
    leaf->next = nleaf;                                                       \
    M_BPTR33_NODE_CONTRACT(N, isMulti, key_oplist, leaf, b->root);            \
    M_BPTR33_NODE_CONTRACT(N, isMulti, key_oplist, nleaf, b->root);           \
    /* Update parent to inject *key_ptr that splits between (leaf, nleaf) */  \
    key_t *key_ptr = &leaf->key[num-1];                                       \
    while (true) {                                                            \
      if (pit->num == 0) {                                                    \
        /* We reach root ==> Need to increase the height of the tree.*/       \
        node_t parent = M_C(name, _new_node)();                               \
        parent->num = 1;                                                      \
        /* TBC: DO_INIT_MOVE instead ? If key was in a node !*/               \
        M_CALL_INIT_SET(key_oplist, parent->key[0], *key_ptr);                \
        parent->kind.node[0] = leaf;                                          \
        parent->kind.node[1] = nleaf;                                         \
        b->root = parent;                                                     \
        M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                         \
        return;                                                               \
      }                                                                       \
      /* Non root node. Get the parent node */                                \
      node_t parent = pit->parent[--pit->num];                                \
      /* Insert into parent (It is big enough to receive temporary one more) */ \
      i = M_C(name, _search_and_insert_in_node)(parent, leaf, *key_ptr);      \
      parent->kind.node[i] = leaf;                                            \
      parent->kind.node[i+1] = nleaf;                                         \
      /* Test if parent node is full? */                                      \
      if (M_LIKELY (parent->num <= N)) {                                      \
        M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                         \
        return; /* No need to split parent.*/                                 \
      }                                                                       \
      M_ASSERT (parent->num == N+1);                                          \
      /* Need to split parent in {np} {med} {nnp} */                          \
      int nnp = N / 2;                                                        \
      int np = N - nnp;                                                       \
      M_ASSERT (nnp > 0 && np > 0 && nnp+np+1 == N+1);                        \
      node_t nparent = M_C(name, _new_node)();                                \
      /* Move half items to new node (Like a classic B-TREE)                  \
         and the median key to the grand-parent*/                             \
      memmove(&nparent->key[0], &parent->key[np+1], sizeof(key_t)*(unsigned int)nnp); \
      memmove(&nparent->kind.node[0], &parent->kind.node[np+1], sizeof(node_t)*(unsigned int)(nnp+1)); \
      parent->num = np;                                                       \
      nparent->num = nnp;                                                     \
      nparent->next = parent->next;                                           \
      parent->next = nparent;                                                 \
      M_BPTR33_NODE_CONTRACT(N, isMulti, key_oplist, parent, b->root);        \
      M_BPTR33_NODE_CONTRACT(N, isMulti, key_oplist, nparent, b->root);       \
      /* Prepare for the next step */                                         \
      key_ptr = &parent->key[np];                                             \
      leaf = parent;                                                          \
      nleaf = nparent;                                                        \
    }                                                                         \
  }                                                                           \
                                                                              \
  static inline value_t *M_C(name, _safe_get)(tree_t b, key_t const key)      \
  {                                                                           \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                             \
    /* Not optimized implementation */                                        \
    value_t *ret = M_C(name, _get)(b, key);                                   \
    if (ret == NULL) {                                                        \
      M_IF(isMap)(                                                            \
        value_t v;                                                            \
        M_CALL_INIT(value_oplist, v);                                         \
        M_C(name, _set_at)(b, key, v);                                        \
      ,                                                                       \
        M_C(name, _push)(b, key);                                             \
      )                                                                       \
      ret = M_C(name, _get)(b, key);                                          \
    }                                                                         \
    return ret;                                                               \
  }                                                                           \
                                                                              \
  static inline int                                                           \
  M_C(name, _search_and_remove_in_leaf)(node_t n, key_t const key)            \
  {                                                                           \
    M_ASSERT(M_C(name, _is_leaf)(n));                                         \
    const int num = M_C(name, _get_num)(n);                                   \
    for(int i = 0; i < num; i++) {                                            \
      const int cmp = M_CALL_CMP(key_oplist, key, n->key[i]);                 \
      if (cmp == 0) {                                                         \
        /* found key ==> delete it */                                         \
        M_CALL_CLEAR(key_oplist, n->key[i]);                                  \
        M_IF(isMap)(M_CALL_CLEAR(value_oplist, n->kind.value[i]);,)           \
        memmove(&n->key[i], &n->key[i+1], sizeof(key_t)*(unsigned int)(num-1-i)); \
        M_IF(isMap)(memmove(&n->kind.value[i], &n->kind.value[i+1], sizeof(value_t)*(unsigned int)(num-1-i));,) \
        n->num -= -1; /* decrease number as num is < 0 */                     \
        return i;                                                             \
      }                                                                       \
    }                                                                         \
    return -1; /* Not found */                                                \
  }                                                                           \
                                                                              \
  static inline void M_C(name, _left_shift)(node_t parent, int k)             \
  {                                                                           \
    M_ASSERT (parent != NULL && !M_C(name, _is_leaf)(parent));                \
    M_ASSERT (0 <= k && k < M_C(name, _get_num)(parent));                     \
    node_t left = parent->kind.node[k];                                       \
    node_t right = parent->kind.node[k+1];                                    \
    M_ASSERT (left != NULL && right != NULL);                                 \
    int num_left = M_C(name, _get_num)(left);                                 \
    int num_right = M_C(name, _get_num)(right);                               \
    M_ASSERT (num_left > N/2);                                                \
    M_ASSERT (num_right < N/2);                                               \
                                                                              \
    /* Move one item from the left node to the right node */                  \
    memmove(&right->key[1], &right->key[0], sizeof(key_t)*(unsigned int)num_right); \
    if (M_C(name, _is_leaf)(left)) {                                          \
      M_IF(isMap)(memmove (&right->kind.value[1], &right->kind.value[0], sizeof(value_t)*(unsigned int)num_right);,) \
      memmove (&right->key[0], &left->key[num_left-1], sizeof (key_t));       \
      M_IF(isMap)(memmove (&right->kind.value[0], &left->kind.value[num_left-1], sizeof (value_t));,) \
      right->num = -num_right - 1;                                            \
      left->num = -num_left + 1;                                              \
      M_CALL_SET(key_oplist, parent->key[k], left->key[num_left-2]);          \
    } else {                                                                  \
      memmove(&right->kind.node[1], &right->kind.node[0], sizeof(node_t)*(unsigned int)(num_right+1)); \
      /* parent[k] is moved to right[0] (clear). parent[k] is therefore clear */ \
      memmove(&right->key[0], &parent->key[k], sizeof(key_t));                \
      right->kind.node[0] = left->kind.node[num_left];                        \
      right->num = num_right + 1;                                             \
      left->num = num_left - 1;                                               \
      /* left[n-1] is move to parent[k] (clear). left[n-1] is therefore clear */ \
      memmove(&parent->key[k], &left->key[num_left-1], sizeof (key_t));       \
    }                                                                         \
    M_ASSERT (right->num != 0);                                               \
    M_ASSERT (left->num != 0);                                                \
  }                                                                           \
                                                                              \
  static inline void M_C(name, _right_shift)(node_t parent, int k)            \
  {                                                                           \
    M_ASSERT (parent != NULL && !M_C(name, _is_leaf)(parent));                \
    M_ASSERT (0 <= k && k < M_C(name, _get_num)(parent));                     \
    node_t left = parent->kind.node[k];                                       \
    node_t right = parent->kind.node[k+1];                                    \
    M_ASSERT (left != NULL && right != NULL);                                 \
    int num_left = M_C(name, _get_num)(left);                                 \
    int num_right = M_C(name, _get_num)(right);                               \
    M_ASSERT (num_left < N/2);                                                \
    M_ASSERT (num_right > N/2);                                               \
                                                                              \
    /* Move one item from the right node to the left node. */                 \
    if (M_C(name, _is_leaf)(right)) {                                         \
      memmove (&left->key[num_left], &right->key[0], sizeof(key_t));          \
      memmove (&right->key[0], &right->key[1], sizeof(key_t)*(unsigned int)(num_right-1)); \
      M_IF(isMap)(memmove (&left->kind.value[num_left], &right->kind.value[0], sizeof (value_t));,) \
      M_IF(isMap)(memmove (&right->kind.value[0], &right->kind.value[1], sizeof(value_t)*(unsigned int)(num_right-1));,) \
      right->num = -num_right + 1;                                            \
      left->num = -num_left - 1;                                              \
      M_CALL_SET(key_oplist, parent->key[k], left->key[num_left]);            \
    } else {                                                                  \
      memmove (&left->key[num_left], &parent->key[k], sizeof (key_t));        \
      memmove (&parent->key[k], &right->key[0], sizeof (key_t));              \
      memmove (&right->key[0], &right->key[1], sizeof(key_t)*(unsigned int)(num_right-1)); \
      left->kind.node[num_left+1] = right->kind.node[0];                      \
      memmove (&right->kind.node[0], &right->kind.node[1], sizeof(node_t)*(unsigned int)num_right); \
      right->num = num_right - 1;                                             \
      left->num = num_left + 1;                                               \
    }                                                                         \
    M_ASSERT (right->num != 0);                                               \
    M_ASSERT (left->num != 0);                                                \
  }                                                                           \
                                                                              \
  static inline void M_C(name, _merge_node)(node_t parent, int k, bool leaf)  \
  {                                                                           \
    M_ASSERT (parent != NULL && !M_C(name, _is_leaf)(parent));                \
    M_ASSERT (0 <= k && k < M_C(name, _get_num(parent)));                     \
    node_t left = parent->kind.node[k];                                       \
    node_t right = parent->kind.node[k+1];                                    \
    M_ASSERT (left != NULL && right != NULL);                                 \
    int num_parent = M_C(name, _get_num)(parent);                             \
    int num_left   = M_C(name, _get_num)(left);                               \
    int num_right  = M_C(name, _get_num)(right);                              \
                                                                              \
    /* Merge node 'k' and 'k+1' into a single one */                          \
    if (leaf) {                                                               \
      M_ASSERT (num_left + num_right <= N);                                   \
      memmove(&left->key[num_left], &right->key[0], sizeof(key_t)*(unsigned int)num_right); \
      M_IF(isMap)(memmove(&left->kind.value[num_left], &right->kind.value[0], sizeof(value_t)*(unsigned int)num_right);,) \
      left->num = -num_left - num_right;                                      \
    } else {                                                                  \
      M_ASSERT (num_left + num_right <= N -1);                                \
      memmove(&left->key[num_left+1], &right->key[0], sizeof(key_t)*(unsigned int)num_right); \
      memmove(&left->kind.node[num_left+1], &right->kind.node[0], sizeof(node_t)*(unsigned int)(num_right+1)); \
      M_CALL_INIT_SET(key_oplist, left->key[num_left], parent->key[k]);       \
      left->num = num_left + 1 + num_right;                                   \
    }                                                                         \
    left->next = right->next;                                                 \
    M_CALL_DEL(key_oplist, right);                                            \
    /* remove k'th key from the parent */                                     \
    M_CALL_CLEAR(key_oplist, parent->key[k]);                                 \
    memmove(&parent->key[k], &parent->key[k+1], sizeof(key_t)*(unsigned int)(num_parent - k - 1)); \
    memmove(&parent->kind.node[k+1], &parent->kind.node[k+2], sizeof(node_t)*(unsigned int)(num_parent - k -1)); \
    parent->num --;                                                           \
  }                                                                           \
                                                                              \
  /* We can also cache the index when we descend the tree.                    \
     TODO: Bench if this is worth the effort.*/                               \
  static inline int                                                           \
  M_C(name, _search_for_node)(node_t parent, node_t child)                    \
  {                                                                           \
    M_ASSERT (!M_C(name, _is_leaf)(parent));                                  \
    const int num = M_C(name, _get_num)(parent);                              \
    for(int i = 0; i <= num; i++) {                                           \
      if (parent->kind.node[i] == child)                                      \
        return i;                                                             \
    }                                                                         \
    M_ASSERT(false);                                                          \
    return -1; /* unreachable */                                              \
  }                                                                           \
                                                                              \
  static inline bool M_C(name, _erase)(tree_t b, key_t const key)             \
  {                                                                           \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                             \
    pit_t pit;                                                                \
    node_t leaf = M_C(name, _search_for_leaf)(pit, b, key);                   \
    int k = M_C(name, _search_and_remove_in_leaf)(leaf, key);                 \
    /* If key is not found ==> erase failed */                                \
    if (k < 0) return false;                                                  \
    /* Remove one item from the B+TREE */                                     \
    b->size --;                                                               \
    /* If number of keys greater than N>2 or root ==> Nothing more to do */   \
    if (M_LIKELY (M_C(name, _get_num)(leaf) >= N/2) || pit->num == 0)         \
      return true;                                                            \
    /* Leaf is too small. Needs rebalancing */                                \
    M_ASSERT (M_C(name, _get_num)(leaf) == N/2-1);                            \
    bool pass1 = true;                                                        \
    while (true) {                                                            \
      M_ASSERT (pit->num > 0);                                                \
      /* Search for node 'leaf' in parent */                                  \
      node_t parent = pit->parent[--pit->num];                                \
      M_ASSERT (parent != NULL);                                              \
      k = M_C(name, _search_for_node)(parent, leaf);                          \
      /* Look for the neighboor of the removed key. */                        \
      /* if we can steal one key from them to keep our node balanced */       \
      if (k > 0 && M_C(name, _get_num)(parent->kind.node[k-1]) > N/2) {       \
        M_C(name, _left_shift)(parent, k-1);                                  \
        return true;                                                          \
      } else if (k < M_C(name, _get_num)(parent)                              \
                 && M_C(name, _get_num)(parent->kind.node[k+1]) > N/2) {      \
        M_C(name, _right_shift)(parent, k);                                   \
        return true;                                                          \
      }                                                                       \
      /* Merge both nodes, removing 'k' from parent */                        \
      if (k == M_C(name, _get_num)(parent))                                   \
        k--;                                                                  \
      M_ASSERT(k >= 0 && k < M_C(name, _get_num)(parent));                    \
      /* Merge 'k' & 'k+1' & remove 'k' from parent */                        \
      M_C(name, _merge_node)(parent, k, pass1);                               \
      /* Check if we need to continue */                                      \
      if (M_C(name, _get_num)(parent) >= N/2)                                 \
        return true;                                                          \
      if (pit->num == 0) {                                                    \
        /* We reach the root */                                               \
        if (M_C(name, _get_num)(parent) == 0) {                               \
          /* Update root (deleted) */                                         \
          b->root = parent->kind.node[0];                                     \
          M_CALL_DEL(key_oplist, parent);                                     \
        }                                                                     \
        return true;                                                          \
      }                                                                       \
      /* Next iteration */                                                    \
      leaf = parent;                                                          \
      pass1 = false;                                                          \
    }                                                                         \
  }                                                                           \
                                                                              \
  static inline bool M_C(name, _pop_at)(value_t *ptr, tree_t b, key_t const key) \
  {                                                                           \
    if (ptr != NULL) {                                                        \
      value_t *ref = M_C(name, _get)(b, key);                                 \
      if (ref == NULL) {                                                      \
        return false;                                                         \
      }                                                                       \
      M_CALL_SET(value_oplist, *ptr, *ref);                                   \
    }                                                                         \
    return M_C(name, _erase)(b, key);                                         \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _it)(it_t it, const tree_t b)                                     \
  {                                                                           \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                             \
    M_ASSERT (it != NULL);                                                    \
    node_t n = b->root;                                                       \
    /* Scan down the nodes */                                                 \
    while (!M_C(name, _is_leaf)(n)) {                                         \
      n = n->kind.node[0];                                                    \
    }                                                                         \
    it->node = n;                                                             \
    it->idx  = 0;                                                             \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _it_end)(it_t it, const tree_t b)                                 \
  {                                                                           \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                             \
    M_ASSERT (it != NULL);                                                    \
    node_t n = b->root;                                                       \
    /* Scan down the nodes */                                                 \
    while (!M_C(name, _is_leaf)(n)) {                                         \
      n = n->kind.node[n->num];                                               \
    }                                                                         \
    it->node = n;                                                             \
    it->idx  = -n->num;                                                       \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _it_set)(it_t itd, const it_t its)                                \
  {                                                                           \
    M_ASSERT (itd != NULL && its != NULL);                                    \
    itd->node = its->node;                                                    \
    itd->idx  = its->idx;                                                     \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_C(name, _end_p)(it_t it)                                                  \
  {                                                                           \
    M_ASSERT (it != NULL && it->node != NULL);                                \
    M_ASSERT (M_C(name, _is_leaf)(it->node));                                 \
    return it->node->next ==NULL && it->idx >= -it->node->num;                \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_C(name, _it_equal_p)(const it_t it1, const it_t it2)                      \
  {                                                                           \
    return it1->node == it2->node && it1->idx == it2->idx;                    \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _next)(it_t it)                                                   \
  {                                                                           \
    M_ASSERT (it != NULL && it->node != NULL);                                \
    M_ASSERT (M_C(name, _is_leaf)(it->node));                                 \
    it->idx ++;                                                               \
    if (it->idx >= -it->node->num && it->node->next != NULL) {                \
      it->node = it->node->next;                                              \
      it->idx = 0;                                                            \
    }                                                                         \
  }                                                                           \
                                                                              \
  static inline subtype_t *                                                   \
  M_C(name, _ref)(it_t it)                                                    \
  {                                                                           \
    M_ASSERT (it != NULL && it->node != NULL);                                \
    M_ASSERT (M_C(name, _is_leaf)(it->node));                                 \
    M_ASSERT (it->idx <= -it->node->num);                                     \
    M_IF(isMap)(                                                              \
                it->pair.key_ptr = &it->node->key[it->idx];                   \
                it->pair.value_ptr = &it->node->kind.value[it->idx];          \
                return &it->pair                                              \
                ,                                                             \
                return &it->node->key[it->idx]                                \
                                                                        );    \
  }                                                                           \
                                                                              \
  static inline subtype_t const *                                             \
  M_C(name, _cref)(it_t it)                                                   \
  {                                                                           \
    return M_CONST_CAST(subtype_t, M_C(name, _ref)(it));                      \
  }                                                                           \
                                                                              \
                                                                              \
  static inline void                                                          \
  M_C(name, _it_from)(it_t it, const tree_t b, key_t const key)               \
  {                                                                           \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                             \
    M_ASSERT (it != NULL);                                                    \
    pit_t pit;                                                                \
    node_t n = M_C(name, _search_for_leaf)(pit, b, key);                      \
    it->node = n;                                                             \
    int i;                                                                    \
    M_BPTR33_NODE_CONTRACT(N, isMulti, key_oplist, n, b->root);               \
    for(i = 0; i < -n->num; i++) {                                            \
      if (M_CALL_CMP(key_oplist, key, n->key[i]) <= 0)                        \
        break;                                                                \
    }                                                                         \
    if (i == -n->num && n->next != NULL) {                                    \
      it->node = n->next;                                                     \
      i = 0;                                                                  \
    }                                                                         \
    it->idx  = i;                                                             \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_C(name, _it_until_p)(it_t it, key_t const key)                            \
  {                                                                           \
    M_ASSERT (it != NULL);                                                    \
    node_t n = it->node;                                                      \
    if (it->idx >= -n->num) return true;                                      \
    int cmp = M_CALL_CMP(key_oplist, n->key[it->idx], key);                   \
    return (cmp >= 0);                                                        \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_C(name, _it_while_p)(it_t it, key_t const key)                            \
  {                                                                           \
    M_ASSERT (it != NULL);                                                    \
    node_t n = it->node;                                                      \
    if (it->idx >= -n->num) return false;                                     \
    int cmp = M_CALL_CMP(key_oplist, n->key[it->idx], key);                   \
    return (cmp <= 0);                                                        \
  }                                                                           \
                                                                              \
  static inline value_t *                                                     \
  M_C(name, _min)(const tree_t b)                                             \
  {                                                                           \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                             \
    if (M_UNLIKELY (b->size == 0)) return NULL;                               \
    node_t n = b->root;                                                       \
    /* Scan down the nodes */                                                 \
    while (!M_C(name, _is_leaf)(n)) {                                         \
      n = n->kind.node[0];                                                    \
    }                                                                         \
    return &n->M_IF(isMap)(kind.value, key)[0];                               \
  }                                                                           \
                                                                              \
  static inline value_t *                                                     \
  M_C(name, _max)(const tree_t b)                                             \
  {                                                                           \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                             \
    if (M_UNLIKELY (b->size == 0)) return NULL;                               \
    node_t n = b->root;                                                       \
    /* Scan down the nodes */                                                 \
    while (!M_C(name, _is_leaf)(n)) {                                         \
      n = n->kind.node[n->num];                                               \
    }                                                                         \
    return &n->M_IF(isMap)(kind.value, key)[-n->num-1];                       \
  }                                                                           \
                                                                              \
  static inline value_t const *                                               \
  M_C(name, _cmin)(const tree_t tree)                                         \
  {                                                                           \
    return M_CONST_CAST(value_t, M_C(name, _min)(tree));                      \
  }                                                                           \
                                                                              \
  static inline value_t const *                                               \
  M_C(name, _cmax)(const tree_t tree)                                         \
  {                                                                           \
    return M_CONST_CAST(value_t, M_C(name, _max)(tree));                      \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _init_move)(tree_t b, tree_t ref)                                 \
  {                                                                           \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, ref);                           \
    M_ASSERT (b != NULL && b != ref);                                         \
    b->size = ref->size;                                                      \
    b->root = ref->root;                                                      \
    ref->root = NULL;                                                         \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                             \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _move)(tree_t b, tree_t ref)                                      \
  {                                                                           \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                             \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, ref);                           \
    M_ASSERT (b != ref);                                                      \
    M_C(name,_clear)(b);                                                      \
    M_C(name,_init_move)(b, ref);                                             \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, b);                             \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _swap)(tree_t tree1, tree_t tree2)                                \
  {                                                                           \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, tree1);                         \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, tree2);                         \
    M_SWAP(size_t, tree1->size, tree2->size);                                 \
    M_SWAP(node_t, tree1->root, tree2->root);                                 \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, tree1);                         \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, tree2);                         \
  }                                                                           \
                                                                              \
  M_BPTR33_FUNC_ADDITIONAL_DEF2(name, N, key_t, key_oplist, value_t, value_oplist, isMap, isMulti, tree_t, node_t, pit_t, it_t, subtype_t)


/* Define additional functions.
   Do not used any fields but the already defined methods */
#define M_BPTR33_FUNC_ADDITIONAL_DEF2(name, N, key_t, key_oplist, value_t, value_oplist, isMap, isMulti, tree_t, node_t, pit_t, it_t, subtype_t) \
                                                                              \
  M_IF_METHOD_BOTH(EQUAL, key_oplist, value_oplist)(                          \
  static inline bool M_C(name,_equal_p)(const tree_t t1, const tree_t t2) {   \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, t1);                            \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, t2);                            \
    if (t1->size != t2->size) return false;                                   \
    if (t1->size == 0) return true;                                           \
    /* Slow comparaison */                                                    \
    it_t it1;                                                                 \
    it_t it2;                                                                 \
    /* NOTE: We can't compare two trees directly as they can be               \
       structuraly different but functionnaly equal (you get this by          \
       constructing the tree in a different way). We have to                  \
       compare the ordered value within the tree. */                          \
    M_C(name, _it)(it1, t1);                                                  \
    M_C(name, _it)(it2, t2);                                                  \
    while (!M_C(name, _end_p)(it1)                                            \
           && !M_C(name, _end_p)(it2)) {                                      \
      const subtype_t *ref1 = M_C(name, _cref)(it1);                          \
      const subtype_t *ref2 = M_C(name, _cref)(it2);                          \
      M_IF(isMap)(                                                            \
      if (!M_CALL_EQUAL(key_oplist, *ref1->key_ptr, *ref2->key_ptr))          \
        return false;                                                         \
      if (!M_CALL_EQUAL(value_oplist, *ref1->value_ptr, *ref2->value_ptr))    \
        return false;                                                         \
      ,                                                                       \
      if (!M_CALL_EQUAL(key_oplist, *ref1, *ref2))                            \
        return false;                                                         \
                                                                        )     \
      M_C(name, _next)(it1);                                                  \
      M_C(name, _next)(it2);                                                  \
    }                                                                         \
    return M_C(name, _end_p)(it1)                                             \
      && M_C(name, _end_p)(it2);                                              \
  }                                                                           \
  , /* NO EQUAL METHOD */ )                                                   \
                                                                              \
  M_IF_METHOD_BOTH(HASH, key_oplist, value_oplist)(                           \
  static inline size_t M_C(name,_hash)(const tree_t t1) {                     \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, t1);                            \
    M_HASH_DECL(hash);                                                        \
    /* NOTE: We can't compute the hash directly for the same reason           \
       than for EQUAL operator. */                                            \
    it_t it1;                                                                 \
    M_C(name, _it)(it1, t1);                                                  \
    while (!M_C(name, _end_p)(it1)) {                                         \
      subtype_t const *ref1 = M_C(name, _cref)(it1);                          \
      M_IF(isMap)(                                                            \
                  M_HASH_UP(hash, M_CALL_HASH(key_oplist, *ref1->key_ptr));   \
                  M_HASH_UP(hash, M_CALL_HASH(value_oplist, *ref1->value_ptr)); \
                  ,                                                           \
                  M_HASH_UP(hash, M_CALL_HASH(key_oplist, *ref1));            \
                                                                        )     \
      M_C(name, _next)(it1);                                                  \
    }                                                                         \
    return M_HASH_FINAL (hash);                                               \
  }                                                                           \
  , /* NO HASH METHOD */ )                                                    \
                                                                              \
  M_IF_METHOD_BOTH(GET_STR, key_oplist, value_oplist)(                        \
  static inline void M_C(name, _get_str)(m_string_t str,                      \
                                         const tree_t t1, bool append) {      \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, t1);                            \
    M_ASSERT(str != NULL);                                                    \
    (append ? m_string_cat_cstr : m_string_set_cstr) (str, "[");              \
    /* NOTE: The print is really naive, and not really efficient */           \
    bool commaToPrint = false;                                                \
    it_t it;                                                                  \
    for (M_C(name, _it)(it, t1) ;                                             \
         !M_C(name, _end_p)(it);                                              \
         M_C(name, _next)(it)) {                                              \
      if (commaToPrint)                                                       \
        m_string_push_back (str, M_GET_SEPARATOR key_oplist);                 \
      commaToPrint = true;                                                    \
      subtype_t const *ref1 = M_C(name, _cref)(it);                           \
      M_IF(isMap)(                                                            \
                  M_CALL_GET_STR(key_oplist, str, *ref1->key_ptr, true);      \
                  m_string_cat_cstr(str, ":");                                \
                  M_CALL_GET_STR(value_oplist,str, *ref1->value_ptr, true)    \
                  ,                                                           \
                  M_CALL_GET_STR(key_oplist, str, *ref1, true);               \
                                                                        );    \
    }                                                                         \
    m_string_push_back (str, ']');                                            \
  }                                                                           \
  , /* NO GET_STR */ )                                                        \
                                                                              \
  M_IF_METHOD_BOTH(OUT_STR, key_oplist, value_oplist)(                        \
  static inline void                                                          \
  M_C(name, _out_str)(FILE *file, tree_t const t1)                            \
  {                                                                           \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, t1);                            \
    M_ASSERT (file != NULL);                                                  \
    fputc ('[', file);                                                        \
    bool commaToPrint = false;                                                \
    it_t it;                                                                  \
    for (M_C(name, _it)(it, t1) ;                                             \
         !M_C(name, _end_p)(it);                                              \
         M_C(name, _next)(it)){                                               \
      if (commaToPrint)                                                       \
        fputc (M_GET_SEPARATOR key_oplist, file);                             \
      commaToPrint = true;                                                    \
      subtype_t const *ref1 = M_C(name, _cref)(it);                           \
      M_IF(isMap)(                                                            \
                  M_CALL_OUT_STR(key_oplist, file, *ref1->key_ptr);           \
                  fputc (':', file);                                          \
                  M_CALL_OUT_STR(value_oplist, file, *ref1->value_ptr)        \
                  ,                                                           \
                  M_CALL_OUT_STR(key_oplist, file, *ref1);                    \
                                                                        );    \
    }                                                                         \
    fputc (']', file);                                                        \
  }                                                                           \
  , /* no out_str */ )                                                        \
                                                                              \
  M_IF_METHOD_BOTH(PARSE_STR, key_oplist, value_oplist)(                      \
  static inline bool                                                          \
  M_C(name, _parse_str)(tree_t t1, const char str[], const char **endp)       \
  {                                                                           \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, t1);                            \
    M_ASSERT (str != NULL);                                                   \
    M_C(name,_reset)(t1);                                                     \
    bool success = false;                                                     \
    int c = *str++;                                                           \
    if (M_UNLIKELY (c != '[')) goto exit;                                     \
    c = *str++;                                                               \
    if (M_UNLIKELY (c == ']')) { success = true; goto exit;}                  \
    if (M_UNLIKELY (c == 0)) goto exit;                                       \
    str--;                                                                    \
    key_t key;                                                                \
    M_CALL_INIT(key_oplist, key);                                             \
    M_IF(isMap)(value_t value;                                                \
                M_CALL_INIT(value_oplist, value);                             \
    , /* No isMap */)                                                         \
    do {                                                                      \
      bool b = M_CALL_PARSE_STR(key_oplist, key, str, &str);                  \
      do { c = *str++; } while (isspace(c));                                  \
      if (b == false) goto exit_clear;                                        \
      M_IF(isMap)(if (c != ':') goto exit_clear;                              \
                  b = M_CALL_PARSE_STR(value_oplist, value, str, &str);       \
      do { c = *str++; } while (isspace(c));                                  \
      if (b == false || c == 0) goto exit_clear;                              \
      M_C(name, _set_at)(t1, key, value);                                     \
      ,                                                                       \
      M_C(name, _push)(t1, key);                                              \
      )                                                                       \
    } while (c == M_GET_SEPARATOR key_oplist);                                \
    success = (c == ']');                                                     \
  exit_clear:                                                                 \
    M_CALL_CLEAR(key_oplist, key);                                            \
    M_IF(isMap)(M_CALL_CLEAR(value_oplist, value);   , /* No isMap */ )       \
  exit:                                                                       \
    if (endp) *endp = str;                                                    \
    return success;                                                           \
  }                                                                           \
  , /* no parse_str */ )                                                      \
                                                                              \
  M_IF_METHOD_BOTH(IN_STR, key_oplist, value_oplist)(                         \
  static inline bool                                                          \
  M_C(name, _in_str)(tree_t t1, FILE *file)                                   \
  {                                                                           \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, t1);                            \
    M_ASSERT (file != NULL);                                                  \
    M_C(name,_reset)(t1);                                                     \
    int c = fgetc(file);                                                      \
    if (M_UNLIKELY (c != '[')) return false;                                  \
    c = fgetc(file);                                                          \
    if (M_UNLIKELY (c == ']')) return true;                                   \
    if (M_UNLIKELY (c == EOF)) return false;                                  \
    ungetc(c, file);                                                          \
    key_t key;                                                                \
    M_CALL_INIT (key_oplist, key);                                            \
    M_IF(isMap)(value_t value;                                                \
                M_CALL_INIT (value_oplist, value);                            \
                ,)                                                            \
    do {                                                                      \
      bool b = M_CALL_IN_STR(key_oplist, key, file);                          \
      do { c = fgetc(file); } while (isspace(c));                             \
      if (b == false) break;                                                  \
      M_IF(isMap)(if (c!=':') break;                                          \
                  b = M_CALL_IN_STR(value_oplist,value, file);                \
                  do { c = fgetc(file); } while (isspace(c));                 \
                  if (b == false || c == EOF) break;                          \
                  M_C(name, _set_at)(t1, key, value)                          \
                  ,                                                           \
                  M_C(name, _push)(t1, key)                                   \
                  );                                                          \
    } while (c == M_GET_SEPARATOR key_oplist);                                \
    M_CALL_CLEAR(key_oplist, key);                                            \
    M_IF(isMap)(M_CALL_CLEAR(value_oplist, value);                            \
                ,)                                                            \
    return c == ']';                                                          \
  }                                                                           \
  , /* no in_str */ )                                                         \
                                                                              \
  M_IF_METHOD_BOTH(OUT_SERIAL, key_oplist, value_oplist)(                     \
  static inline m_serial_return_code_t                                        \
  M_C(name, _out_serial)(m_serial_write_t f, tree_t const t1)                 \
  {                                                                           \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, t1);                            \
    M_ASSERT (f != NULL && f->m_interface != NULL);                           \
    m_serial_return_code_t ret;                                               \
    m_serial_local_t local;                                                   \
    subtype_t const *item;                                                    \
    bool first_done = false;                                                  \
    it_t it;                                                                  \
    /* Format is different between associative container                      \
       & set container */                                                     \
    M_IF(isMap)(                                                              \
                ret = f->m_interface->write_map_start(local, f, t1->size);    \
                for (M_C(name, _it)(it, t1) ;                                 \
                     !M_C(name, _end_p)(it);                                  \
                     M_C(name, _next)(it)){                                   \
                  item = M_C(name, _cref)(it);                                \
                  if (first_done)                                             \
                    ret |= f->m_interface->write_map_next(local, f);          \
                  ret |= M_CALL_OUT_SERIAL(key_oplist, f, *item->key_ptr);    \
                  ret |= f->m_interface->write_map_value(local, f);           \
                  ret |= M_CALL_OUT_SERIAL(value_oplist, f, *item->value_ptr); \
                  first_done = true;                                          \
                }                                                             \
                ret |= f->m_interface->write_map_end(local, f);               \
                ,                                                             \
                ret = f->m_interface->write_array_start(local, f, t1->size);  \
                for (M_C(name, _it)(it, t1) ;                                 \
                     !M_C(name, _end_p)(it);                                  \
                     M_C(name, _next)(it)){                                   \
                  item = M_C(name, _cref)(it);                                \
                  if (first_done)                                             \
                    ret |= f->m_interface->write_array_next(local, f);        \
                  ret |= M_CALL_OUT_SERIAL(key_oplist, f, *item);             \
                  first_done = true;                                          \
                }                                                             \
                ret |= f->m_interface->write_array_end(local, f);             \
                                                                        )     \
      return ret & M_SERIAL_FAIL;                                             \
  }                                                                           \
  , /* no OUT_SERIAL */ )                                                     \
                                                                              \
  M_IF_METHOD_BOTH(IN_SERIAL, key_oplist, value_oplist)(                      \
  static inline m_serial_return_code_t                                        \
  M_C(name, _in_serial)(tree_t t1, m_serial_read_t f)                         \
  {                                                                           \
    M_BPTR33_CONTRACT(N, isMulti, key_oplist, t1);                            \
    M_ASSERT (f != NULL && f->m_interface != NULL);                           \
    m_serial_local_t local;                                                   \
    m_serial_return_code_t ret;                                               \
    size_t estimated_size = 0;                                                \
    key_t key;                                                                \
    M_C(name,_reset)(t1);                                                     \
    M_IF(isMap)(                                                              \
                value_t value;                                                \
                ret = f->m_interface->read_map_start(local, f, &estimated_size); \
                if (M_UNLIKELY (ret != M_SERIAL_OK_CONTINUE)) return ret;     \
                M_CALL_INIT(key_oplist, key);                                 \
                M_CALL_INIT (value_oplist, value);                            \
                do {                                                          \
                  ret = M_CALL_IN_SERIAL(key_oplist, key, f);                 \
                  if (ret != M_SERIAL_OK_DONE)     return M_SERIAL_FAIL;      \
                  ret = f->m_interface->read_map_value(local, f);             \
                  if (ret != M_SERIAL_OK_CONTINUE) return M_SERIAL_FAIL;      \
                  ret = M_CALL_IN_SERIAL(value_oplist, value, f);             \
                  if (ret != M_SERIAL_OK_DONE)     return M_SERIAL_FAIL;      \
                  M_C(name, _set_at)(t1, key, value);                         \
                } while ((ret = f->m_interface->read_map_next(local, f)) == M_SERIAL_OK_CONTINUE); \
                M_CALL_CLEAR(key_oplist, key);                                \
                M_CALL_CLEAR(value_oplist, value);                            \
                ,                                                             \
                ret = f->m_interface->read_array_start(local, f, &estimated_size); \
                if (M_UNLIKELY (ret != M_SERIAL_OK_CONTINUE)) return ret;     \
                M_CALL_INIT(key_oplist, key);                                 \
                do {                                                          \
                  ret = M_CALL_IN_SERIAL(key_oplist, key, f);                 \
                  if (ret != M_SERIAL_OK_DONE) { break; }                     \
                  M_C(name, _push)(t1, key);                                  \
                } while ((ret = f->m_interface->read_array_next(local, f)) == M_SERIAL_OK_CONTINUE); \
                M_CALL_CLEAR(key_oplist, key);                                \
                ) /* End of IF isMap */                                       \
      return ret;                                                             \
  }                                                                           \
  , /* no in_serial */ )                                                      \


#if M_USE_SMALL_NAME
#define BPTREE_DEF2 M_BPTREE_DEF2
#define BPTREE_DEF2_AS M_BPTREE_DEF2_AS
#define BPTREE_DEF M_BPTREE_DEF
#define BPTREE_DEF_AS M_BPTREE_DEF_AS
#define BPTREE_MULTI_DEF2 M_BPTREE_MULTI_DEF2
#define BPTREE_MULTI_DEF2_AS M_BPTREE_MULTI_DEF2_AS
#define BPTREE_MULTI_DEF M_BPTREE_MULTI_DEF
#define BPTREE_MULTI_DEF_AS M_BPTREE_MULTI_DEF_AS
#define BPTREE_OPLIST M_BPTREE_OPLIST
#define BPTREE_OPLIST2 M_BPTREE_OPLIST2
#endif

#endif
