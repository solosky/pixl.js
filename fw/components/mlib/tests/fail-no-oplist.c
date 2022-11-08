/*
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

/* The goal of this test file is to check that invalid constructs
   are properly rejected, and that it generates a helpfull error
   message in this case. */
#include "m-core.h"

#if !defined(TEST)
# warning Please define TEST variable to select the test to run

#elif TEST == 1

/* Test of the call to no default function */
#define OP (INIT(init))

void f(int x, int y) {
  M_CALL_SWAP(OP, x, y);
}

#elif TEST == 2

#define OP (SWAP(0))

void f(int x, int y) {
  M_CALL_SWAP(OP, x, y);
}
     
#elif TEST == 3

#include "m-array.h"
ARRAY_DEF(array, int, DEFAULT_OPLIST)

#elif TEST == 4

#include "m-list.h"
LIST_DEF(list, int, DEFAULT_OPLIST)

#elif TEST == 5

#include "m-list.h"
LIST_DUAL_PUSH_DEF(list, int, DEFAULT_OPLIST)

#elif TEST == 6

#include "m-deque.h"
DEQUE_DEF(deque, int, DEFAULT_OPLIST)

#elif TEST == 7

#include "m-i-list.h"
typedef struct tmp_str_s {
  ILIST_INTERFACE(tmpstr, struct tmp_str_s);
  int n;
} tmp_str_t;
ILIST_DEF(tmpstr, tmp_str_t, DEFAULT_OPLIST)

#elif TEST == 8

#include "m-string.h"
void f(void)
{
  M_LET(x, M_M_STRING_OPLIST) {
  }
}

#elif TEST == 9

#include "m-string.h"
typedef string_t my_type;
#define M_OPL_my_type() MSTRING_OPLIST
void f(void)
{
  M_LET(x, my_type) {
  }
}

#elif TEST == 10

void f(void)
{
  M_LET(x, () ) {
  }
}

#elif TEST == 11

#include "m-algo.h"
#include "m-array.h"

ARRAY_DEF(array, int)
ALGO_DEF(array, M_M_ARRAY_OPLIST(array))

#elif TEST == 12

#include "m-algo.h"
#include "m-array.h"

ARRAY_DEF(array, int)
ALGO_DEF(array, (OPLIST(array)))

#elif TEST == 13

#include "m-bptree.h"

BPTREE_DEF2(tree, 4, int, M_BASIC_OPLIST, int, DEFAULT_OPLIST)

#elif TEST == 14

#include "m-bptree.h"

BPTREE_DEF2(tree, 4, int, DEFAULT_OPLIST, int, M_BASIC_OPLIST)

#elif TEST == 15

#include "m-bptree.h"

BPTREE_DEF(tree, 4, int, DEFAULT_OPLIST)

#elif TEST == 16

#include "m-bptree.h"

BPTREE_MULTI_DEF2(tree, 4, int, DEFAULT_OPLIST, int, M_BASIC_OPLIST)

#elif TEST == 17

#include "m-bptree.h"

BPTREE_MULTI_DEF(tree, 4, int, DEFAULT_OPLIST)

#elif TEST == 18

#include "m-buffer.h"

BUFFER_DEF(buffer, int, 0, BUFFER_QUEUE, DEFAULT_OPLIST)

#elif TEST == 19

#include "m-buffer.h"

QUEUE_MPMC_DEF(buffer, int, BUFFER_QUEUE, DEFAULT_OPLIST)

#elif TEST == 20

#include "m-buffer.h"

QUEUE_SPSC_DEF(buffer, int, BUFFER_QUEUE, DEFAULT_OPLIST)

#elif TEST == 21

#include "m-array.h"
#include "m-concurrent.h"

ARRAY_DEF(array, int)
CONCURRENT_DEF(lock_array, array_t, M_M_ARRAY_OPLIST(array))

#elif TEST == 22

#include "m-array.h"
#include "m-concurrent.h"

ARRAY_DEF(array, int)
CONCURRENT_RP_DEF(lock_array, array_t, MARRAY_OPLIST(array))

