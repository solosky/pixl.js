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
#include <stdlib.h>
#include "m-string.h"
#include "m-bptree.h"
#include "test-obj.h"
#include "coverage.h"

START_COVERAGE
BPTREE_DEF2(btree, 3, int, M_BASIC_OPLIST, int, M_BASIC_OPLIST)
END_COVERAGE

BPTREE_DEF2(btree_my, 3, testobj_t, TESTOBJ_CMP_OPLIST, testobj_t, TESTOBJ_CMP_OPLIST)
BPTREE_DEF2(btree_string, 5, string_t, STRING_OPLIST, string_t, STRING_OPLIST)
BPTREE_DEF2(btree_int, 17, int, int)

BPTREE_DEF(btree_intset, 13, int)
BPTREE_DEF(btree_myset, 15, testobj_t, TESTOBJ_CMP_OPLIST)

BPTREE_MULTI_DEF2(multimap, 3, int, M_BASIC_OPLIST, int, M_BASIC_OPLIST)
BPTREE_MULTI_DEF(multiset, 6, int, M_BASIC_OPLIST)

BPTREE_DEF2_AS(MapDouble, MapDouble, MapDoubleIt, MapDoubleItRef, 4, double, double)
#define M_OPL_MapDouble() BPTREE_OPLIST2(MapDouble, M_BASIC_OPLIST, M_BASIC_OPLIST)
BPTREE_DEF_AS(SetDouble, SetDouble, SetDoubleIt, 7, double)
#define M_OPL_SetDouble() BPTREE_OPLIST(SetDouble, M_BASIC_OPLIST)
BPTREE_MULTI_DEF2_AS(MultiMapDouble, MultiMapDouble, MultiMapDoubleIt, MultiMapDoubleItRef, 8, double, double)
#define M_OPL_MultiMapDouble() BPTREE_OPLIST2(MultiMapDouble, M_BASIC_OPLIST, M_BASIC_OPLIST)
BPTREE_MULTI_DEF_AS(MultiSetDouble, MultiSetDouble, MultiSetDoubleIt, 9, double)
#define M_OPL_MultiSetDouble() BPTREE_OPLIST(MultiSetDouble, M_BASIC_OPLIST)

static void test1(void)
{
  btree_t b;
  btree_init(b);
  assert (btree_empty_p(b));
  btree_set_at(b, 12, 34);
  assert (!btree_empty_p(b));
  assert (btree_size(b) == 1);
  assert (btree_get(b, 12) != NULL);
  assert (*btree_get(b, 12) == 34);
  btree_set_at(b, 11, 35);
  assert (!btree_empty_p(b));
  assert (btree_size(b) == 2);
  assert (btree_get(b, 11) != NULL);
  assert (*btree_get(b, 11) == 35);
  btree_set_at(b, 1, 36);
  assert (!btree_empty_p(b));
  assert (btree_size(b) == 3);
  assert (btree_get(b, 1) != NULL);
  assert (*btree_get(b, 1) == 36);
  btree_set_at(b, 25, 37);
  assert (!btree_empty_p(b));
  assert (btree_size(b) == 4);
  assert (btree_get(b, 25) != NULL);
  assert (*btree_get(b, 25) == 37);
  btree_set_at(b, -25, 38);
  assert (btree_size(b) == 5);
  assert (btree_get(b, -25) != NULL);
  assert (*btree_get(b, -25) == 38);
  btree_set_at(b, 5, 39);
  assert (btree_size(b) == 6);
  assert (*btree_get(b, -25) == 38);
  assert (*btree_get(b, 5) == 39);
  btree_set_at(b, 5, 40);
  assert (btree_size(b) == 6);
  assert (*btree_get(b, 5) == 40);
  btree_set_at(b, -5, 41);
  assert (*btree_get(b, -5) == 41);
  for(int i = -1000; i < 1000; i+=3) {
    btree_set_at(b, i, 2*i);
    assert (btree_get(b, i) != NULL);
    assert (*btree_get(b, i) == 2*i);
  }
  for(int i = -1000+1; i < 1000; i+=3) {
    btree_set_at(b, i, 2*i);
    assert (btree_get(b, i) != NULL);
    assert (*btree_get(b, i) == 2*i);
  }
  for(int i = -1000+2; i < 1000; i+=3) {
    btree_set_at(b, i, 2*i);
    assert (btree_get(b, i) != NULL);
    assert (*btree_get(b, i) == 2*i);
  }
  for(int i = -1000; i < 1000; i++) {
    assert (btree_get(b, i) != NULL);
    assert (*btree_get(b, i) == 2*i);
  }
  assert (btree_size(b) == 2000);
  bool r = btree_erase(b, 0);
  assert (r == true);
  assert (btree_get(b, 0) == NULL);
  assert (btree_size(b) == 1999);
  r = btree_erase(b, 1);
  assert (r == true);
  assert (btree_get(b, 1) == NULL);
  assert (btree_size(b) == 1998);
  r = btree_erase(b, 2);
  assert (r == true);
  assert (btree_get(b, 2) == NULL);
  assert (btree_size(b) == 1997);
  r = btree_erase(b, 0);
  assert (r == false);
  assert (btree_get(b, 0) == NULL);
  assert (btree_size(b) == 1997);
  btree_erase(b, -1);
  assert (btree_get(b, -1) == NULL);
  btree_erase(b, -2);
  assert (btree_get(b, -2) == NULL);
  btree_erase(b, 3);
  assert (btree_get(b, 3) == NULL);
  btree_erase(b, 4);
  assert (btree_get(b, 4) == NULL);
  btree_erase(b, -3);
  assert (btree_get(b, -3) == NULL);
  btree_erase(b, 5);
  assert (btree_get(b, 5) == NULL);
  btree_erase(b, -5);
  assert (btree_get(b, -5) == NULL);
  btree_erase(b, -4);
  assert (btree_get(b, -4) == NULL);
  btree_erase(b, 6);
  assert (btree_get(b, 6) == NULL);
  btree_erase(b, 7);
  assert (btree_get(b, 7) == NULL);
  for(int i = -1000; i < 0; i++) {
    btree_erase(b, i);
    assert (btree_get(b, i) == NULL);
  }
  for(int i = 0; i < 1000; i++) {
    btree_erase(b, i);
    assert (btree_get(b, i) == NULL);
  }
  assert (btree_size(b) == 0);
  assert (btree_empty_p(b));

  int *p = btree_safe_get(b, 1000);
  assert(p != NULL && *p == 0);
  *p = 23;
  p = btree_safe_get(b, 1000);
  assert(p != NULL && *p == 23);
  
  btree_clear(b);
}

