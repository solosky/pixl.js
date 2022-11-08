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
   message in this case.
   It tests invalid oplist in an _OPLIST constructor of a container.
*/
#include "m-core.h"

#if !defined(TEST)
# warning Please define TEST variable to select the test to run

#elif TEST == 1

#include "m-array.h"

ARRAY_DEF(array, int)
ARRAY_DEF(array2, array_t, ARRAY_OPLIST(array, DEFAULT_OPLIST))

#elif TEST == 2

#include "m-list.h"

LIST_DEF(list, int)
LIST_DEF(list2, list_t, LIST_OPLIST(list, DEFAULT_OPLIST))

#elif TEST == 3

#include "m-deque.h"

DEQUE_DEF(deque, int)
DEQUE_DEF(deque2, deque_t, DEQUE_OPLIST(deque, DEFAULT_OPLIST))

#elif TEST == 4

#include "m-bptree.h"

BPTREE_DEF(bptree, 4, int)
BPTREE_DEF(bptree2, 5, bptree_t, BPTREE_OPLIST(bptree, DEFAULT_OPLIST))

#elif TEST == 5

#include "m-bptree.h"

BPTREE_DEF2(bptree, 4, int, int)
BPTREE_DEF2(bptree2, 5, bptree_t, BPTREE_OPLIST2(bptree, DEFAULT_OPLIST, M_BASIC_OPLIST), int, M_BASIC_OPLIST)

#elif TEST == 6

#include "m-bptree.h"

BPTREE_DEF2(bptree, 4, int, int)
BPTREE_DEF2(bptree2, 5, bptree_t, BPTREE_OPLIST2(bptree, M_BASIC_OPLIST, DEFAULT_OPLIST), int, M_BASIC_OPLIST)

#elif TEST == 7

#include "m-buffer.h"

BUFFER_DEF(buffer, int, 4, BUFFER_THREAD_UNSAFE)
BUFFER_DEF(buffer2, buffer_t, 5, BUFFER_QUEUE, BUFFER_OPLIST(buffer, DEFAULT_OPLIST))
  
#elif TEST == 8

#include "m-array.h"
#include "m-concurrent.h"

ARRAY_DEF(array, int)
CONCURRENT_DEF(array2, array_t, ARRAY_OPLIST(array, M_BASIC_OPLIST))
ARRAY_DEF(array3, array2_t, CONCURRENT_OPLIST(array2, ARRAY_OPLIST(array, DEFAULT_OPLIST)))

#elif TEST == 9

#include "m-dict.h"

DICT_DEF2(dict, int, int)
DICT_DEF2(dict2, int, M_BASIC_OPLIST, dict_t, DICT_OPLIST(dict, DEFAULT_OPLIST, M_BASIC_OPLIST) )

#elif TEST == 10

#include "m-dict.h"

DICT_DEF2(dict, int, int)
DICT_DEF2(dict2, int, M_BASIC_OPLIST, dict_t, DICT_OPLIST(dict, M_BASIC_OPLIST, DEFAULT_OPLIST) )

#elif TEST == 11

#include "m-dict.h"
#include "m-array.h"
  
DICT_SET_DEF(dict, int)
ARRAY_DEF(array, dict_t, DICT_SET_OPLIST(dict, DEFAULT_OPLIST) )

#elif TEST == 12

#include "m-i-list.h"
#include "m-concurrent.h"

typedef struct tmp_str_s {
  ILIST_INTERFACE(tmpstr, struct tmp_str_s);
  int n;
} tmp_str_t;
ILIST_DEF(tmpstr, tmp_str_t, (CLEAR(M_NOTHING_DEFAULT)))
CONCURRENT_DEF(array, tmpstr_t, ILIST_OPLIST(tmpstr, X()))

#elif TEST == 13

#include "m-i-shared.h"
#include "m-concurrent.h"

typedef struct tmp_str_s {
  ISHARED_PTR_INTERFACE(tmpstr, struct tmp_str_s);
  int n;
} tmp_str_t;
ISHARED_PTR_DEF(tmpstr, tmp_str_t, (CLEAR(M_NOTHING_DEFAULT)))
CONCURRENT_DEF(array, tmpstr_t, ISHARED_PTR_OPLIST(tmpstr, X()))

