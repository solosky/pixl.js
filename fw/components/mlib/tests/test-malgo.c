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
#include "m-list.h"
#include "m-i-list.h"
#include "m-array.h"
#include "m-string.h"
#include "m-deque.h"
#include "m-dict.h"
#include "m-tuple.h"
#include "m-algo.h"

#include "test-obj.h"

typedef struct over_s {
  unsigned long data;
  ILIST_INTERFACE(ilist_over, over_s);
} over_t;

// TODO: Test Intrusive List
// HASHMAP, RBTREE, B+TREE: Is this useful?
ARRAY_DEF(array_int, int)
#define M_OPL_array_int_t() ARRAY_OPLIST(array_int)
LIST_DEF(list_int, int)
ILIST_DEF(ilist_over, over_t, M_POD_OPLIST)
LIST_DEF(list_string, string_t, STRING_OPLIST)
DEQUE_DEF(deque_obj, testobj_t, TESTOBJ_CMP_OPLIST)
#define M_OPL_deque_obj_t() DEQUE_OPLIST(deque_obj, TESTOBJ_CMP_OPLIST)
DICT_DEF2(dict_obj, string_t, STRING_OPLIST, testobj_t, TESTOBJ_OPLIST)
LIST_DUAL_PUSH_DEF(dlist_int, int)
DICT_DEF2(dict_int, string_t, int)
#define M_OPL_dict_int_t() DICT_OPLIST(dict_int, STRING_OPLIST, M_BASIC_OPLIST)
ARRAY_DEF(array_uint, unsigned int)
#define M_OPL_array_uint_t() ARRAY_OPLIST(array_uint)

TUPLE_DEF2(person, (age, int), (name, string_t) )
#define M_OPL_person_t() TUPLE_OPLIST(person, M_BASIC_OPLIST, STRING_OPLIST)
ARRAY_DEF(array_person, person_t)
#define M_OPL_array_person_t() ARRAY_OPLIST(array_person, M_OPL_person_t() )

#include "coverage.h"
START_COVERAGE
ALGO_DEF(algo_array, array_int_t)
ALGO_DEF(algo_list,  LIST_OPLIST(list_int))
ALGO_DEF(algo_over,  ILIST_OPLIST(ilist_over, M_POD_OPLIST))
ALGO_DEF(algo_string, LIST_OPLIST(list_string, STRING_OPLIST))
ALGO_DEF(algo_deque, deque_obj_t)
ALGO_DEF(algo_dict, DICT_OPLIST(dict_obj, STRING_OPLIST, TESTOBJ_OPLIST))
END_COVERAGE
ALGO_DEF(algo_dlist, LIST_OPLIST(list_int))

// Needs to be included ***AFTER*** so that the algorithms can be generated without Function Object before.
#include "m-funcobj.h"
ALGO_DEF(algo_array_fo, array_int_t)
ALGO_DEF(algo_list_fo,  LIST_OPLIST(list_int))


/* Helper functions */
int g_min, g_max, g_count;

static void g_f(int n)
{
  assert (g_min <= n && n <= g_max);
  g_count++;
}

static void func_map(int *d, int n)
{
  assert (g_min <= n && n <= g_max);
  g_count++;
  *d = n * n;
}

static void func_reduce(int *d, int n)
{
  *d += n;
}

static bool func_test_42(int d)
{
  return d == 42;
}

static bool func_test_101(int d)
{
  return d == 101;
}

static bool func_test_pos(int d)
{
  return d >= 0;
}

static bool func_test_both_even_or_odd(int a, int b)
{
  return (a&1) == (b&1);
}

#define start_with(pattern, item)                \
  string_start_with_str_p((item).key, (pattern))

#define get_value(out, item) ((out) = (item).value)