static void test2(void)
{
  btree_t b;
  btree_init(b);
  for(int i = 0; i < 100; i++)
    btree_set_at(b, i, 100*i);
  for(int i = 0; i < 100; i++)
    assert (*btree_cget(b, i) == 100*i);
  assert (btree_size(b) == 100);
  int k;
  bool r = btree_pop_at(NULL, b, 0);
  assert (r == true);
  assert (btree_size(b) == 99);
  r = btree_pop_at(&k, b, 1);
  assert (r == true);
  assert (btree_size(b) == 98);
  assert (k == 100);
  k = -2;
  r = btree_pop_at(&k, b, 1);
  assert (r == false);
  assert (k == -2);
  r = btree_pop_at(NULL, b, 0);
  assert (r == false);
  btree_reset(b);
  assert (btree_size(b) == 0);
  assert (btree_empty_p(b));
  btree_clear(b);
}

static void test3(void)
{
  btree_t b;
  btree_init(b);

  for(int i = 0; i < 1000; i++)
    btree_set_at(b, i, 1000*i);
  assert(btree_size(b) == 1000);
  for(int i = 0; i < 1000; i++) {
    bool r = btree_erase(b, i);
    assert (r == true);
  }
  assert(btree_size(b) == 0);

  for(int i = 1000-1; i >= 0; i--)
    btree_set_at(b, i, 1000*i);
  assert(btree_size(b) == 1000);
  for(int i = 1000-1; i >= 0; i--) {
    bool r = btree_erase(b, i);
    assert (r == true);
  }
  assert(btree_size(b) == 0);

  const int prime[5] = { 3, 5, 11, 13, 17};
  const int prime_size = 5;
  for(int k = 0; k < prime_size; k++) {
    int p = prime[k];
    for(int i = 0; i < 1024; i++) {
      btree_set_at(b, (i*p) % 1024, i);
      assert(*btree_cget(b, (i*p) % 1024) == i);
    }
    assert(btree_size(b) == 1024);
    for(int i = 0; i < 1024; i++) {
      assert(*btree_cget(b, (i*p) % 1024) == i);
    }
    for(int i = 0; i < 1024; i++) {
      bool r = btree_erase(b, (i*p) % 1024);
      assert(r);
      assert(btree_cget(b, (i*p) % 1024) == NULL);
    }
    assert(btree_size(b) == 0);
  }

  for(int k = 0; k < prime_size; k++) {
    int p = prime[k];
    for(int i = 0; i < 512; i++) {
      btree_set_at(b, (i*p) % 1024, i);
      assert(*btree_cget(b, (i*p) % 1024) == i);
    }
    assert(btree_size(b) == 512);
    for(int i = 256; i < 512; i++) {
      bool r = btree_erase(b, (i*p) % 1024);
      assert(r);
      assert(btree_cget(b, (i*p) % 1024) == NULL);
    }
    for(int i = 256; i < 1024; i++) {
      btree_set_at(b, (i*p) % 1024, i);
      assert(*btree_cget(b, (i*p) % 1024) == i);
    }
    assert(btree_size(b) == 1024);
    for(int i = 0; i < 1024; i++) {
      bool r = btree_erase(b, (i*p) % 1024);
      assert(r);
      assert(btree_cget(b, (i*p) % 1024) == NULL);
    }
    assert(btree_size(b) == 0);
  }
  
  btree_clear(b);
}

