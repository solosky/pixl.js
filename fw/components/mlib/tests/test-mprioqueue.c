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
#include <assert.h>
#include "test-obj.h"
#include "m-string.h"
#include "m-prioqueue.h"

#include "coverage.h"
START_COVERAGE
PRIOQUEUE_DEF(int_pqueue, int)
END_COVERAGE

static inline bool testobj_equal2_p(const testobj_t z1, const testobj_t z2)
{
  return z1->a == z2->a;
}

PRIOQUEUE_DEF(obj_pqueue, testobj_t, M_OPEXTEND(TESTOBJ_CMP_OPLIST, EQUAL(testobj_equal2_p) ))

#define OUT_DOUBLE(f, d)      fprintf(f, "%f", d)
#define IN_DOUBLE(d, f)       m_core_fscan_double(d, f)
#define GET_STR_DOUBLE(str, d, append) (append ? string_cat_printf : string_printf)(str, "%f", d)
#define PARSE_DOUBLE(x, s, e) m_core_parse_double(x, s, e)
#define double_OPLIST         M_OPEXTEND(M_BASIC_OPLIST,        \
                                         OUT_STR(OUT_DOUBLE), IN_STR(IN_DOUBLE  M_IPTR), GET_STR(GET_STR_DOUBLE), PARSE_STR(PARSE_DOUBLE M_IPTR))
PRIOQUEUE_DEF_AS(PrioDouble, PrioDouble, PrioDoubleIt, double, double_OPLIST)
#define M_OPL_PrioDouble() PRIOQUEUE_OPLIST(PrioDouble, double_OPLIST)

static void test1(void)
{
  int x;
  int_pqueue_t p;
  int_pqueue_init(p);
  assert (int_pqueue_empty_p(p));
  assert (int_pqueue_size(p) == 0);
  int_pqueue_push(p, 10);
  assert (!int_pqueue_empty_p(p));
  assert (int_pqueue_size(p) == 1);
  int_pqueue_push(p, 60);
  int_pqueue_push(p, 40);
  int_pqueue_push(p, 5);
  int_pqueue_push(p, 30);
  assert (int_pqueue_size(p) == 5);
  assert (*int_pqueue_front(p) == 5);
  int_pqueue_pop(&x, p);
  assert (int_pqueue_size(p) == 4);
  assert (x == 5);
  int_pqueue_pop(&x, p);
  assert (int_pqueue_size(p) == 3);
  assert (x == 10);
  int_pqueue_pop(&x, p);
  assert (x == 30);
  int_pqueue_pop(&x, p);
  assert (x == 40);
  int_pqueue_pop(&x, p);
  assert (x == 60);
  assert (int_pqueue_empty_p(p));
  assert (int_pqueue_size(p) == 0);

  int_pqueue_push(p, 10);
  int_pqueue_push(p, 30);
  int_pqueue_pop(&x, p);
  assert (x == 10);
  int_pqueue_push(p, 5);
  int_pqueue_pop(&x, p);
  assert (x == 5);
  int_pqueue_push(p, 17);
  bool b = int_pqueue_erase(p, 17);
  assert(b);
  assert (int_pqueue_size(p) == 1);
  b = int_pqueue_erase(p, 1742);
  assert(!b);
  b = int_pqueue_erase(p, 30);
  assert(b);
  assert (int_pqueue_size(p) == 0);

  int_pqueue_push(p, 30);
  int_pqueue_push(p, 40);
  int_pqueue_push(p, 10);
  int_pqueue_push(p, 50);
  int_pqueue_push(p, 20);
  assert (int_pqueue_size(p) == 5);
  int_pqueue_erase(p, 30);
  int_pqueue_push(p, 30);
  assert (*int_pqueue_front(p) == 10);
  int_pqueue_update(p, 40, 15);
  assert (*int_pqueue_front(p) == 10);
  int_pqueue_update(p, 20, 5);
  assert (*int_pqueue_front(p) == 5);
  int_pqueue_update(p, 5, 25);
  assert (*int_pqueue_front(p) == 10);

  int_pqueue_clear(p);
}