/* Tests starts */
static void test_list(void)
{
  list_int_t l;
  list_int_init(l);
  for(int i = 0; i < 100; i++)
    list_int_push_back (l, i);
  assert( algo_list_contain_p(l, 62) == true);
  assert( algo_list_contain_p(l, -1) == false);
  
  assert( algo_list_count(l, 1) == 1);
  list_int_push_back (l, 17);
  assert( algo_list_count(l, 17) == 2);
  assert( algo_list_count(l, -1) == 0);

  assert (algo_list_all_of_p(l, func_test_42) == false);
  assert (algo_list_any_of_p(l, func_test_42) == true);
  assert (algo_list_none_of_p(l, func_test_42) == false);
  assert (algo_list_all_of_p(l, func_test_101) == false);
  assert (algo_list_any_of_p(l, func_test_101) == false);
  assert (algo_list_none_of_p(l, func_test_101) == true);
  assert (algo_list_all_of_p(l, func_test_pos) == true);
  assert (algo_list_any_of_p(l, func_test_pos) == true);
  assert (algo_list_none_of_p(l, func_test_pos) == false);

#define f(x) assert((x) >= 0 && (x) < 100);
  ALGO_FOR_EACH(l, LIST_OPLIST(list_int), f);
#define g(y, x) assert((x) >= 0 && (x) < y);
  ALGO_FOR_EACH(l, LIST_OPLIST(list_int), g, 100);

  assert( algo_list_count_if(l, func_test_42) == 1);
  assert( algo_list_count_if(l, func_test_101) == 0);

  M_LET(tmp, LIST_OPLIST(list_int)) {
    g_min = 0;
    g_max = 99;
    g_count = 0;
    algo_list_transform(tmp, l, func_map);
    assert(g_count == 101);
    assert(list_int_size(tmp) == 101);
    for(int i = 0; i < 100; i++) {
      assert(*list_int_get(tmp, (size_t) i) == i * i);
    }
    assert(*list_int_get(tmp, 100) == 17 * 17);
  }

  int *p = algo_list_min(l);
  assert(p != NULL && *p == 0);
  p = algo_list_max(l);
  assert(p != NULL && *p == 99);
  int *p2;
  algo_list_minmax(&p, &p2, l);
  assert(p != NULL && *p == 0);
  assert(p2 != NULL && *p2 == 99);
  
  list_int_push_back (l, 3);
  list_int_it_t it1, it2;
  algo_list_find (it1, l, 3);
  assert (!list_int_end_p (it1));
  algo_list_find_last (it2, l, 3);
  assert (!list_int_end_p (it2));
  assert (!list_int_it_equal_p(it1, it2));

  algo_list_find_if(it1, l, func_test_101);
  assert (list_int_end_p (it1));
  algo_list_find_if(it1, l, func_test_42);
  assert (!list_int_end_p (it1));
  assert (*list_int_cref(it1) == 42);

  algo_list_mismatch(it1, it2, l, l);
  assert (list_int_end_p (it1));
  assert (list_int_end_p (it2));
  
  for(int i = -100; i < 100; i+=2)
    list_int_push_back (l, i);

  assert (!algo_list_sort_p(l));
  algo_list_sort(l);
  assert (algo_list_sort_p(l));

  list_int_clear(l);

  list_int_init(l);
  for(int i = 1; i <= 5; i++) list_int_push_back(l, i);
  assert (list_int_size(l) == 5);

  algo_list_remove_val(l, 3);
  assert (list_int_size(l) == 4);
  algo_list_find (it1, l, 3);
  assert (list_int_end_p (it1));
  algo_list_remove_val(l, 3);
  assert (list_int_size(l) == 4);

  algo_list_remove_if(l, func_test_42);
  assert (list_int_size(l) == 4);
  list_int_push_back(l, 42);
  list_int_push_back(l, 43);
  list_int_push_back(l, 42);
  algo_list_remove_if(l, func_test_42);
  assert (list_int_size(l) == 5);
  algo_list_find (it1, l, 42);
  assert (list_int_end_p (it1));
  
  list_int_clear(l);

  M_LET( (l1, 1, 3, 5), (l2, 1, 2, 4), (l3, 1, 2, 3, 4, 5), LIST_OPLIST(list_int, M_BASIC_OPLIST)) {
    algo_list_sort_union(l1, l2);
    assert(list_int_equal_p (l1, l3));
  }

  M_LET( (l1, 1, 30, 31), (l2, 2, 4, 31, 32), (l3, 1, 2, 4, 30, 31, 32), LIST_OPLIST(list_int, M_BASIC_OPLIST)) {
    algo_list_sort_union(l1, l2);
    assert(list_int_equal_p (l1, l3));
  }

  M_LET( (l1, 4, 28, 29, 30, 34), (l2, 1, 3, 31, 32, 33), (l3, 1, 3, 4, 28, 29, 30, 31, 32, 33, 34), LIST_OPLIST(list_int, M_BASIC_OPLIST)) {
    algo_list_sort_union(l1, l2);
    assert(list_int_equal_p (l1, l3));
  }

  M_LET( (l1, 1, 4, 28, 29, 31, 34), (l2, 1, 3, 31, 32, 33), (l3, 1, 31), LIST_OPLIST(list_int, M_BASIC_OPLIST)) {
    algo_list_sort_intersect(l1, l2);
    assert(list_int_equal_p (l1, l3));
  }

  M_LET( (l1, 4, 28, 29, 31, 34), (l2, 1, 3, 31, 32, 33), (l3, (31)), LIST_OPLIST(list_int, M_BASIC_OPLIST)) {
    algo_list_sort_intersect(l1, l2);
    assert(list_int_equal_p (l1, l3));
  }

  M_LET( (l1, 1, 4, 28, 29, 31, 34), (l2, 3, 32, 33), l3, LIST_OPLIST(list_int, M_BASIC_OPLIST)) {
    algo_list_sort_intersect(l1, l2);
    assert(list_int_equal_p (l1, l3));
  }

}