static void test3_string(void)
{
  string_t key, value;
  string_init(key);
  string_init(value);
  btree_string_t b;
  btree_string_init(b);

  const int prime[6] = { 1, 3, 5, 11, 13, 17};
  const int prime_size = 6;
  for(int k = 0; k < prime_size; k++) {
    int p = prime[k];
    for(int i = 0; i < 1000; i++) {
      string_printf(key, "%d", (i*p) % 1024);
      string_printf(value, "%d", 1000*i);
      btree_string_set_at(b, key, value);
      assert(string_equal_p(value, *btree_string_cget(b, key)));
    }
    assert(btree_string_size(b) == 1000);
    for(int i = 0; i < 1000; i++) {
      string_printf(key, "%d", (i*p) % 1024);
      string_printf(value, "%d", 1000*i);
      assert(string_equal_p(value, *btree_string_cget(b, key)));
      bool r = btree_string_erase(b, key);
      assert (r == true);
    }
    assert(btree_string_size(b) == 0);
  }

  btree_string_clear(b);
  string_clear(key);
  string_clear(value);
}

static void test4(void)
{
  btree_t b1, b2;
  btree_init(b1);

  for(int i = 0; i < 1000; i++)
    btree_set_at(b1, i, 1000*i);
  assert(btree_size(b1) == 1000);
  btree_init_set(b2, b1);
  assert (btree_size(b2) == 1000);
  assert (btree_equal_p(b1, b2));
  for(int i = 0; i < 1000; i++) {
    bool r = btree_erase(b2, i);
    assert (r == true);
    assert (!btree_equal_p(b1, b2));
  }
  assert(btree_size(b2) == 0);

  for(int i = 5000; i < 10000; i++)
    btree_set_at(b2, i, 1000*i);
  assert(btree_size(b2) == 5000);
  btree_set(b2, b1);
  assert(btree_size(b2) == 1000);
  assert (btree_equal_p(b1, b2));
  for(int i = 0; i < 1000; i++) {
    bool r = btree_erase(b2, i);
    assert (r == true);
  }
  assert(btree_size(b2) == 0);

  btree_set(b1, b2);
  assert(btree_size(b1) == 0);
  assert (btree_equal_p(b1, b2));

  btree_clear(b1);
  btree_clear(b2);

  btree_init(b1);
  btree_init(b2);
  for(int i = -50; i < 50; i++)
    btree_set_at(b1, i, 1000*i);
  assert (btree_size(b1) == 100);
  for(int i = -10; i < 10; i++)
    btree_set_at(b2, i, 1000*i);
  assert (btree_size(b2) == 20);
  btree_swap(b1, b2);
  assert (btree_size(b1) == 20);
  assert (btree_size(b2) == 100);  
  btree_move(b1, b2);
  btree_clear(b1);

  
}