static void test2(void)
{
  int_pqueue_t p;
  int_pqueue_init(p);
  for(int i = -100; i < 100; i++) {
    int_pqueue_push(p, i);
  }
  assert (int_pqueue_size(p) == 200);
  for(int i = -200; i < -100; i++) {
    int_pqueue_push(p, i);
  }
  assert (int_pqueue_size(p) == 300);
  for(int i = 100; i < 200; i++) {
    int_pqueue_push(p, i);
  }
  assert (int_pqueue_size(p) == 400);
  for(int i = -200; i < 200; i++) {
    int x;
    int_pqueue_pop(&x, p);
    assert (x == i);
  }
  
  assert (int_pqueue_size(p) == 0);
  for(int i = -100; i < 100; i++) {
    int_pqueue_push(p, i);
  }
  assert (int_pqueue_size(p) == 200);
  for(int i = -100; i < 100; i++) {
    bool b = int_pqueue_erase(p, i);
    assert (b);
  }
  assert (int_pqueue_size(p) == 0);

  int_pqueue_clear(p);
}

static void test_update(void)
{
  obj_pqueue_t p;
  obj_pqueue_init(p);
  const testobj_t *front;
  M_LET( x, y, z, TESTOBJ_OPLIST) {
    testobj_set_ui(x, 7);
    testobj_set_id(x, 1);
    obj_pqueue_push(p, x);

    testobj_set_ui(x, 30);
    testobj_set_id(x, 2);
    obj_pqueue_push(p, x);

    testobj_set_ui(x, 40);
    testobj_set_id(x, 3);
    obj_pqueue_push(p, x);

    testobj_set_ui(x, 10);
    testobj_set_id(x, 4);
    obj_pqueue_push(p, x);

    testobj_set_ui(x, 20);
    testobj_set_id(x, 5);
    obj_pqueue_push(p, x);

    testobj_set_ui(x, 3);
    testobj_set_id(x, 6);
    obj_pqueue_push(p, x);

    testobj_set_ui(x, 35);
    testobj_set_id(x, 7);
    obj_pqueue_push(p, x);

    front = obj_pqueue_front(p);
    assert ( (*front)->a == 6);

    testobj_set_ui(y, 8);
    testobj_set_id(y, 5);
    obj_pqueue_update(p, y, y);

    front = obj_pqueue_front(p);
    assert ( (*front)->a == 6);

    testobj_set_ui(y, 1);
    testobj_set_id(y, 5);
    obj_pqueue_update(p, y, y);

    front = obj_pqueue_front(p);
    assert ( (*front)->a == 5);

    testobj_set_ui(y, 4);
    testobj_set_id(y, 5);
    obj_pqueue_update(p, y, y);

    front = obj_pqueue_front(p);
    assert ( (*front)->a == 6);

    testobj_set_ui(y, 4);
    testobj_set_id(y, 6);
    obj_pqueue_erase(p, y);

    front = obj_pqueue_front(p);
    assert ( (*front)->a == 5);

    testobj_set_ui(y, 100);
    testobj_set_id(y, 5);
    obj_pqueue_update(p, y, y);

    front = obj_pqueue_front(p);
    assert ( (*front)->a == 1);

    testobj_set_ui(y, 4);
    testobj_set_id(y, 2);
    obj_pqueue_erase(p, y);

    front = obj_pqueue_front(p);
    assert ( (*front)->a == 1);

    testobj_set_ui(y, 4);
    testobj_set_id(y, 3);
    obj_pqueue_erase(p, y);

    front = obj_pqueue_front(p);
    assert ( (*front)->a == 1);

    testobj_set_ui(y, 4);
    testobj_set_id(y, 4);
    obj_pqueue_erase(p, y);

    front = obj_pqueue_front(p);
    assert ( (*front)->a == 1);

    testobj_set_ui(y, 4);
    testobj_set_id(y, 1);
    obj_pqueue_erase(p, y);

    front = obj_pqueue_front(p);
    assert ( (*front)->a == 7);

    testobj_set_ui(y, 4);
    testobj_set_id(y, 7);
    obj_pqueue_erase(p, y);

    front = obj_pqueue_front(p);
    assert ( (*front)->a == 5);

    obj_pqueue_emplace_ui(p, 0);
    front = obj_pqueue_front(p);
    assert ( (*front)->ptr[0] == 0);
  }
  obj_pqueue_clear(p);
}

