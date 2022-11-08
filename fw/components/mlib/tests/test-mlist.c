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
#include <stdio.h>
#include "test-obj.h"
#include "m-string.h"
#include "m-list.h"

#include "coverage.h"
START_COVERAGE
LIST_DEF(list_uint, unsigned int)
LIST_DEF(list_mpz, testobj_t, TESTOBJ_OPLIST)
LIST_DUAL_PUSH_DEF(list2_double, double) 
END_COVERAGE
LIST_DUAL_PUSH_DEF(list2_mpz, testobj_t, TESTOBJ_OPLIST)
#define LIST_UINT_OPLIST LIST_OPLIST(list_uint)
#define LIST2_DOUBLE_OPLIST LIST_OPLIST(list2_double)

// Array with the minimum number of methods.
LIST_DEF(list_min_z, testobj_t, (INIT_SET(testobj_init_set), SET(testobj_set), CLEAR(testobj_clear)))
LIST_DUAL_PUSH_DEF(list2_min_z, testobj_t, (INIT_SET(testobj_init_set), SET(testobj_set), CLEAR(testobj_clear)))

LIST_DEF(list_string, string_t)
#define M_OPL_list_string_t() LIST_OPLIST(list_string, STRING_OPLIST)

LIST_DEF_AS(list_double, ListDouble, ListDoubleIt, double)
#define M_OPL_ListDouble() LIST_OPLIST(list_double, M_BASIC_OPLIST)

LIST_DUAL_PUSH_DEF_AS(list_doubleDP, ListDoubleDP, ListDoubleDPIt, double)
#define M_OPL_ListDoubleDP() LIST_OPLIST(list_doubleDP, M_BASIC_OPLIST)

ListDouble   g_array1 = LIST_INIT_VALUE();
ListDoubleDP g_array2 = LIST_DUAL_PUSH_INIT_VALUE();