static void test5(void)
{
  btree_t b;
  btree_it_t it, it2;
  btree_init(b);

  assert (btree_cmin(b) == NULL);
  assert (btree_cmax(b) == NULL);

  btree_it(it, b);
  assert(btree_end_p(it));
  btree_it_end(it, b);
  assert(btree_end_p(it));

  for(int i = 0; i < 1000; i+=3)
    btree_set_at(b, i, 1000*i);
  for(int i = 1; i < 1000; i+=3)
    btree_set_at(b, i, 1000*i);
  for(int i = 2; i < 1000; i+=3)
    btree_set_at(b, i, 1000*i);
  assert(btree_size(b) == 1000);

  assert (*btree_cmin(b) == 0*1000);
  assert (*btree_cmax(b) == 999*1000);

  int i = 0;
  for(btree_it(it, b) ; !btree_end_p(it); btree_it_set(it2, it) , btree_next(it)) {
    const btree_itref_t *item = btree_cref(it);
    assert (*item->key_ptr == i);
    assert (*item->value_ptr == 1000*i);
    i++;
  }

  assert (i == 1000);
  assert (!btree_it_equal_p(it, it2));
  btree_it_end(it2, b);
  assert (btree_it_equal_p(it, it2));

  i = 500;
  for(btree_it_from(it, b, 500); !btree_it_until_p(it, 600); btree_next(it)) {
    const btree_itref_t *item = btree_cref(it);
    assert (*item->key_ptr == i);
    assert (*item->value_ptr == 1000*i);
    i++;
  }
  assert (i == 600);

  btree_erase (b, 500);
  i = 501;
  for(btree_it_from(it, b, 500); !btree_it_until_p(it, 600); btree_next(it)) {
    const btree_itref_t *item = btree_cref(it);
    assert (*item->key_ptr == i);
    assert (*item->value_ptr == 1000*i);
    i++;
  }
  assert (i == 600);

  btree_it_from(it, b, 1000);
  assert (btree_end_p(it));
  assert (btree_it_until_p(it, 1001));
	  
  btree_clear(b);
}

static void test_io(void)
{
  M_LET(mpz1, mpz2, TESTOBJ_CMP_OPLIST)
  M_LET(str, STRING_OPLIST)
  M_LET(tree1, tree2, BPTREE_OPLIST2(btree_my, TESTOBJ_CMP_OPLIST, TESTOBJ_CMP_OPLIST)) {
    btree_my_get_str(str, tree1, false);
    assert(string_equal_str_p(str, "[]"));
    const char *endp;
    bool b = btree_my_parse_str(tree2, string_get_cstr(str), &endp);
    assert(b);
    assert(*endp == 0);
    assert (btree_my_equal_p (tree1, tree2));

    FILE *f = m_core_fopen ("a-mbptree.dat", "wt");
    if (!f) abort();
    btree_my_out_str(f, tree1);
    fclose (f);

    f = m_core_fopen ("a-mbptree.dat", "rt");
    if (!f) abort();
    b = btree_my_in_str (tree2, f);
    assert (b == true);
    assert (btree_my_equal_p (tree1, tree2));
    fclose(f);

    testobj_set_ui(mpz1, 67);
    testobj_set_ui(mpz2, 670);
    btree_my_set_at(tree1, mpz1, mpz2);
    testobj_set_ui(mpz1, 17);
    testobj_set_ui(mpz2, 170);
    btree_my_set_at(tree1, mpz1, mpz2);

    btree_my_get_str(str, tree1, false);
    assert(string_equal_str_p(str, "[17:170,67:670]"));
    b = btree_my_parse_str(tree2, string_get_cstr(str), &endp);
    assert(b);
    assert(*endp == 0);
    assert (btree_my_equal_p (tree1, tree2));

    f = m_core_fopen ("a-mbptree.dat", "wt");
    if (!f) abort();
    btree_my_out_str(f, tree1);
    fclose (f);

    f = m_core_fopen ("a-mbptree.dat", "rt");
    if (!f) abort();
    b = btree_my_in_str (tree2, f);
    assert (b == true);
    assert (btree_my_equal_p (tree1, tree2));
    fclose(f);
  }
}

static void test_io_set(void)
{
  M_LET(mpz1, TESTOBJ_CMP_OPLIST)
  M_LET(str, STRING_OPLIST)
  M_LET(tree1, tree2, BPTREE_OPLIST(btree_myset, TESTOBJ_CMP_OPLIST)) {
    btree_myset_get_str(str, tree1, false);
    assert(string_equal_str_p(str, "[]"));
    const char *endp;
    bool b = btree_myset_parse_str(tree2, string_get_cstr(str), &endp);
    assert(b);
    assert(*endp == 0);
    assert (btree_myset_equal_p (tree1, tree2));

    FILE *f = m_core_fopen ("a-mbptree.dat", "wt");
    if (!f) abort();
    btree_myset_out_str(f, tree1);
    fclose (f);

    f = m_core_fopen ("a-mbptree.dat", "rt");
    if (!f) abort();
    b = btree_myset_in_str (tree2, f);
    assert (b == true);
    assert (btree_myset_equal_p (tree1, tree2));
    fclose(f);

    testobj_set_ui(mpz1, 67);
    btree_myset_push(tree1, mpz1);
    testobj_set_ui(mpz1, 670);
    btree_myset_push(tree1, mpz1);
    testobj_set_ui(mpz1, 17);
    btree_myset_push(tree1, mpz1);
    testobj_set_ui(mpz1, 170);
    btree_myset_push(tree1, mpz1);
    btree_myset_get_str(str, tree1, false);
    assert(string_equal_str_p(str, "[17,67,170,670]"));
    b = btree_myset_parse_str(tree2, string_get_cstr(str), &endp);
    assert(b);
    assert(*endp == 0);
    assert (btree_myset_equal_p (tree1, tree2));

    f = m_core_fopen ("a-mbptree.dat", "wt");
    if (!f) abort();
    btree_myset_out_str(f, tree1);
    fclose (f);

    f = m_core_fopen ("a-mbptree.dat", "rt");
    if (!f) abort();
    b = btree_myset_in_str (tree2, f);
    assert (b == true);
    assert (btree_myset_equal_p (tree1, tree2));
    fclose(f);
  }
}

