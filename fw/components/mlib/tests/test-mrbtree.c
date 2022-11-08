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
#include "m-rbtree.h"

static bool uint_in_str(unsigned int *u, FILE *f)
{
  int n = m_core_fscanf(f, "%u", u);
  return n == 1;
}

static bool uint_parse_str(unsigned int *u, const char str[], const char **endptr)
{
  char *end;
  *u = (unsigned int) strtol(str, &end, 0);
  if (endptr) { *endptr = (const char*) end; }
  return (uintptr_t) end != (uintptr_t) str;
}

static void uint_out_str(FILE *f, unsigned int u)
{
  fprintf(f, "%u", u);
}

static void uint_get_str(string_t str, unsigned int u, bool append)
{
  (append ? string_cat_printf : string_printf) (str, "%u", u);
}

#include "coverage.h"
START_COVERAGE
RBTREE_DEF(rbtree_uint, unsigned int, M_OPEXTEND(M_BASIC_OPLIST, IN_STR(uint_in_str M_IPTR), OUT_STR(uint_out_str), GET_STR(uint_get_str), PARSE_STR(uint_parse_str M_IPTR)) )
END_COVERAGE

RBTREE_DEF(rbtree_float, float)
RBTREE_DEF(rbtree_mpz, testobj_t, TESTOBJ_CMP_OPLIST)

#define UINT_OPLIST RBTREE_OPLIST(rbtree_uint)
#define FLOAT_OP RBTREE_OPLIST(rbtree_float)

RBTREE_DEF_AS(TreeDouble, TreeDouble, TreeDoubleIt, double, M_BASIC_OPLIST)
#define M_OPL_TreeDouble() RBTREE_OPLIST(TreeDouble, M_BASIC_OPLIST)