#elif TEST == 14

#include "m-prioqueue.h"

PRIOQUEUE_DEF(prioqueue, int)
PRIOQUEUE_DEF(prioqueue2, prioqueue_t, PRIOQUEUE_OPLIST(prioqueue, M_BASIC_OPLIST()))

#elif TEST == 15

#include "m-rbtree.h"

RBTREE_DEF(rbtree, int)
RBTREE_DEF(rbtree2, rbtree_t, RBTREE_OPLIST(rbtree, M_BASIC_OPLIST2))

#elif TEST == 16

#include "m-shared.h"

SHARED_PTR_DEF(shared, int)
SHARED_PTR_DEF(shared2, shared_t, SHARED_PTR_OPLIST(shared, M_BASIC_OPLISTop))

#elif TEST == 17

#include "m-snapshot.h"

SNAPSHOT_SPSC_DEF(shared, int)
SNAPSHOT_SPSC_DEF(shared2, shared_t, SNAPSHOT_OPLIST(shared, M_BASIC_OPLIST x))

#elif TEST == 18

#include "m-tuple.h"

TUPLE_DEF2(aggregate, (x, int), (z, float))
TUPLE_DEF2(struct2, (a, aggregate_t, TUPLE_OPLIST(aggregate, DEFAULT_OPLIST, M_BASIC_OPLIST)))

#elif TEST == 19

#include "m-tuple.h"

TUPLE_DEF2(aggregate, (x, int), (z, float))
TUPLE_DEF2(struct2, (a, aggregate_t, TUPLE_OPLIST(aggregate, M_BASIC_OPLIST, M_BASIC_OPLIST() )))

#elif TEST == 20

#include "m-variant.h"

VARIANT_DEF2(aggregate, (x, int), (z, float))
VARIANT_DEF2(struct2, (a, aggregate_t, VARIANT_OPLIST(aggregate, DEFAULT_OPLIST, M_BASIC_OPLIST)))

#elif TEST == 21

#include "m-variant.h"

VARIANT_DEF2(aggregate, (x, int), (z, float))
VARIANT_DEF2(struct2, (a, aggregate_t, VARIANT_OPLIST(aggregate, M_BASIC_OPLIST, DEFAULT_OPLIST)))

#elif TEST == 22

#include "m-funcobj.h"

FUNC_OBJ_ITF_DEF(itf, int, int)
FUNC_OBJ_INS_DEF(ins, itf, (x), { return x * self->a + self->b; }, (a, int, M_BASIC_OPLIST), (b, int, M_BASIC_OPLIST))
#define M_OPL_ins_t() FUNC_OBJ_INS_OPLIST(ins, M_BASIC_OPLIST, DEFAULT_OPLIST)

int f(void) {
  int r;
  M_LET( (f, 1, 2), ins_t)
    r = itf_call(ins_as_interface(f), 4);
  return r;
}

#elif TEST == 23

#include "m-funcobj.h"

FUNC_OBJ_ITF_DEF(itf, int, int)
FUNC_OBJ_INS_DEF(ins, itf, (x), { return x * self->a + self->b; }, (a, int, M_BASIC_OPLIST), (b, int, M_BASIC_OPLIST))
#define M_OPL_ins_t() FUNC_OBJ_INS_OPLIST(ins, DEFAULT_OPLIST, M_BASIC_OPLIST)

int f(void) {
  int r;
  M_LET( (f, 1, 2), ins_t)
    r = itf_call(ins_as_interface(f), 4);
  return r;
}

#elif TEST == 24

#include "m-tuple.h"
#include "m-string.h"

TUPLE_DEF2(aggregate, (x, int), (z, string_t))
#define M_OPL_aggregate_t() TUPLE_OPLIST(aggregate, int, string_t)
TUPLE_DEF2(struct2, (a, aggregate_t))

#else
# warning TEST variable is out of range.
#endif

