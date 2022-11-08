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
#include <assert.h>
#include "test-obj.h"

#include "m-shared.h"

#include "coverage.h"
START_COVERAGE
SHARED_PTR_DEF(shared_int, int)
SHARED_RESOURCE_DEF(shared_ressource, testobj_t, TESTOBJ_OPLIST)
END_COVERAGE

SHARED_PTR_DEF(shared_mpz, testobj_t, TESTOBJ_OPLIST)
SHARED_PTR_RELAXED_DEF(shared_relaxed_int, int)

SHARED_PTR_DEF_AS(SharedDouble, SharedDouble, double)
SHARED_PTR_RELAXED_DEF_AS(SharedDouble2, SharedDouble2, double, M_BASIC_OPLIST)
SHARED_RESOURCE_DEF_AS(SharedRessource, SharedRessource, SharedRessourceIt, double)

static int f(const shared_int_t p)
{
  return *shared_int_cref(p);
}

static void test1(void)
{
  shared_int_t p1, p2;

  int *p = M_ASSIGN_CAST(int*, malloc(sizeof(int)));
  assert (p != NULL);
  
  shared_int_init2(p1, p);
  assert(!shared_int_NULL_p(p1));
  assert(shared_int_ref(p1) != NULL);

  *shared_int_ref(p1) = 12;

  shared_int_init_set(p2, p1);
  assert(f(p2) == 12);

  shared_int_reset(p1);
  assert(shared_int_NULL_p(p1));
  assert(!shared_int_NULL_p(p2));
  assert(f(p2) == 12);
  
  shared_int_set(p1, p2);

  shared_int_reset(p2);
  assert(!shared_int_NULL_p(p1));  
  assert(shared_int_NULL_p(p2));
  assert(f(p1) == 12);
  shared_int_set(p2, p1);
  assert(!shared_int_NULL_p(p1));  
  assert(!shared_int_NULL_p(p2));
  assert(f(p2) == 12);

  shared_int_reset(p2);
  shared_int_reset(p1);
  
  assert(shared_int_NULL_p(p1));
  assert(shared_int_NULL_p(p2));

  shared_int_clear(p2);
  shared_int_clear(p1);
  
  shared_int_init2(p1, NULL);
  assert(shared_int_NULL_p(p1));
  shared_int_clear(p1);
                  
  shared_int_init(p1);
  assert(shared_int_NULL_p(p1));
  shared_int_clear(p1);

  shared_int_init_new(p1);
  assert(!shared_int_NULL_p(p1));
  *shared_int_ref(p1) = 1;
  shared_int_init_new(p2);
  assert(!shared_int_NULL_p(p2));
  *shared_int_ref(p2) = 2;
  assert(!shared_int_equal_p(p1, p2));
  assert (*shared_int_cref(p1) == 1);
  assert (*shared_int_cref(p2) == 2);
  shared_int_swap(p1, p2);
  assert (*shared_int_cref(p1) == 2);
  assert (*shared_int_cref(p2) == 1);
  shared_int_set(p1, p2);
  assert(shared_int_equal_p(p1, p2));
  assert (*shared_int_cref(p1) == 1);
  assert (*shared_int_cref(p2) == 1);
  shared_int_clear(p1);
  shared_int_init_move(p1, p2);
  assert (*shared_int_cref(p1) == 1);
  shared_int_init_new(p2);
  assert (*shared_int_cref(p2) == 0);
  shared_int_set(p2, p1);
  shared_int_move(p2, p1);
  assert (*shared_int_cref(p2) == 1);
  shared_int_clear(p2);
}

static void test2(void)
{
  shared_int_t p1, p2;

  shared_int_init_new (p1);
  *shared_int_ref(p1) = 1;
  shared_int_init_new (p2);
  *shared_int_ref(p2) = 2;
  shared_int_set(p1, p2);
  assert (*shared_int_ref(p1) == 2);
  assert (*shared_int_ref(p2) == 2);
  shared_int_clear(p2);
  assert (*shared_int_ref(p1) == 2);
  shared_int_clear(p1);
}

#define MAX_RESSOURCE 10

static void test_ressource(int n)
{
  shared_ressource_t ressource;
  shared_ressource_it_t it[MAX_RESSOURCE];
  shared_ressource_it_t copy[MAX_RESSOURCE];
  shared_ressource_it_t onemore;
  
  shared_ressource_init (ressource, (size_t) n);
  for(int i = 0; i < n ; i++) {
    shared_ressource_it (it[i], ressource);
    assert (!shared_ressource_end_p(it[i]));
    testobj_t *z = shared_ressource_ref(it[i]);
    assert (z != NULL);
    testobj_set_ui(*z, (unsigned int) i);
  }
  shared_ressource_it (onemore, ressource);
  assert (shared_ressource_end_p(onemore));
  for(int i = 0; i < n ; i++) {
    shared_ressource_it_set (copy[i], it[i]);
    assert (!shared_ressource_end_p(copy[i]));
    const testobj_t *z = shared_ressource_cref(copy[i]);
    assert (z != NULL);
    assert (testobj_cmp_ui(*z, (unsigned int) i) == 0);
  }
  shared_ressource_it (onemore, ressource);
  assert (shared_ressource_end_p(onemore));
  for(int i = 0; i < n ; i++) {
    shared_ressource_end (it[i], ressource);
  }
  shared_ressource_it (onemore, ressource);
  assert (shared_ressource_end_p(onemore));
  for(int i = n-1; i >= 0 ; i--) {
    const testobj_t *z = shared_ressource_cref(copy[i]);
    assert (z != NULL);
    shared_ressource_end (copy[i], ressource);
    assert (shared_ressource_end_p(copy[i]));
    shared_ressource_it (onemore, ressource);
    assert (!shared_ressource_end_p(onemore));
    shared_ressource_end (onemore, ressource);
    shared_ressource_end (copy[i], ressource);
  }
  
  shared_ressource_clear(ressource);
}

static void test_double(void){
  SharedDouble p;
  SharedDouble_init_new(p);
  assert(*SharedDouble_ref(p) == 0.0);
  *SharedDouble_ref(p) = 5678.0;
  SharedDouble q;
  SharedDouble_init_set(q, p);
  assert(*SharedDouble_ref(q) == 5678.0);
  SharedDouble_clear(p);
  SharedDouble_clear(q);
}

static void test_double2(void){
  SharedDouble2 p;
  SharedDouble2_init_new(p);
  assert(*SharedDouble2_ref(p) == 0.0);
  *SharedDouble2_ref(p) = 5678.0;
  SharedDouble2 q;
  SharedDouble2_init_set(q, p);
  assert(*SharedDouble2_ref(q) == 5678.0);
  SharedDouble2_clear(p);
  SharedDouble2_clear(q);
}

static void test_double_res(void){
  SharedRessource r;
  SharedRessource_init(r, 4);
  SharedRessourceIt i;
  SharedRessource_it(i, r);
  assert(*SharedRessource_ref(i) == 0);
  *SharedRessource_ref(i) = 3456.0;
  SharedRessource_end(i, r);
  SharedRessource_it(i, r);
  assert(*SharedRessource_ref(i) == 3456.0);
  SharedRessource_clear(r);
}

int main(void)
{
  test1();
  test2();
  for(int i = 1; i < MAX_RESSOURCE; i++) {
    test_ressource(i);  
  }
  test_double();
  test_double2();
  test_double_res();
  exit(0);
}