static void test_uint(void)
{
  rbtree_uint_t tree, tree2, tree3;
  rbtree_uint_init(tree);
  assert (rbtree_uint_empty_p(tree));
  rbtree_uint_init_set(tree2, tree);
  assert (rbtree_uint_empty_p(tree2));

  int k = 0;
  assert(rbtree_uint_size(tree) == 0);
  for M_EACH(item, tree, UINT_OPLIST) {
      assert(item != NULL);
      assert (false);
      k ++;
    }
  assert(k == 0);

  rbtree_uint_push(tree, 10);

  k = 0;
  assert(rbtree_uint_size(tree) == 1);
  for M_EACH(item, tree, UINT_OPLIST) {
      assert (*item == 10);
      k ++;
    }
  assert(k == 1);
  
  rbtree_uint_push(tree, 5);

  k = 0;
  assert(rbtree_uint_size(tree) == 2);
  const unsigned int tab2[] = {5, 10};
  for M_EACH(item, tree, UINT_OPLIST) {
      assert (*item == tab2[k] );
      k ++;
    }
  assert(k == 2);

  rbtree_uint_push(tree, 17);

  k = 0;
  assert(rbtree_uint_size(tree) == 3);
  const unsigned int tab3[] = {5, 10, 17};
  for M_EACH(item, tree, UINT_OPLIST) {
      assert (*item == tab3[k] );
      k ++;
    }
  assert(k == 3);

  rbtree_uint_push(tree, 3);

  k = 0;
  assert(rbtree_uint_size(tree) == 4);
  const unsigned int tab4[] = {3, 5, 10, 17};
  for M_EACH(item, tree, UINT_OPLIST) {
      assert (*item == tab4[k] );
      k ++;
    }
  assert(k == 4);

  rbtree_uint_push(tree, 11);

  assert(rbtree_uint_size(tree) == 5);
  assert(rbtree_uint_empty_p(tree) == false);
  const unsigned int tab5[] = {3, 5, 10, 11, 17};
  k = 0;
  for M_EACH(item, tree, UINT_OPLIST) {
      assert (tab5[k] == *item);
      k ++;
    }
  assert (k == 5);

  rbtree_uint_it_t it, it2;
  rbtree_uint_it(it2, tree);
  for (rbtree_uint_it_last(it, tree) ; !rbtree_uint_end_p(it) ; rbtree_uint_previous(it)) {
    k--;
    rbtree_uint_it_set(it2, it);
    assert(rbtree_uint_it_equal_p(it2, it));
    assert (tab5[k] == *rbtree_uint_cref(it2) );
  }
  rbtree_uint_it_end(it, tree);
  assert (rbtree_uint_end_p(it));
  assert(!rbtree_uint_it_equal_p(it2, it));
  rbtree_uint_it_end(it2, tree);
  assert(rbtree_uint_it_equal_p(it2, it));
  assert( k == 0);
  assert (rbtree_uint_hash(tree) != 0);

  rbtree_uint_init(tree3);
  rbtree_uint_push(tree3, 5);
  assert(rbtree_uint_size(tree3) == 1);
  assert(*rbtree_uint_cmin(tree3) == 5);
  assert(*rbtree_uint_cmax(tree3) == 5);
  rbtree_uint_move(tree3, tree);
  rbtree_uint_clear(tree3);

  rbtree_uint_init(tree3);
  rbtree_uint_push(tree3, 5);
  rbtree_uint_push(tree3, 3);
  rbtree_uint_push(tree3, 7);
  rbtree_uint_push(tree3, 2);
  assert(rbtree_uint_size(tree3) == 4);
  rbtree_uint_it(it, tree3);
  rbtree_uint_remove(tree3, it);
  assert(rbtree_uint_size(tree3) == 3);
  assert(*rbtree_uint_cref(it) == 3);
  rbtree_uint_remove(tree3, it);
  assert(rbtree_uint_size(tree3) == 2);
  assert(*rbtree_uint_cref(it) == 5);
  rbtree_uint_remove(tree3, it);
  assert(rbtree_uint_size(tree3) == 1);
  assert(*rbtree_uint_cref(it) == 7);
  rbtree_uint_remove(tree3, it);
  assert(rbtree_uint_size(tree3) == 0);

  rbtree_uint_push(tree3, 1);
  rbtree_uint_push(tree3, 0);
  rbtree_uint_push(tree3, 5);
  rbtree_uint_push(tree3, 3);
  rbtree_uint_push(tree3, 7);
  rbtree_uint_push(tree3, 2);
  rbtree_uint_push(tree3, 8);
  rbtree_uint_push(tree3, 9);
  rbtree_uint_push(tree3, 10);
  rbtree_uint_it(it, tree3);
  assert(*rbtree_uint_cref(it) == 0);
  rbtree_uint_next(it);
  assert(*rbtree_uint_cref(it) == 1);
  rbtree_uint_next(it);
  assert(*rbtree_uint_cref(it) == 2);
  rbtree_uint_next(it);
  assert(*rbtree_uint_cref(it) == 3);
  rbtree_uint_previous(it);
  assert(*rbtree_uint_cref(it) == 2);
  rbtree_uint_next(it);
  assert(*rbtree_uint_cref(it) == 3);
  rbtree_uint_remove(tree3, it);
  assert(*rbtree_uint_cref(it) == 5);
  rbtree_uint_previous(it);
  assert(*rbtree_uint_cref(it) == 2);
  rbtree_uint_remove(tree3, it);
  assert(*rbtree_uint_cref(it) == 5);
  rbtree_uint_remove(tree3, it);
  assert(*rbtree_uint_cref(it) == 7);
  rbtree_uint_next(it);
  rbtree_uint_remove(tree3, it);
  assert(*rbtree_uint_cref(it) == 9);
  
  rbtree_uint_clear(tree3);
  
  rbtree_uint_init_move(tree, tree2);
  rbtree_uint_init_move(tree2, tree);
  assert (rbtree_uint_empty_p(tree2));
  
  const unsigned int max = 1001;
  for(unsigned int num = 1; num < max; num++) {
    rbtree_uint_init(tree);
    for(unsigned int i = 0; i < num; i++)
      rbtree_uint_push(tree, i);
    assert(rbtree_uint_size(tree) == num);
    assert(rbtree_uint_empty_p(tree) == false);
    if (num > 10) {
      rbtree_uint_push(tree, 10);
      assert(rbtree_uint_size(tree) == num);
      assert(rbtree_uint_empty_p(tree) == false);
    }
    unsigned int count = 0;
    for M_EACH(item, tree, UINT_OPLIST) {
        assert (count == *item);
        count ++;
      }
    assert (*rbtree_uint_min(tree) == 0);
    assert (*rbtree_uint_max(tree) == num-1);
    assert (*rbtree_uint_cmin(tree) == 0);
    assert (*rbtree_uint_cmax(tree) == num-1);
    assert (count == num);
    rbtree_uint_set(tree2, tree);
    unsigned int *ptr = rbtree_uint_get(tree2, num/2);
    assert (ptr != NULL);
    assert (*ptr == num/2);
    const unsigned int *cptr = rbtree_uint_cget(tree2, num/2);
    assert (cptr != NULL);
    assert (*cptr == num/2);
    rbtree_uint_clear(tree);

    bool b = rbtree_uint_pop_at(NULL, tree2, num+1);
    assert (b == false);
    for(unsigned int i = 0; i < num; i+=3) {
      b = rbtree_uint_pop_at(NULL, tree2, i);
      assert(b);
      ptr = rbtree_uint_get(tree2, i);
      assert (ptr == NULL);
    }
    for(unsigned int i = 1; i < num; i+=3) {
      b = rbtree_uint_pop_at(NULL, tree2, i);
      assert(b);
      ptr = rbtree_uint_get(tree2, i);
      assert (ptr == NULL);
    }    
  }

  rbtree_uint_init (tree);
  assert(rbtree_uint_empty_p(tree));
  rbtree_uint_reset(tree2);
  rbtree_uint_push(tree2, 17421);
  assert(rbtree_uint_size(tree2) == 1);
  rbtree_uint_swap(tree, tree2);
  assert(rbtree_uint_empty_p(tree2));
  assert(rbtree_uint_size(tree) == 1);
  unsigned int *ptr = rbtree_uint_get(tree, 17421);
  assert(ptr != NULL && *ptr == 17421);
  
  rbtree_uint_clear(tree);
  rbtree_uint_clear(tree2);
}