#elif TEST == 23

#include "m-dict.h"

DICT_DEF2(array, int, M_BASIC_OPLIST, int, DEFAULT_OPLIST)

#elif TEST == 24

#include "m-dict.h"

DICT_DEF2(array, int, DEFAULT_OPLIST, int, M_BASIC_OPLIST)

#elif TEST == 25

#include "m-dict.h"

DICT_STOREHASH_DEF2(array, int, DEFAULT_OPLIST, int, M_BASIC_OPLIST)

#elif TEST == 26

#include "m-dict.h"

DICT_STOREHASH_DEF2(array, int, M_BASIC_OPLIST, int, DEFAULT_OPLIST)

#elif TEST == 27

#include "m-dict.h"

DICT_OA_DEF2(array, int, DEFAULT_OPLIST, int, M_BASIC_OPLIST)

#elif TEST == 28

#include "m-dict.h"

DICT_OA_DEF2(array, int, M_BASIC_OPLIST, int, DEFAULT_OPLIST)

#elif TEST == 29

#include "m-dict.h"

DICT_SET_DEF(array, int, DEFAULT_OPLIST)

#elif TEST == 30

#include "m-dict.h"

DICT_OASET_DEF(array, int, DEFAULT_OPLIST)

#elif TEST == 31

#include "m-i-shared.h"

typedef struct tmp_s {
  ISHARED_PTR_INTERFACE(tmpi, struct_tmp_s);
  const char *s;
} tmp_t;

ISHARED_PTR_DEF(tmpi, tmp_t, NOT_AN_OPLIST)

#elif TEST == 32

#include "m-prioqueue.h"

PRIOQUEUE_DEF(prio, int, DEFAULT_OPLIST)

#elif TEST == 33

#include "m-rbtree.h"

RBTREE_DEF(tree, int, M_BASIC_OPLIST() )

#elif TEST == 34

#include "m-shared.h"

SHARED_PTR_DEF(shared, int, M_BASIC_OPLIST2 )

#elif TEST == 35

#include "m-shared.h"

SHARED_RESOURCE_DEF(shared, int, M_BASIC_OPLIST3 )

#elif TEST == 36

#include "m-snapshot.h"

SNAPSHOT_SPSC_DEF(snap, int, M_BASIC_OPLIST2 )

#elif TEST == 37

#include "m-snapshot.h"

SNAPSHOT_SPMC_DEF(snap, int, M_BASIC_OPLIST2 )

#elif TEST == 38

#include "m-snapshot.h"

SNAPSHOT_MPMC_DEF(snap, int, M_BASIC_OPLIST8 )

#elif TEST == 39

#include "m-tuple.h"

TUPLE_DEF2(agregate, (x, int, M_BASIC_OPLIST), (y, float, DEFAULT_OPLIST) )

#elif TEST == 40

#include "m-tuple.h"

TUPLE_DEF2(agregate, (x, int, DEFAULT_OPLIST), (y, float) )

#elif TEST == 41

#include "m-variant.h"

VARIANT_DEF2(agregate, (x, int, M_BASIC_OPLIST), (y, float, DEFAULT_OPLIST) )

#elif TEST == 42

#include "m-variant.h"

VARIANT_DEF2(agregate, (x, int, DEFAULT_OPLIST), (y, float) )

#elif TEST == 43

#include "m-funcobj.h"

FUNC_OBJ_ITF_DEF(itf, int, int)

FUNC_OBJ_INS_DEF(ins, itf, (x), { return x * self->a + self->b; }, (a, int, DEFAULT_OPLIST), (b, int, M_BASIC_OPLIST))

#elif TEST == 44

#include "m-funcobj.h"

FUNC_OBJ_ITF_DEF(itf, int, int)

FUNC_OBJ_INS_DEF(ins, itf, (x), { return x * self->a + self->b; }, (a, int), (b, int, M_BASIC_OPLIST() ))


#else
# warning TEST variable is out of range.
#endif