static void test_it(void)
{
  int_pqueue_t p, q;

  int_pqueue_init(p);
  for(int i = -10; i < 10; i++) {
    int_pqueue_push(p, i*i);
  }
  assert(int_pqueue_size(p) == 20);

  int_pqueue_init_set(q, p);
  assert(int_pqueue_equal_p(p, q));
  assert(int_pqueue_size(q) == 20);
  int_pqueue_push(q, 43);
  assert(int_pqueue_size(p) == 20);
  assert(int_pqueue_size(q) == 21);
  int_pqueue_swap(p, q);
  assert(int_pqueue_size(p) == 21);
  assert(int_pqueue_size(q) == 20);
  int_pqueue_swap(p, q);
  assert(int_pqueue_size(p) == 20);
  assert(int_pqueue_size(q) == 21);
  int_pqueue_reset(q);
  assert(!int_pqueue_equal_p(p, q));
  assert(int_pqueue_empty_p(q));
  assert(!int_pqueue_empty_p(p));

  int_pqueue_set(q, p);
  assert(int_pqueue_equal_p(p, q));
  assert(int_pqueue_size(q) == 20);

  int_pqueue_move(q, p);
  assert(int_pqueue_size(q) == 20);
  int_pqueue_init_move(p, q);
  assert(int_pqueue_size(p) == 20);

  int_pqueue_it_t it, it2;
  int i = 0;
  const int tab[] = {0 , 1 , 1 , 16 , 4 , 4 , 9 , 25 , 49 , 64 , 9 , 81 , 25 , 36 , 16 , 100 , 36 , 49 , 64 , 81};
  assert (int_pqueue_size(p) == sizeof(tab)/sizeof(tab[0]) );
  for(int_pqueue_it(it, p) ; 
      !int_pqueue_end_p(it) ;
      int_pqueue_next(it) ) {
      assert(tab[i] == *int_pqueue_cref(it) );
      i++;
    }

  int_pqueue_it_end(it, p);
  assert(int_pqueue_end_p(it)) ;

  int_pqueue_it_last(it, p);
  assert(!int_pqueue_end_p(it)) ;
  int_pqueue_next(it);
  assert(int_pqueue_end_p(it)) ;

  int_pqueue_previous(it);
  int_pqueue_it_set(it2, it);
  assert(int_pqueue_last_p(it2));
  assert(int_pqueue_it_equal_p(it, it2));
  int_pqueue_previous(it2);
  assert(!int_pqueue_it_equal_p(it, it2));

  int_pqueue_clear(p);
}

static void test_double(void)
{
  M_LET( (tab, 0.0, 1.0, 2.0, 3.0), PrioDouble) {
    double ref = 0.0;
    // NOTE: The binary heap is sorted in the same natural order that the sorted array
    for M_EACH(i, tab, PrioDouble) {
      assert (*i == ref);
      ref += 1.0;
    }
  }
}

static void test_io(void)
{
  PrioDouble q1, q2;
  string_t str;
  PrioDouble_init(q1);
  PrioDouble_init(q2);
  string_init(str);

  // Test empty
  FILE *f = m_core_fopen("a-mprioqueue.dat", "wt");
  if (!f) abort();
  PrioDouble_out_str(f, q1);
  fclose(f);
  f = m_core_fopen ("a-mprioqueue.dat", "rt");
  if (!f) abort();
  bool b = PrioDouble_in_str (q2, f);
  assert (b == true);
  assert (PrioDouble_equal_p (q1, q2));
  fclose(f);

  PrioDouble_get_str(str, q1, false);
  assert(string_equal_str_p(str, "[]"));
  const char *endp;
  b= PrioDouble_parse_str(q2, string_get_cstr(str), &endp);
  assert(b);
  assert(*endp == 0);
  assert (PrioDouble_equal_p (q1, q2));

  PrioDouble_clear(q1);
  PrioDouble_clear(q2);
  string_clear(str);
}

int main(void)
{
  test1();
  test2();
  test_update();
  test_double();
  test_it();
  test_io();
  exit(0);
}