static void
test_multimap(void)
{
  multimap_t b;
  multimap_init(b);
  
  for (int size = 20; size < 1000; size += 10) {
    for(int i = 0; i < size; i++) {
      multimap_set_at(b, i/4, i);
    }
    assert(multimap_size(b) == (unsigned) size);

    for (int k = 0 ; k < size/4; k++) {
      int j = (4*k+3);
      multimap_it_t it;
      for(multimap_it_from(it, b, k);
          multimap_it_while_p(it, k);
          multimap_next(it)) {
        const multimap_itref_t *ref = multimap_cref(it);
        assert(*ref->key_ptr == k);
        assert(*ref->value_ptr == j);
        j--;
      }
      assert(j == 4*(k-1)+3);
    }
    
    for(int i = 0; i < size; i++) {
      bool r = multimap_erase(b, i/4);
      assert (r == true);
    }
    assert(multimap_size(b) == 0);
  }
  
  multimap_clear(b);
}

static void
test_multiset(void)
{
  multiset_t b;
  multiset_init(b);
  
  for (int size = 20; size < 1000; size += 10) {
    for(int i = 0; i < size; i++) {
      multiset_push(b, i/4);
    }
    assert(multiset_size(b) == (unsigned) size);

    for (int k = 0 ; k < size/4; k++) {
      int j = 0;
      multiset_it_t it;
      for(multiset_it_from(it, b, k);
          multiset_it_while_p(it, k);
          multiset_next(it)) {
        const int *ref = multiset_cref(it);
        assert(*ref == k);
        j++;
      }
      assert(j == 4);
    }
    
    for(int i = 0; i < size; i++) {
      bool r = multiset_erase(b, i/4);
      assert (r == true);
    }
    assert(multiset_size(b) == 0);
  }
  
  multiset_clear(b);
}

static void test_double(void)
{
  M_LET( (tree, (1.0, 2.0), (3.0, 4.0)), MapDouble) {
    double *r = MapDouble_get(tree, 1.0);
    assert( r != NULL);
    assert( *r == 2.0);
    r = MapDouble_get(tree, 3.0);
    assert( r != NULL);
    assert( *r == 4.0);
    r = MapDouble_get(tree, 4.0);
    assert( r == NULL);
  }

  M_LET( (tree, 1.0, 3.0), SetDouble) {
    double *r = SetDouble_get(tree, 1.0);
    assert( r != NULL);
    assert( *r == 1.0);
    r = SetDouble_get(tree, 3.0);
    assert( r != NULL);
    assert( *r == 3.0);
    r = SetDouble_get(tree, 4.0);
    assert( r == NULL);
  }

  M_LET( (tree, (1.0, 2.0), (3.0, 4.0)), MultiMapDouble) {
    double *r = MultiMapDouble_get(tree, 1.0);
    assert( r != NULL);
    assert( *r == 2.0);
    r = MultiMapDouble_get(tree, 3.0);
    assert( r != NULL);
    assert( *r == 4.0);
    r = MultiMapDouble_get(tree, 4.0);
    assert( r == NULL);
  }

  M_LET( (tree, 1.0, 3.0), MultiSetDouble) {
    double *r = MultiSetDouble_get(tree, 1.0);
    assert( r != NULL);
    assert( *r == 1.0);
    r = MultiSetDouble_get(tree, 3.0);
    assert( r != NULL);
    assert( *r == 3.0);
    r = MultiSetDouble_get(tree, 4.0);
    assert( r == NULL);
  }
}

int main(void)
{
  test1();
  test2();
  test3();
  test3_string();
  test4();
  test5();
  test_io();
  test_io_set();
  test_multimap();
  test_multiset();
  test_double();
  exit(0);
}