static void test_uint(void)
{
  list_uint_t v;
  list_uint_init(v);

  assert (list_uint_empty_p(v) == true);

  list_uint_push_back(v, 2);
  assert (list_uint_empty_p(v) == false);
  unsigned int x ;
  list_uint_pop_back(&x, v);
  assert( x == 2);
  assert (list_uint_empty_p(v) == true);
  assert (list_uint_size(v) == 0);

  for(unsigned int i = 0; i < 1000; i ++) {
    list_uint_push_back(v, i);
  }
  assert (list_uint_size(v) == 1000);
  assert (*list_uint_cget(v, 500) == 500);
  assert (*list_uint_cget(v, 999) == 999);

  list_uint_it_t u;
  unsigned int s = 0;
  for(list_uint_it(u, v); !list_uint_end_p(u); list_uint_next(u)) {
    s += *list_uint_cref(u);
  }
  assert (s == 1000*999/2);
  s = 0;
  for M_EACH(item, v, LIST_UINT_OPLIST) {
    s += *item;
  }
  assert (s == 1000*999/2);

  list_uint_insert(v, u, 1996); // Insert in back since u is empty
  assert (list_uint_size(v) == 1001);
  list_uint_it(u, v);
  list_uint_insert(v, u, 1997); // Insert in second place
  assert (list_uint_size(v) == 1002);
  list_uint_pop_back(&x, v);
  assert (x == 1996);
  list_uint_pop_back(&x, v);
  assert (x == 1997);
  assert (list_uint_size(v) == 1000);

  list_uint_push_back (v, x);
  list_uint_pop_back (NULL, v);
  
  list_uint_t v2;
  list_uint_it_t u2;
  list_uint_init(v2);
  list_uint_set(v, v);
  list_uint_set(v2,v);
  for(list_uint_it(u, v), list_uint_it(u2, v2); !list_uint_end_p(u); list_uint_next(u), list_uint_next(u2)) {
    assert(list_uint_end_p(u2) == false);
    assert(*list_uint_cref(u) == *list_uint_cref(u2));
  }

  list_uint_it(u2, v2);   //v2 is the first el
  assert (*list_uint_cref(u2) == 999);
  list_uint_next(u2);  //v2 is the second el
  assert (*list_uint_cref(u2) == 998);
  list_uint_remove(v2, u2);// the second el is deleted
  list_uint_it(u2, v2);   //v2 is the first el
  assert (*list_uint_cref(u2) == 999);
  list_uint_next(u2);  //v2 is the second el
  assert (*list_uint_cref(u2) == 997);
  
  list_uint_it(u2, v2);   //v2 is the first el
  list_uint_remove(v2, u2);// the second el is deleted
  list_uint_it(u2, v2);   //v2 is the first el
  assert (*list_uint_cref(u2) == 997);

  list_uint_reset(v);
  assert (list_uint_empty_p(v) == true);
  assert (list_uint_empty_p(v2) == false);
  assert (list_uint_size(v2) == 998);

  list_uint_set (v, v2);
  list_uint_splice (v, v2);
  assert (list_uint_empty_p(v) == false);
  assert (list_uint_empty_p(v2) == true);
  assert (list_uint_size(v) == 998*2);

  list_uint_reverse(v2);
  assert (list_uint_empty_p(v2) == true);
  list_uint_push_back(v2, 12);
  list_uint_push_back(v2, 17);
  list_uint_reverse(v2);
  list_uint_pop_back(&x, v2);
  assert (x == 12);
  list_uint_pop_back(&x, v2);
  assert (x == 17);
  assert (list_uint_empty_p(v2) == true);

  unsigned int *p = list_uint_push_new(v);
  assert (p != NULL);
  assert (*p == 0);
  *p=17421742;
  assert (*list_uint_back(v) == 17421742);

  size_t s1 = list_uint_size(v);
  size_t s2 = list_uint_size(v2);
  list_uint_swap(v, v2);
  assert (list_uint_size(v) == s2);
  assert (list_uint_size(v2) == s1);

  list_uint_it_end(u, v2);
  assert(list_uint_end_p(u));
  list_uint_it_set(u2, u);
  assert(list_uint_end_p(u2));
  assert(list_uint_it_equal_p(u, u2));
  assert(list_uint_last_p(u2));
  s1 = list_uint_size(v2);
  for(list_uint_it(u, v2), s2 = 0; !list_uint_last_p(u); list_uint_next(u)) {s2++;}
  assert(!list_uint_end_p(u));
  assert (!list_uint_it_equal_p(u, u2));
  list_uint_next(u);
  assert(list_uint_end_p(u));
  assert (s1 == s2 + 1);
  
  list_uint_clear(v);
  list_uint_init_set(v, v2);
  assert(list_uint_equal_p(v,v2));

  list_uint_move (v, v2);
  list_uint_init_move (v2, v);
  list_uint_init_set(v, v2);
  assert(list_uint_equal_p(v,v2));
  assert(list_uint_equal_p(v,v));

  list_uint_reset(v);
  list_uint_push_back (v2, 14562398);
  list_uint_it(u, v2);
  s2 = list_uint_size(v2);
  list_uint_splice_back(v, v2, u);
  assert (list_uint_size(v2) == s2 - 1);
  assert (list_uint_size(v) == 1);
  assert (*list_uint_back(v) == 14562398);
  list_uint_next(u);
  list_uint_splice_back(v, v2, u);
  assert (list_uint_size(v2) == s2 - 2);
  assert (list_uint_size(v) == 2);
  assert (!list_uint_equal_p(v,v2));

  assert (list_uint_hash(v) != 0);

  x = 1478963;
  s = *list_uint_back(v);
  list_uint_push_move(v, &x);
  assert (*list_uint_back(v) == 1478963);
  x = 0;
  list_uint_pop_move(&x, v);
  assert (x == 1478963);
  assert (s == *list_uint_back(v));

  list_uint_reset(v);
  list_uint_reset(v2);
  for(unsigned int i = 0; i < 10; i++)
    list_uint_push_back (v, i);
  list_uint_it (u, v);
  list_uint_it_end (u2, v2);
  for(unsigned int i = 0; i < 10; i++) {
    list_uint_splice_at (v2, u2, v, u);
  }
  assert (list_uint_empty_p(v));
  assert (list_uint_size (v2) == 10);
  s = 9;
  for M_EACH(item, v2, LIST_UINT_OPLIST) {
      assert (*item == s);
      s --;
    }

  list_uint_clear(v);
  list_uint_clear(v2);
}