static void test_array(void)
{
  array_int_t l;
  array_int_init(l);
  for(int i = 0; i < 100; i++)
    array_int_push_back (l, i);
  assert( algo_array_contain_p(l, 62) == true);
  assert( algo_array_contain_p(l, -1) == false);
  assert( algo_array_sort_p(l) == true);

  assert (algo_array_all_of_p(l, func_test_42) == false);
  assert (algo_array_any_of_p(l, func_test_42) == true);
  assert (algo_array_none_of_p(l, func_test_42) == false);
  assert (algo_array_all_of_p(l, func_test_101) == false);
  assert (algo_array_any_of_p(l, func_test_101) == false);
  assert (algo_array_none_of_p(l, func_test_101) == true);
  assert (algo_array_all_of_p(l, func_test_pos) == true);
  assert (algo_array_any_of_p(l, func_test_pos) == true);
  assert (algo_array_none_of_p(l, func_test_pos) == false);

  assert( algo_array_count(l, 1) == 1);
  array_int_push_back (l, 17);
  assert( algo_array_count(l, 17) == 2);
  assert( algo_array_count(l, -1) == 0);
  assert( algo_array_sort_p(l) == false);

  assert( algo_array_count_if(l, func_test_42) == 1);
  assert( algo_array_count_if(l, func_test_101) == 0);

  array_int_it_t it;
  algo_array_find_last(it, l, 17);
  assert (!array_int_end_p (it));
  assert (array_int_last_p (it));
  algo_array_find_last(it, l, 1742);
  assert (array_int_end_p (it));
  algo_array_find(it, l, 1742);
  assert (array_int_end_p (it));

#define f(x) assert((x) >= 0 && (x) < 100);
  ALGO_FOR_EACH(l, ARRAY_OPLIST(array_int), f);
  ALGO_FOR_EACH(l, array_int_t, f);

  g_min = 0;
  g_max = 99;
  g_count = 0;
  algo_array_for_each(l, g_f);
  assert(g_count == 101);

  int n;
  algo_array_reduce(&n, l, func_reduce);
  assert(n == 100*99/2+17);

  g_min = 0;
  g_max = 99;
  g_count = 0;
  algo_array_map_reduce(&n, l, func_reduce, func_map);
  assert(g_count == 101);
  assert(n == (328350 + 17*17));

  M_LET(tmp, array_int_t) {
    g_count = 0;
    algo_array_transform(tmp, l, func_map);
    assert(g_count == 101);
    assert(array_int_size(tmp) == 101);
    for(int i = 0; i < 100; i++) {
      assert(*array_int_get(l, (size_t) i) == i);
      assert(*array_int_get(tmp, (size_t) i) == i * i);
    }
    assert(*array_int_get(l, 100) == 17);
    assert(*array_int_get(tmp, 100) == 17 * 17);

    array_int_reset(tmp);
#define FT(d,x) ((d) = (x) + 1)
    ALGO_TRANSFORM(tmp, array_int_t, l, array_int_t, FT);
    assert(array_int_size(tmp) == 101);
    for(int i = 0; i < 100; i++) {
      assert(*array_int_get(l, (size_t) i) == i);
      assert(*array_int_get(tmp, (size_t) i) == i + 1);
    }
    
    array_int_reset(tmp);
#define FT2(d,x,a) ((d) = (x) + (a))
    ALGO_TRANSFORM(tmp, array_int_t, l, array_int_t, FT2, 17);
    assert(array_int_size(tmp) == 101);
    for(int i = 0; i < 100; i++) {
      assert(*array_int_get(l, (size_t) i) == i);
      assert(*array_int_get(tmp, (size_t) i) == i + 17);
    }

  }

  int *min, *max;
  assert (*algo_array_min(l) == 0);
  assert (*algo_array_max(l) == 99);
  algo_array_minmax(&min, &max, l);
  assert (*min == 0);
  assert (*max == 99);
  array_int_push_back (l, 1742);
  array_int_push_back (l, -17);
  assert (*algo_array_min(l) == -17);
  assert (*algo_array_max(l) == 1742);
  algo_array_minmax(&min, &max, l);
  assert (*min == -17);
  assert (*max == 1742);
  assert( algo_array_sort_p(l) == false);

  algo_array_sort(l);
  assert( algo_array_sort_p(l) == true);
  assert (array_int_size(l) == 103);
  algo_array_uniq(l);
  assert (array_int_size(l) == 102);
  assert( algo_array_sort_p(l) == true);

  M_LET(l2, array_int_t) {
    array_int_set(l2, l);
    array_int_it_t it1, it2;
    algo_array_mismatch(it1, it2, l, l2);
    assert (array_int_end_p (it1));
    assert (array_int_end_p (it2));
    array_int_pop_back(NULL, l2);
    array_int_push_back(l2, 159);
    algo_array_mismatch(it1, it2, l, l2);
    assert (!array_int_end_p (it1));
    assert (!array_int_end_p (it2));
    assert (*array_int_cref(it1) == 1742);
    assert (*array_int_cref(it2) == 159);
    algo_array_mismatch_if(it1, it2, l, l2, func_test_both_even_or_odd);
    assert (!array_int_end_p (it1));
    assert (!array_int_end_p (it2));
    assert (*array_int_cref(it1) == 1742);
    assert (*array_int_cref(it2) == 159);
    array_int_pop_back(NULL, l2);
    array_int_push_back(l2, 152);
    algo_array_mismatch_if(it1, it2, l, l2, func_test_both_even_or_odd);
    assert (array_int_end_p (it1));
    assert (array_int_end_p (it2));
  }

  array_int_reset(l);
  assert (algo_array_min(l) == NULL);
  assert (algo_array_max(l) == NULL);
  algo_array_minmax(&min, &max, l);
  assert (min == NULL);
  assert (max == NULL);
  assert (algo_array_sort_p(l) == true);
  algo_array_uniq(l);
  assert (array_int_size(l) == 0);
  assert( algo_array_sort_p(l) == true);

  for(int i = -14025; i < 324035; i+=17)
    array_int_push_back(l, i);
  for(int i = -14025; i < 324035; i+=7)
    array_int_push_back(l, i);
  assert( algo_array_sort_p(l) == false);
  array_int_special_stable_sort(l);
  assert( algo_array_sort_p(l) == true);
  
  array_int_clear(l);

  array_int_init(l);
  for(int i = 1; i <= 5; i++) array_int_push_back(l, i);
  assert (array_int_size(l) == 5);
  assert (algo_array_sort_p(l) == true);
  assert (algo_array_sort_dsc_p(l) == false);
  algo_array_sort_dsc(l);
  assert (algo_array_sort_p(l) == false);
  assert (algo_array_sort_dsc_p(l) == true);

  array_int_clear(l);

  M_LET( (arr, 1, 5, 34), array_int_t) {
    assert (array_int_size(arr) == 3);
    assert (algo_array_sort_p(arr) == true);
    assert (algo_array_sort_dsc_p(arr) == false);

    algo_array_fill(arr, 1789);
    assert(*array_int_get(arr, 0) == 1789);
    assert(*array_int_get(arr, 1) == 1789);
    assert(*array_int_get(arr, 2) == 1789);

    algo_array_fill_a(arr, 1789, 1);
    assert(*array_int_get(arr, 0) == 1789);
    assert(*array_int_get(arr, 1) == 1790);
    assert(*array_int_get(arr, 2) == 1791);

    algo_array_fill_a(arr, 0, 2);
    assert(*array_int_get(arr, 0) == 0);
    assert(*array_int_get(arr, 1) == 2);
    assert(*array_int_get(arr, 2) == 4);

    algo_array_fill_n(arr, 5, 1742);
    assert (array_int_size(arr) == 5);
    assert(*array_int_get(arr, 0) == 1742);
    assert(*array_int_get(arr, 1) == 1742);
    assert(*array_int_get(arr, 2) == 1742);
    assert(*array_int_get(arr, 3) == 1742);
    assert(*array_int_get(arr, 4) == 1742);

    algo_array_fill_an(arr, 4, -1, 3);
    assert (array_int_size(arr) == 4);
    assert(*array_int_get(arr, 0) == -1);
    assert(*array_int_get(arr, 1) == 2);
    assert(*array_int_get(arr, 2) == 5);
    assert(*array_int_get(arr, 3) == 8);
  }
}