static void test_uint_permut(void)
{
  const unsigned int max = 1001;
  unsigned int tab[1001];
  for(unsigned int num = 1; num < max; num++) {
    rbtree_uint_t tree;
    rbtree_uint_init(tree);
    for(unsigned int i = 0; i < num; i++)
      tab[i] = i;
    for(unsigned int i = 0; i < 2 * num; i++) {
      unsigned int j = (unsigned int) rand() % num;
      unsigned int k = (unsigned int) rand() % num;
      M_SWAP(unsigned int, tab[j], tab[k]);
    }
    for(unsigned int i = 0; i < num; i++)
      rbtree_uint_push(tree, tab[i]);
    unsigned int k = 0;
    for M_EACH(item, tree, UINT_OPLIST) {
      assert (k == *item);
      k++;
    }
    rbtree_uint_clear(tree);
  }
}


static void test_float(void)
{
  M_LET(tree, FLOAT_OP) {
    for(float f = -17.42f; f < 17.42f; f+=0.01f)
      rbtree_float_push (tree, f);
    bool p = false;
    float g = 0.0;
    for M_EACH(item, tree, FLOAT_OP) {
        if (p) {
          assert (g < *item);
        }
        g = *item;
        p = true;
      }
  }
}

static void test_io(void)
{
  M_LET(str, STRING_OPLIST)
  M_LET(tree1, tree2, UINT_OPLIST) {
    // Empty one
    FILE *f = m_core_fopen ("a-mrbtree.dat", "wt");
    if (!f) abort();
    rbtree_uint_out_str(f, tree1);
    fclose (f);

    f = m_core_fopen ("a-mrbtree.dat", "rt");
    if (!f) abort();
    bool b = rbtree_uint_in_str (tree2, f);
    assert (b == true);
    assert (rbtree_uint_equal_p (tree1, tree2));
    fclose(f);

    rbtree_uint_get_str(str, tree1, false);
    assert(string_equal_str_p(str, "[]"));
    const char *sp;
    b = rbtree_uint_parse_str(tree2, string_get_cstr(str), &sp);
    assert(b);
    assert(*sp == 0);
    assert(rbtree_uint_equal_p(tree1, tree2));

    // Fill in data
    for(unsigned int i = 0 ; i < 10; i++)
      rbtree_uint_push(tree1, i);

    f = m_core_fopen ("a-mrbtree.dat", "wt");
    if (!f) abort();
    rbtree_uint_out_str(f, tree1);
    fclose (f);

    f = m_core_fopen ("a-mrbtree.dat", "rt");
    if (!f) abort();
    b = rbtree_uint_in_str (tree2, f);
    assert (b == true);
    assert (rbtree_uint_equal_p (tree1, tree2));
    fclose(f);

    rbtree_uint_get_str(str, tree1, false);
    assert(string_equal_str_p(str, "[0,1,2,3,4,5,6,7,8,9]"));
    b = rbtree_uint_parse_str(tree2, string_get_cstr(str), &sp);
    assert(b);
    assert(*sp == 0);
    assert(rbtree_uint_equal_p(tree1, tree2));
  }  
}

static void test_double(void)
{
  M_LET( (tree, 0.0, 1.0, 2.0), TreeDouble) {
    double ref = 0.0;
    for M_EACH(i, tree, TreeDouble) {
      assert (*i == ref);
      ref += 1.0;
    }
  }
}

