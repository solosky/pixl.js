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

#include "m-i-shared.h"

#include "m-mutex.h"
#include "coverage.h"

typedef struct ishared_s {
    ISHARED_PTR_INTERFACE(ishared1, struct ishared_s);
    ISHARED_PTR_INTERFACE(ishared2, struct ishared_s);
    int data[100];
} ishared_t[1];

static void ishared_init_data(struct ishared_s *x)
{
    for(int i = 0 ; i < 100; i++)
        x->data[i] = i * i;
}

static void ishared_test(struct ishared_s *x)
{
    for(int i = 0 ; i < 100; i++) {
        assert (x->data[i] == i*i);
    }
}

START_COVERAGE
ISHARED_PTR_DEF(ishared1, struct ishared_s, (INIT(API_2(ishared_init_data))))
ISHARED_PTR_DEF(ishared2, struct ishared_s, (INIT(API_2(ishared_init_data)), NEW(0), DEL(0)) )
END_COVERAGE

/*ISHARED_PTR_DEF_AS(SharedDouble, SharedDouble, double) */

static void test_single(void)
{
    ishared1_t p1, p2;
    p1 = ishared1_init_new();
    ishared_test(p1);
    p2 = ishared1_init_set(p1);
    ishared_test(p2);
    ishared1_clear(p1);
    ishared_test(p2);

    p1 = ishared1_init(NULL);
    ishared1_set(&p1, p2);
    ishared1_reset(&p2);
    assert(p2 == NULL);
    ishared_test(p1);
    ishared1_clear(p1);

    p1 = ishared1_init_set(p2);
    assert(p1 == NULL);
    ishared1_clear(p1);

    ishared1_clear(p2);

    struct ishared_s *x = (struct ishared_s *) malloc(sizeof (struct ishared_s));
    assert(x != NULL);
    ishared_init_data(x);
    p1 = ishared1_init(x);
    ishared_test(p1);
    ishared1_clear(p1);
}

static struct ishared_s g_var = { 
    ISHARED_PTR_STATIC_INIT(ishared1, struct ishared_s),
    ISHARED_PTR_STATIC_INIT(ishared2, struct ishared_s),
    { 0 }
};

static void test_static(void)
{
    ishared2_t p1, p2;

    p1 = ishared2_init_once(NULL);
    assert(p1 == NULL);
    p1 = ishared2_init_once(&g_var);
    ishared_test(p1);
    p2 = ishared2_init_once(&g_var);
    ishared_test(p2);
    ishared2_clear(p1);
    ishared_test(p2);
    ishared2_clear(p2);
}

#define MAX_THREAD 128
#define MAX_VAR 5

#define VAL(x) { ISHARED_PTR_STATIC_INIT(ishared1, struct ishared_s), ISHARED_PTR_STATIC_INIT(ishared2, struct ishared_s), { 0 } }
static struct ishared_s g_tab[MAX_VAR] = { 
    M_MAP_C(VAL, M_SEQ(1, MAX_VAR))
};

static void func(void *arg)
{
    int id = (int)(intptr_t) arg;
    int v  = id % MAX_VAR;
    ishared2_t p = ishared2_init_once(&g_tab[v]);
    for(int i = 0; i < 100; i++) {
        ishared_test(p);
    }
    ishared2_clear(p);
}

static void test_shared(void)
{
    m_thread_t id[MAX_THREAD];

    for(int i = 0; i < MAX_THREAD; i++) {
        m_thread_create(id[i], func, (void*) (intptr_t) i);
    }
    for(int i = 0; i < MAX_THREAD; i++) {
        m_thread_join(id[i]);
    }
}

int main(void)
{
  test_single();
  test_static();
  test_shared();
  exit(0);
}