static void test_string(void)
{
  list_string_t l;
  list_string_init(l);
  string_t s;
  string_init_set_str(s, "Hello, World, John");
  algo_string_split(l, s, ',');
  list_string_pop_back (&s, l);
  assert (string_equal_str_p(s, "Hello"));
  list_string_pop_back (&s, l);
  assert (string_equal_str_p(s, " World"));
  list_string_pop_back (&s, l);
  assert (string_equal_str_p(s, " John"));
  assert (list_string_empty_p(l));

  string_set_str(s, "Hello,,John");
  algo_string_split(l, s, ',');
  list_string_pop_back (&s, l);
  assert (string_equal_str_p(s, "Hello"));
  list_string_pop_back (&s, l);
  assert (string_equal_str_p(s, ""));
  list_string_pop_back (&s, l);
  assert (string_equal_str_p(s, "John"));
  assert (list_string_empty_p(l));

  list_string_push_back (l, STRING_CTE("John"));
  list_string_push_back (l, STRING_CTE("Who"));
  list_string_push_back (l, STRING_CTE("Is"));
  algo_string_join(s, l, STRING_CTE("-"));
  // NOTE: List is reverse... Is-it really ok?
  assert (string_equal_str_p(s, "Is-Who-John"));

  string_clear(s);
  
  list_string_clear(l);

  M_LET( (a, (13, ("Jean") ),(14, ("Michael"))), array_person_t ) {
    assert( array_person_size(a) == 2);
    assert( (*array_person_get(a, 0))->age == 13 );
    assert( string_equal_str_p( (*array_person_get(a, 0))->name, "Jean") );
    assert( (*array_person_get(a, 1))->age == 14 );
    assert( string_equal_str_p( (*array_person_get(a, 1))->name, "Michael") );
  }
}