static void test_mpz(void)
{
  list_mpz_t v;
  testobj_t z, x, s;

  testobj_init (z);
  testobj_init (x);
  testobj_init (s);

  list_mpz_init(v);
  assert (list_mpz_empty_p(v) == true);

  testobj_set_ui (z, 2);
  list_mpz_push_back(v, z);
  assert (list_mpz_empty_p(v) == false);

  list_mpz_pop_back(&x, v);
  assert( testobj_equal_p (x, z));
  assert (list_mpz_empty_p(v) == true);
  assert (list_mpz_size(v) == 0);

  for(unsigned int i = 0; i < 1000; i ++) {
    testobj_set_ui (z, i);
    list_mpz_push_back(v, z);
  }
  assert (list_mpz_size(v) == 1000);

  list_mpz_it_t u;
  testobj_set_ui (s, 0);
  for(list_mpz_it(u, v); !list_mpz_end_p(u); list_mpz_next(u)) {
    testobj_add (s, s, *list_mpz_cref(u));
  }
  assert (testobj_get_ui(s) == 1000*999/2);

  FILE *f = m_core_fopen ("a-mlist.dat", "wt");
  if (!f) abort();
  list_mpz_out_str(f, v);
  fclose (f);

  list_mpz_t list2;
  list_mpz_init(list2);
  f = m_core_fopen ("a-mlist.dat", "rt");
  bool b = list_mpz_in_str (list2, f);
  assert (b == true);
  fclose(f);
  assert (list_mpz_equal_p (v, list2));
  list_mpz_clear(list2);

  // Print using M_FPRINT
  f = m_core_fopen ("a-mlist.dat", "wt");
  if (!f) abort();
  M_FPRINT(f, (v, LIST_OPLIST(list_mpz, TESTOBJ_OPLIST)) );
  fclose (f);

  list_mpz_init(list2);
  f = m_core_fopen ("a-mlist.dat", "rt");
  b = list_mpz_in_str (list2, f);
  assert (b == true);
  fclose(f);
  assert (list_mpz_equal_p (v, list2));
  list_mpz_clear(list2);

  list_mpz_reset(v);
  list_mpz_init(list2);
  M_LET(str, STRING_OPLIST) {
    list_mpz_get_str(str, v, false);
    assert (string_cmp_str (str, "[]") == 0);
    const char *sp;
    b = list_mpz_parse_str(list2, string_get_cstr(str), &sp);
    assert(b);
    assert(*sp == 0);
    assert(list_mpz_equal_p(v, list2));

    testobj_set_ui (z, 17);
    list_mpz_push_back (v, z);
    list_mpz_get_str(str, v, false);
    assert (string_cmp_str (str, "[17]") == 0);
    b = list_mpz_parse_str(list2, string_get_cstr(str), &sp);
    assert(b);
    assert(*sp == 0);
    assert(list_mpz_equal_p(v, list2));

    testobj_set_ui (z, 42);
    list_mpz_push_back (v, z);
    list_mpz_get_str(str, v, true);
    assert (string_cmp_str (str, "[17][42,17]") == 0);
    b = list_mpz_parse_str(list2, string_get_cstr(str), &sp);
    assert(b);
    assert(strcmp(sp, "[42,17]") == 0);
    assert(!list_mpz_equal_p(v, list2));
    b = list_mpz_parse_str(list2, sp, &sp);
    assert(b);
    assert(strcmp(sp, "") == 0);
    assert(list_mpz_equal_p(v, list2));
  }
  
  list_mpz_clear(v);

  /* Test of the emplace functions.
   * 3 functions have been generated for testobj:
   * one from unsigned int (suffix _ui)
   * one from C string (suffix _str)
   * one from testobj_t itself (no suffix)
   */
  list_mpz_reset(list2);
  list_mpz_emplace_back_ui(list2, 42);
  assert(testobj_cmp_ui(*list_mpz_back(list2), 42) == 0);
  list_mpz_emplace_back_ui(list2, 426);
  assert(testobj_cmp_ui(*list_mpz_back(list2), 426) == 0);
  list_mpz_emplace_back_str(list2, "458");
  assert(testobj_cmp_ui(*list_mpz_back(list2), 458) == 0);
  testobj_set_ui(z, 789);
  list_mpz_emplace_back(list2, z);
  assert(testobj_cmp_ui(*list_mpz_back(list2), 789) == 0);

  list_mpz_clear(list2);

  list2_mpz_t listz;
  list2_mpz_init(listz);

  list2_mpz_emplace_back_ui(listz, 45);
  assert(testobj_cmp_ui(*list2_mpz_back(listz), 45) == 0);
  list2_mpz_emplace_front_str(listz, "450");
  assert(testobj_cmp_ui(*list2_mpz_front(listz), 450) == 0);
  assert(testobj_cmp_ui(*list2_mpz_back(listz), 45) == 0);

  list2_mpz_clear(listz);
  
  testobj_clear (z);
  testobj_clear (x);
  testobj_clear (s);
}