static void test_from(void)
{
  rbtree_uint_t    tree;
  rbtree_uint_it_t it;
  
  rbtree_uint_init(tree);

  // No element.
  rbtree_uint_it_from(it, tree, 3);
  assert(rbtree_uint_end_p(it));
  assert(rbtree_uint_it_until_p(it, 3));
  assert(!rbtree_uint_it_while_p(it, 3));

  // One element
  rbtree_uint_push(tree, 3);
  rbtree_uint_it_from(it, tree, 3);
  assert(!rbtree_uint_end_p(it));
  assert(rbtree_uint_it_until_p(it, 3));
  assert(rbtree_uint_it_while_p(it, 3));
  assert(!rbtree_uint_it_until_p(it, 4));
  assert(rbtree_uint_it_while_p(it, 4));
  assert(rbtree_uint_it_until_p(it, 2));
  assert(!rbtree_uint_it_while_p(it, 2));

  rbtree_uint_it_from(it, tree, 2);
  assert(!rbtree_uint_end_p(it));
  assert(rbtree_uint_it_until_p(it, 3));
  assert(rbtree_uint_it_while_p(it, 3));
  assert(!rbtree_uint_it_until_p(it, 4));
  assert(rbtree_uint_it_while_p(it, 4));
  assert(rbtree_uint_it_until_p(it, 2));
  assert(!rbtree_uint_it_while_p(it, 2));

  rbtree_uint_it_from(it, tree, 4);
  assert(rbtree_uint_end_p(it));
  assert(rbtree_uint_it_until_p(it, 3));
  assert(!rbtree_uint_it_while_p(it, 3));

  // More elements
  rbtree_uint_push(tree, 10);
  rbtree_uint_push(tree, 5);
  rbtree_uint_push(tree, 1);
  rbtree_uint_push(tree, 7);

  int k = 0;
  const unsigned int tab0[] = { 3, 5, 7};
  for( rbtree_uint_it_from(it, tree, 3); !rbtree_uint_it_until_p(it, 8); rbtree_uint_next(it)) {
    assert (k < 3);
    assert(tab0[k] == *rbtree_uint_cref(it));
    k++;
  }
  assert(k == 3);
  k = 0;
  for( rbtree_uint_it_from(it, tree, 3); !rbtree_uint_it_until_p(it, 7); rbtree_uint_next(it)) {
    assert (k < 3);
    assert(tab0[k] == *rbtree_uint_cref(it));
    k++;
  }
  assert(k == 2);

  k = 0;
  for( rbtree_uint_it_from(it, tree, 3); rbtree_uint_it_while_p(it, 8); rbtree_uint_next(it)) {
    assert (k < 3);
    assert(tab0[k] == *rbtree_uint_cref(it));
    k++;
  }
  assert(k == 3);

  k = 0;
  for( rbtree_uint_it_from(it, tree, 3); rbtree_uint_it_while_p(it, 7); rbtree_uint_next(it)) {
    assert (k < 3);
    assert(tab0[k] == *rbtree_uint_cref(it));
    k++;
  }
  assert(k == 3);

  k = 0;
  for( rbtree_uint_it_from(it, tree, 3); rbtree_uint_it_while_p(it, 6); rbtree_uint_next(it)) {
    assert (k < 3);
    assert(tab0[k] == *rbtree_uint_cref(it));
    k++;
  }
  assert(k == 2);

  k = 0;
  const unsigned int tab1[] = { 1, 3, 5};
  for( rbtree_uint_it_from(it, tree, 0); rbtree_uint_it_while_p(it, 6); rbtree_uint_next(it)) {
    assert (k < 3);
    assert(tab1[k] == *rbtree_uint_cref(it));
    k++;
  }
  assert(k == 3);

  k = 0;
  const unsigned int tab2[] = { 5, 7};
  for( rbtree_uint_it_from(it, tree, 4); rbtree_uint_it_while_p(it, 7); rbtree_uint_next(it)) {
    assert (k < 2);
    assert(tab2[k] == *rbtree_uint_cref(it));
    k++;
  }
  assert(k == 2);

  // From higher than all elements in the tree ==> no iterator.
  k = 0;
  for( rbtree_uint_it_from(it, tree, 11); rbtree_uint_it_while_p(it, 17); rbtree_uint_next(it)) {
    assert (k < 2);
    k++;
  }
  assert(k == 0);

  rbtree_uint_clear(tree);
}

static void test_z(void)
{
  rbtree_mpz_t v;
  testobj_t    z;

  testobj_init(z);
  rbtree_mpz_init(v);

  testobj_set_ui(z, 67);
  rbtree_mpz_push(v, z);
  assert(testobj_cmp_ui(*rbtree_mpz_cmax(v), 67) == 0);
  assert(testobj_cmp_ui(*rbtree_mpz_cmin(v), 67) == 0);

  rbtree_mpz_emplace_ui(v, 42);
  assert(testobj_cmp_ui(*rbtree_mpz_cmax(v), 67) == 0);
  assert(testobj_cmp_ui(*rbtree_mpz_cmin(v), 42) == 0);

  rbtree_mpz_emplace_str(v, "422");
  assert(testobj_cmp_ui(*rbtree_mpz_cmax(v), 422) == 0);
  assert(testobj_cmp_ui(*rbtree_mpz_cmin(v), 42) == 0);

  testobj_clear(z);
  rbtree_mpz_clear(v);
}

int main(void)
{
  test_uint();
  test_uint_permut();
  test_float();
  test_io();
  test_double();
  test_from();
  test_z();
  exit(0);
}