static void test_extract(void)
{
  list_int_t l;
  list_int_init (l);
  for(int i = -100; i < 100; i++)
    list_int_push_back (l, i);
  list_int_reverse(l);
  array_int_t a;
  array_int_init(a);

  ALGO_EXTRACT(a, ARRAY_OPLIST(array_int), l, LIST_OPLIST(list_int));
  assert(array_int_size(a) == 200);
  assert(*array_int_get(a, 0) == -100);
  assert(*array_int_get(a, 199) == 99);
  
#define cond(d) ((d) > 0)
  ALGO_EXTRACT(a, ARRAY_OPLIST(array_int), l, LIST_OPLIST(list_int), cond);
  assert(array_int_size(a) == 99);
  assert(*array_int_get(a, 0) == 1);
  assert(*array_int_get(a, 98) == 99);

#define cond2(c, d) ((d) > (c))
  ALGO_EXTRACT(a, ARRAY_OPLIST(array_int), l, LIST_OPLIST(list_int), cond2, 10);
  assert(array_int_size(a) == 89);
  assert(*array_int_get(a, 0) == 11);
  assert(*array_int_get(a, 88) == 99);

  int dst = 0;
#define inc(d, c) (d) += (c)
  ALGO_REDUCE(dst, a, ARRAY_OPLIST(array_int), inc);
  assert (dst == 100*99/2-10*11/2);
  ALGO_REDUCE(dst, a, array_int_t, inc);
  assert (dst == 100*99/2-10*11/2);
#define sqr(d, c) (d) = (c)*(c)
  ALGO_REDUCE(dst, a, ARRAY_OPLIST(array_int), inc, sqr);
  assert (dst == 327965);
#define sqr2(d, f, c) (d) = (f) * (c)
  ALGO_REDUCE(dst, a, ARRAY_OPLIST(array_int), inc, sqr2, 4);
  assert (dst == (100*99/2-10*11/2) *4 );

  ALGO_REDUCE(dst, a, ARRAY_OPLIST(array_int), sum);
  assert (dst == 100*99/2-10*11/2);
  ALGO_REDUCE(dst, a, ARRAY_OPLIST(array_int), add);
  assert (dst == 100*99/2-10*11/2);
  ALGO_REDUCE(dst, a, ARRAY_OPLIST(array_int), and);
  assert (dst == 0);
  ALGO_REDUCE(dst, a, ARRAY_OPLIST(array_int), or);
  assert (dst == 127);

  unsigned long long dst_l = 0;
  // To avoid warnings about sign conversion between int and unsigned long long
  #define my_set(a, b) do { (a) = (unsigned int) (b); } while (0)
  #define my_sum(a, b) do { (a) += (unsigned int) (b); } while (0)
  ALGO_REDUCE( (dst_l, M_OPEXTEND(M_STANDARD_OPLIST, SET(my_set), TYPE(unsigned long long))), a, array_int_t, my_sum);
  assert (dst_l == 100*99/2-10*11/2);

  array_int_reset(a);
  for(int i = 1; i < 10; i++)
    array_int_push_back(a, i);
  ALGO_REDUCE(dst, a, ARRAY_OPLIST(array_int), product);
  assert (dst == 362880);

  array_int_clear(a);
  list_int_clear(l);

  M_LET(keys, array_int_t)
    M_LET( (m, (STRING_CTE("foo"), 1), (STRING_CTE("bar"), 42), (STRING_CTE("bluez"), 7), (STRING_CTE("stop"), 789) ), tmp, dict_int_t) {
    int s;
    /* Extract all elements of 'm' that starts with 'b' */
    ALGO_EXTRACT(tmp, dict_int_t, m, dict_int_t, start_with, "b");
    /* Extract the values of theses elements */
    ALGO_TRANSFORM(keys, array_int_t, tmp, dict_int_t, get_value);
    /* Sum theses values */
    ALGO_REDUCE(s, keys, array_int_t, sum);
    assert(s == 49); 
  }

}