static void test_dual_push1(void)
{
  list2_double_t list;
  double d;
  
  list2_double_init(list);
  assert(list2_double_empty_p(list));
  assert(list2_double_size(list) == 0);

  list2_double_push_back(list, 2.0);
  assert(!list2_double_empty_p(list));
  assert (*list2_double_back(list) == 2.0);
  assert (*list2_double_front(list) == 2.0);
  list2_double_pop_back(&d, list);
  assert (d == 2.0);
  assert(list2_double_empty_p(list));
  assert(list2_double_size(list) == 0);

  list2_double_push_front(list, 3.0);
  assert (*list2_double_back(list) == 3.0);
  assert (*list2_double_front(list) == 3.0);
  assert(!list2_double_empty_p(list));
  list2_double_pop_back(&d, list);
  assert (d == 3.0);
  assert(list2_double_empty_p(list));
  assert(list2_double_size(list) == 0);
  
  list2_double_push_back(list, 2.0);
  list2_double_push_back(list, 3.0);
  list2_double_push_front(list, 1.0);
  list2_double_push_front(list, 0.0);
  assert (*list2_double_back(list) == 3.0);
  assert (*list2_double_front(list) == 0.0);
  list2_double_pop_back(&d, list);
  assert (d == 3.0);
  list2_double_pop_back(&d, list);
  assert (d == 2.0);
  list2_double_pop_back(&d, list);
  assert (d == 1.0);
  list2_double_pop_back(&d, list);
  assert (d == 0.0);
  assert(list2_double_empty_p(list));

  list2_double_push_back_new(list);
  list2_double_pop_back(&d, list);
  assert (d == 0.0);
  list2_double_push_front_new(list);
  list2_double_pop_back(&d, list);
  assert (d == 0.0);

  list2_double_push_back_new(list);
  list2_double_pop_back(NULL, list);
  assert(list2_double_empty_p(list));
  list2_double_push_front_new(list);
  list2_double_pop_back(NULL, list);
  assert(list2_double_empty_p(list));

  for(double e = 0.0; e < 1024.0; e += 1.0) {
    if (e < 1000.0)
      list2_double_push_front(list, e);
    if (e >= 24.0 && e < 1000.0) {
      assert (list2_double_size (list) == 25);
    }
    if (e >= 24.0) {
      list2_double_pop_back(&d, list);
      assert (d + 24.0 == e);
    }
  }

  list2_double_push_back(list, 15);
  list2_double_t list2;
  list2_double_init(list2);
  assert(list2_double_empty_p(list2));
  assert(!list2_double_empty_p(list));
  list2_double_swap(list, list2);
  assert(!list2_double_empty_p(list2));
  assert(list2_double_empty_p(list));
  list2_double_clear(list2);

  for(double e = 0.0; e < 1024.0; e += 1.0) {
    list2_double_push_back(list, e);
  }
  list2_double_init_set (list2, list);
  assert(list2_double_equal_p(list, list2));

  list2_double_reverse(list2);
  for(double e = 0.0; e < 1024.0 ; e += 1.0) {
    list2_double_pop_back(&d, list2);
    assert (d == e);
  }
  assert(list2_double_empty_p(list2));

  list2_double_splice(list2, list);
  assert(list2_double_size(list2) == 1024);
  assert(list2_double_size(list) == 0);
  list2_double_set(list, list2);
  assert(list2_double_size(list) == 1024);
  list2_double_splice(list2, list);
  assert(list2_double_size(list2) == 2048);
  assert(list2_double_size(list) == 0);
  
  list2_double_move(list, list2);
  list2_double_clear(list);
}

