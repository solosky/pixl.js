/*
 * MLIB - TEST module
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
#include <stdio.h>

#include "m-i-list.h"

typedef struct test_s {
  int n;
  ILIST_INTERFACE (ilist_tname, struct test_s);
  ILIST_INTERFACE (ilist_free, struct test_s);
} test_t;


#include "coverage.h"
START_COVERAGE
ILIST_DEF(ilist_tname, test_t, M_POD_OPLIST)
END_COVERAGE

ILIST_DEF(ilist_free, test_t, (DEL(free)))

typedef struct testd_s {
  double n;
  ILIST_INTERFACE (ListDouble, struct testd_s);
} TestDouble;

ILIST_DEF_AS(ListDouble, ListDouble, ListDoubleIt, TestDouble, M_OPEXTEND(M_POD_OPLIST, DEL(0)))
#define M_OPL_ListDouble() ILIST_OPLIST(ListDouble, M_POD_OPLIST)

static void test(void)
{
  test_t x1, x2, x3;
  ilist_tname_t list;

  x1.n = 1;
  x2.n = 2;
  x3.n = 3;
  ilist_tname_init_field(&x1);
  ilist_tname_init_field(&x2);
  ilist_tname_init_field(&x3);

  ilist_tname_init(list);
  
  assert (ilist_tname_size (list) == 0);
  ilist_tname_push_back (list, &x3);
  assert (ilist_tname_size (list) == 1);
  ilist_tname_push_front (list, &x1);
  assert (ilist_tname_size (list) == 2);
  ilist_tname_push_after (&x1, &x2);
  assert (ilist_tname_size (list) == 3);
  assert (ilist_tname_front (list)->n == 1);
  assert (ilist_tname_back (list)->n == 3);
  assert (ilist_tname_next_obj(list, &x1) == &x2);
  assert (ilist_tname_previous_obj(list, &x2) == &x1);
  
  int n = 1;
  for M_EACH(item, list, ILIST_OPLIST(ilist_tname)) {
      assert (n == item->n);
      n++;
    }
  assert (n == 4);
  ilist_tname_it_t it1, it2;
  ilist_tname_it_last(it1, list);
  assert (!ilist_tname_end_p(it1));
  assert (ilist_tname_last_p(it1));
  ilist_tname_it_end(it2, list);
  assert (ilist_tname_end_p(it2));
  assert (ilist_tname_last_p(it2));
  assert (!ilist_tname_it_equal_p(it1, it2));
  for(ilist_tname_it_set(it2, it1) ; !ilist_tname_end_p(it2) ; ilist_tname_previous(it2)) {
    const test_t *item = ilist_tname_cref(it2);
    n--;
    assert (n == item->n);
  }
  assert (n == 1);
  ilist_tname_it_set(it2, it1);
  assert (ilist_tname_it_equal_p(it1, it2));

  ilist_tname_t list2;
  ilist_tname_init(list2);
  ilist_tname_move(list2, list);
  assert (ilist_tname_size (list2) == 3);
  ilist_tname_init_move(list, list2);
  assert (ilist_tname_size (list) == 3);
  
  for M_EACH(item, list, ILIST_OPLIST(ilist_tname)) {
      if (item->n == 2)
        ilist_tname_unlink(item);
    }
  assert (ilist_tname_size(list) == 2);
  assert (&x1 == ilist_tname_pop_front(list));
  assert (ilist_tname_size(list) == 1);
  assert (&x3 == ilist_tname_pop_back(list));
  assert (ilist_tname_size(list) == 0);
  assert (ilist_tname_empty_p(list));
  
  for M_EACH(item, list, ILIST_OPLIST(ilist_tname)) {
      (void)item;
      assert(false);
    }

  ilist_tname_push_back (list, &x3);
  assert (ilist_tname_size (list) == 1);
  ilist_tname_push_front (list, &x1);
  assert (ilist_tname_size (list) == 2);
  ilist_tname_push_after (&x1, &x2);
  assert (ilist_tname_size (list) == 3);
  assert (ilist_tname_front (list)->n == 1);
  assert (ilist_tname_back (list)->n == 3);

  ilist_tname_it(it1, list);
  ilist_tname_next(it1);
  ilist_tname_remove(list, it1);
  assert (ilist_tname_size (list) == 2);
  assert (ilist_tname_cref(it1) == &x3);
  assert (ilist_tname_previous_obj(list, &x3) == &x1);

  ilist_tname_reset(list);
  assert (ilist_tname_empty_p(list));
  
  ilist_tname_clear(list);
}

#define NUM 10
static void test2(void)
{
  test_t x[NUM];
  M_LET(list1, list2, ILIST_OPLIST(ilist_tname)) {
    for(int i = 0; i < NUM; i++)
      x[i].n = i;
    for(int i = 0; i < NUM; i++) {
      ilist_tname_push_back(list1, &x[i]);
    }
    assert (ilist_tname_size(list1) == NUM);
    assert (ilist_tname_size(list2) == 0);
    assert (ilist_tname_equal_p(list1, list2) == false);
    ilist_tname_it_t it;
    ilist_tname_it (it, list1);
    for(int i = 0; i < NUM; i++) {
      assert (ilist_tname_cref(it)->n == i);
      ilist_tname_splice_back (list2, list1, it);
    }
    assert (ilist_tname_size(list1) == 0);
    assert (ilist_tname_size(list2) == NUM);
    assert (ilist_tname_equal_p(list1, list2) == false);
    int n = 0;
    for M_EACH(item, list2, ILIST_OPLIST(ilist_tname)) {
        assert (n == item->n);
        n++;
      }
    assert (n == NUM);

    ilist_tname_it_t it2;
    ilist_tname_it_end (it, list1);
    ilist_tname_it (it2, list2);
    for(int i = 0; i < NUM; i++) {
      assert (ilist_tname_cref(it2)->n == i);
      ilist_tname_splice_at (list1, it, list2, it2);
      assert (ilist_tname_cref(it)->n == i);
    }
    assert (ilist_tname_size(list1) == NUM);
    assert (ilist_tname_size(list2) == 0);
    n = 0;
    for M_EACH(item, list1, ILIST_OPLIST(ilist_tname)) {
        assert (n == item->n);
        n++;
      }
    assert (n == NUM);
    ilist_tname_reverse(list1);
    for M_EACH(item, list1, ILIST_OPLIST(ilist_tname)) {
        n--;
        assert (n == item->n);
      }
    assert (n == 0);
    
  }
  M_LET(list1, list2, ILIST_OPLIST(ilist_tname)) {
    for(int i = 0; i < NUM; i++)
      x[i].n = i;
    for(int i = 0; i < NUM/2; i++) {
      ilist_tname_push_back(list1, &x[i]);
    }
    for(int i = NUM/2; i < NUM; i++) {
      ilist_tname_push_back(list2, &x[i]);
    }
    ilist_tname_splice(list1, list2);
    assert (ilist_tname_size(list1) == NUM);
    assert (ilist_tname_size(list2) == 0);
    int n = 0;
    for M_EACH(item, list1, ILIST_OPLIST(ilist_tname)) {
        assert (n == item->n);
        n++;
      }
    assert (n == NUM);
    
    ilist_tname_swap(list1, list2);
    assert (ilist_tname_size(list2) == NUM);
    assert (ilist_tname_size(list1) == 0);
    n = 0;
    for M_EACH(item, list2, ILIST_OPLIST(ilist_tname)) {
        assert (n == item->n);
        n++;
      }
    assert (n == NUM);
  }
}

static void test_free(void)
{
  ilist_free_t list;
  
  ilist_free_init(list);

  for(int i = 0; i < 1200; i++) {
    test_t *p = M_ASSIGN_CAST(test_t *, malloc(sizeof(test_t)));
    assert(p != NULL);
    ilist_free_init_field(p);
    p->n = i;
    ilist_free_push_back(list, p);
  }
  assert(ilist_free_size(list) == 1200);

  int n = 0;
  for M_EACH(item, list, ILIST_OPLIST(ilist_free, (DEL(free)) )) {
      assert (n == item->n);
      n++;
    }

  ilist_free_clear(list);
}

static void test_double(void)
{
  TestDouble  base[5];
  M_LET( tab, ListDouble) {
    for(int i = 0; i < 5; i++) {
      base[i].n = (double) i;
      ListDouble_init_field(&base[i]);
      ListDouble_push_back(tab, &base[i]);
    }
    double ref = 0.0;
    for M_EACH(i, tab, ListDouble) {
      assert (i->n == ref);
      ref += 1.0;
    }
  }
}

  int main(void)
{
  test();
  test2();
  test_free();
  test_double();
  exit(0);
}