ARRAY_DEF(aint, int)
LIST_DEF(lint, int)
#define M_OPL_aint_t() ARRAY_OPLIST(aint)
#define M_OPL_lint_t() LIST_OPLIST(lint)

static void test_insert(void)
{
  M_LET( (a, 1, 2, 3, 4), aint_t)
    M_LET( (b, -1, -2, -3), aint_t) {
    aint_it_t i;
    aint_it(i, a);
    // Insert it after first element of the array.
    ALGO_INSERT_AT(a, aint_t, i, b, aint_t);
    M_LET( (c, 1, -1, -2, -3, 2, 3, 4), aint_t) {
      assert (aint_equal_p (c, a));
    }
  }

  M_LET( (a, 1, 2, 3, 4), lint_t)
    M_LET( (b, -1, -2, -3), aint_t) {
    lint_it_t i;
    lint_it(i, a);
    // Insert it after first element (aka back)
    ALGO_INSERT_AT(a, lint_t, i, b, aint_t);
    M_LET( (c, 1, -1, -2, -3, 2, 3, 4), lint_t) {
      assert (lint_equal_p (c, a));
    }
  }

  M_LET( (a, 1, 2, 3, 4), lint_t)
    M_LET( (b, -1, -2, -3), lint_t) {
    lint_it_t i;
    lint_it(i, a);
    // Insert it after first element (aka back)
    ALGO_INSERT_AT(a, lint_t, i, b, lint_t);
    M_LET( (c, 1, -1, -2, -3, 2, 3, 4), lint_t) {
      assert (lint_equal_p (c, a));
    }
  }

}