static void test_dual_it1(void)
{
  list2_double_t list;
  list2_double_it_t it, it2;
  double d;
  
  list2_double_init(list);

  list2_double_it(it, list);
  assert(list2_double_end_p(it));
  list2_double_it_end(it, list);
  assert(list2_double_end_p(it));

  for(double e = 0.0; e < 1024.0 ; e += 1.0)
    list2_double_push_front(list, e);
  
  list2_double_it(it, list);
  assert(!list2_double_end_p(it));
  list2_double_it_set (it2, it);
  assert(list2_double_it_equal_p(it2, it));
  list2_double_it_end(it, list);
  assert(!list2_double_it_equal_p(it2, it));
  assert(list2_double_end_p(it));
  assert(!list2_double_end_p(it2));
  assert(*list2_double_ref(it2) == 0.0);
  
  d = 0.0;
  for(list2_double_it(it, list) ;
      !list2_double_end_p(it)   ;
      list2_double_next(it) ) {
    double e = *list2_double_cref(it);
    assert(e == d);
    if (list2_double_last_p(it))
      assert (d == 1023.0);
    d += 1.0;
  }
  assert (d == 1024.0);

  // Insert in last
  list2_double_it_end(it, list);
  list2_double_insert(list, it, -1.0);
  d = -1.0;
  for(list2_double_it(it, list) ;
      !list2_double_end_p(it)   ;
      list2_double_next(it) ) {
    double e = *list2_double_cref(it);
    assert(e == d);
    if (list2_double_last_p(it))
      list2_double_it_set(it2, it);
    d += 1.0;
  }
  assert (d == 1024.0);
  assert(*list2_double_back(list) == -1.0);
  assert(*list2_double_front(list) == 1023.0);
  
  // Insert in front
  list2_double_insert(list, it2, 1024.0);
  d = -1.0;
  for(list2_double_it(it, list) ;
      !list2_double_end_p(it)   ;
      list2_double_next(it) ) {
    double e = *list2_double_cref(it);
    assert(e == d);
    d += 1.0;
  }
  assert (d == 1025.0);
  assert(*list2_double_back(list) == -1.0);
  assert(*list2_double_front(list) == 1024.0);
  
  // Insert in between
  list2_double_it(it, list) ;
  list2_double_insert(list, it, -0.5);
  d = -1.0;
  for(list2_double_it(it, list) ;
      !list2_double_end_p(it)   ;
      list2_double_next(it) ) {
    double e = *list2_double_cref(it);
    assert(e == d);
    d += (d < 0.0) ? 0.5 : 1.0;
  }
  assert (d == 1025.0);
  assert(*list2_double_back(list) == -1.0);
  assert(*list2_double_front(list) == 1024.0);

  // Remove in between
  list2_double_it(it, list) ;
  list2_double_next(it);
  assert (*list2_double_cref(it) == -0.5);
  list2_double_remove(list, it);
  d = -1.0;
  for(list2_double_it(it, list) ;
      !list2_double_end_p(it)   ;
      list2_double_next(it) ) {
    double e = *list2_double_cref(it);
    assert(e == d);
    if (list2_double_last_p(it))
      list2_double_it_set(it2, it);
    d += 1.0;
  }
  assert (d == 1025.0);
  assert(*list2_double_back(list) == -1.0);
  assert(*list2_double_front(list) == 1024.0);

  // Remove front
  list2_double_remove(list, it2);
  d = -1.0;
  for(list2_double_it(it, list) ;
      !list2_double_end_p(it)   ;
      list2_double_next(it) ) {
    double e = *list2_double_cref(it);
    assert(e == d);
    d += 1.0;
  }
  assert (d == 1024.0);
  assert(*list2_double_back(list) == -1.0);
  assert(*list2_double_front(list) == 1023.0);

  // Remove back
  list2_double_it(it, list);
  list2_double_remove(list, it);
  d = 0.0;
  for(list2_double_it(it, list) ;
      !list2_double_end_p(it)   ;
      list2_double_next(it) ) {
    double e = *list2_double_cref(it);
    assert(e == d);
    d += 1.0;
  }
  assert (d == 1024.0);
  assert(*list2_double_back(list) == 0.0);
  assert(*list2_double_front(list) == 1023.0);

  list2_double_t list2;
  list2_double_init_set(list2, list);

  // Splice back the back of list
  list2_double_it(it, list);
  list2_double_splice_back(list2, list, it);
  assert(*list2_double_back(list) == 1.0);
  assert(*list2_double_cref(it) == 1.0);
  list2_double_it(it2, list2);
  assert(*list2_double_cref(it2) == 0.0);
  list2_double_next(it2);
  assert(*list2_double_cref(it2) == 0.0);
  list2_double_next(it2);
  assert(*list2_double_cref(it2) == 1.0);

  // Splice back an element of list
  list2_double_next(it);
  list2_double_splice_back(list2, list, it);
  assert(*list2_double_back(list) == 1.0);
  assert(*list2_double_cref(it) == 3.0);
  list2_double_it(it2, list2);
  assert(*list2_double_cref(it2) == 2.0);
  list2_double_next(it2);
  assert(*list2_double_cref(it2) == 0.0);
  list2_double_next(it2);
  assert(*list2_double_cref(it2) == 0.0);
  list2_double_next(it2);
  assert(*list2_double_cref(it2) == 1.0);
  
  list2_double_reset(list);
  list2_double_reset(list2);
  for(double i = 0; i < 10; i++)
    list2_double_push_back (list, i);
  list2_double_it (it, list);
  list2_double_it_end (it2, list2);
  for(double i = 0; i < 10; i++) {
    list2_double_splice_at (list2, it2, list, it);
  }
  assert (list2_double_empty_p(list));
  assert (list2_double_size (list2) == 10);
  double s = 9.0;
  for M_EACH(item, list2, LIST2_DOUBLE_OPLIST) {
      assert (*item == s);
      s -= 1.0;
    }

  list2_double_clear(list2);
  list2_double_clear(list);
}

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

LIST_DEF(list_int, uint32_t)
#define M_OPL_list_int_t() LIST_OPLIST(list_int)

static void test_out_default_oplist(void)
{
  M_LET(str, string_t)
    M_LET(list, l2, list_int_t) {
    FILE *f = m_core_fopen ("a-mlist.dat", "wt");
    if (!f) abort();
    list_int_push_back(list, 2);
    list_int_push_back(list, 3);
    list_int_out_str(f, list);
    fclose(f);
    f = m_core_fopen("a-mlist.dat", "rt");
    if (!f) abort();
    char buffer[1024];
    assert (fgets(buffer, 1024, f) == buffer);
    assert (strcmp(buffer, "[3,2]") == 0);
    fclose(f);
    list_int_get_str(str, list, false);
    assert (string_equal_str_p(str, "[3,2]"));
    f = m_core_fopen("a-mlist.dat", "rt");
    if (!f) abort();
    assert (list_int_in_str(l2, f) == true);
    assert (list_int_equal_p(l2, list));
    fclose(f);
  }
  M_LET(str, string_t)
    M_LET( (list, 1, 2, 3, 4, 5), list_int_t) {
    list_int_get_str(str, list, false);
    assert (string_equal_str_p(str, "[1,2,3,4,5]"));
  }
}
#else
static void test_out_default_oplist(void)
{
}
#endif

static void test_double(void)
{
  M_LET( (tab, 0.0, 1.0, 2.0, 3.0), ListDouble) {
    double ref = 0.0;
    for M_EACH(i, tab, ListDouble) {
      assert (*i == ref);
      ref += 1.0;
    }
  }

  M_LET( (tab, 0.0, 1.0, 2.0, 3.0), ListDoubleDP) {
    double ref = 0.0;
    for M_EACH(i, tab, ListDoubleDP) {
      assert (*i == ref);
      ref += 1.0;
    }
  }

  assert( list_double_empty_p (g_array1) );
  assert( list_doubleDP_empty_p (g_array2) );

  list_double_push_back(g_array1, 23.0);
  list_doubleDP_push_front(g_array2, 23.0);
  
  list_double_clear(g_array1);
  list_doubleDP_clear(g_array2);
}

static void test_let_string(void)
{
  M_LET( (y, ("!") ), string_t)                   // string_t y = string_t{"!"}
    M_LET( (z, y), string_t)                      // string_t z = y
    M_LET( (x, ("Hello"), ("%d World", 2), y ), list_string_t) // astr_t x = { string_t{"Hello"}, string_t{"World"}, z }
    M_LET( (xx, x), list_string_t)                // astr_t xx = x
    M_LET( (zz, (("Hellno")) ), list_string_t)     // astr_t zz = { string_t{"Hellno"} }
    M_LET( (zx, (y) ), list_string_t)             // astr_t zx = { z }
    M_LET( (tab_name, STRING_CTE("My"), STRING_CTE("CD"), STRING_CTE("IS"), STRING_CTE("OUT")), list_string_t)
    {
      assert(string_equal_str_p( y, "!"));
      assert(string_equal_str_p( z, "!"));
      list_string_get_str(y, x, false);
      assert(string_equal_str_p(y, "[\"Hello\",\"2 World\",\"!\"]"));
      list_string_get_str(y, xx, false);
      assert(string_equal_str_p(y, "[\"Hello\",\"2 World\",\"!\"]"));
      list_string_get_str(y, zz, false);
      assert(string_equal_str_p(y, "[\"Hellno\"]"));
      list_string_get_str(y, zx, false);
      assert(string_equal_str_p(y, "[\"!\"]"));
      list_string_get_str(y, tab_name, false);
      assert(string_equal_str_p(y, "[\"My\",\"CD\",\"IS\",\"OUT\"]"));
    }
}

int main(void)
{
  test_uint();
  test_mpz();
  test_dual_push1();
  test_dual_it1();
  test_out_default_oplist();
  test_double();
  test_let_string();
  exit(0);
}