static void test_string_utf8(void)
{
  M_LET( (s, ("H€llo René Chaînôr¬") ), string_t)
    M_LET( (ref, 72, 8364, 108, 108, 111, 32, 82, 101, 110, 233, 32, 67, 104, 97, 238, 110, 244, 114, 172), a, array_uint_t) {
    // Convert the string into an array of unicode.
    ALGO_EXTRACT(a, array_uint_t, s, string_t);
    // Compare the converted string into the reference array
    assert(array_uint_equal_p(a, ref));
  }
}

/* Define some function objects */
FUNC_OBJ_INS_DEF(fo_test, algo_array_fo_test_obj,
		 (a), {
		   return a == self->x;
		 }, (x, int))
#define M_OPL_fo_test_t() FUNC_OBJ_INS_OPLIST(fo_test, M_BASIC_OPLIST)

FUNC_OBJ_INS_DEF(fo_eq, algo_array_fo_eq_obj,
		 (a, b), {
		   return a == b && self->x != a;
		 }, (x, int))
#define M_OPL_fo_eq_t() FUNC_OBJ_INS_OPLIST(fo_eq, M_BASIC_OPLIST)

FUNC_OBJ_INS_DEF(fo_cmp, algo_array_fo_cmp_obj,
		 (a, b), {
		   return a < b ? -self->x : a > b ? self->x : 0;
		 }, (x, int))
#define M_OPL_fo_cmp_t() FUNC_OBJ_INS_OPLIST(fo_cmp, M_BASIC_OPLIST)

FUNC_OBJ_INS_DEF(fol_cmp, algo_list_fo_cmp_obj,
		 (a, b), {
		   return a < b ? -self->x : a > b ? self->x : 0;
		 }, (x, int))
#define M_OPL_fol_cmp_t() FUNC_OBJ_INS_OPLIST(fol_cmp, M_BASIC_OPLIST)

  static void test_fo(void)
{
  M_LET(tab, array_int_t) {
    for(int i = 0; i < 10; i++)
      array_int_push_back(tab, i);
    array_int_push_back(tab, 5);

    array_int_it_t it;
    M_LET( (obj, 6), fo_test_t)
      algo_array_fo_find_fo(it, tab, fo_test_as_interface(obj));
    assert(!array_int_end_p(it));
    M_LET( (obj, 11), fo_test_t)
      algo_array_fo_find_fo(it, tab, fo_test_as_interface(obj));
    assert(array_int_end_p(it));

    M_LET( (obj, 11), fo_test_t)
      assert( 0 == algo_array_fo_count_fo(tab, fo_test_as_interface(obj)));
    M_LET( (obj, 5), fo_test_t)
      assert( 2 == algo_array_fo_count_fo(tab, fo_test_as_interface(obj)));

    M_LET( (obj, 3), fo_test_t)
      assert( false == algo_array_fo_all_of_fo_p(tab, fo_test_as_interface(obj)));
    M_LET( (obj, 3), fo_test_t)
      assert( true == algo_array_fo_any_of_fo_p(tab, fo_test_as_interface(obj)));
    M_LET( (obj, 3), fo_test_t)
      assert( false == algo_array_fo_none_of_fo_p(tab, fo_test_as_interface(obj)));
    M_LET( (obj, -1), fo_test_t)
      assert( true == algo_array_fo_none_of_fo_p(tab, fo_test_as_interface(obj)));

    array_int_push_back(tab, -1);
    M_LET( (obj, 1), fo_cmp_t)
      algo_array_fo_sort_fo(tab, fo_cmp_as_interface(obj));
    M_LET ( (ref, -1, 0, 1, 2, 3, 4, 5, 5, 6, 7, 8, 9), array_int_t)
      assert( array_int_equal_p(tab, ref));
  }

  M_LET(tab, LIST_OPLIST(list_int)) {
    for(int i = 0; i < 10; i++)
      list_int_push_back(tab, i);
    list_int_push_back(tab, 5);
    list_int_push_back(tab, -1);
    M_LET( (obj, 1), fol_cmp_t)
      algo_list_fo_sort_fo(tab, fol_cmp_as_interface(obj));
    M_LET ( (ref, -1, 0, 1, 2, 3, 4, 5, 5, 6, 7, 8, 9), LIST_OPLIST(list_int)) {
      assert( list_int_equal_p(tab, ref));
    }
  }  
}

int main(void)
{
  test_list();
  test_array();
  test_string();
  test_extract();
  test_insert();
  test_string_utf8();
  test_fo();
  exit(0);
}
