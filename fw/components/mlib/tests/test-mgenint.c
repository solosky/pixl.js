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
#include "m-mutex.h"
#include "m-atomic.h"

#include "m-genint.h"

#define MAX_N 256

static void test(size_t n)
{
  m_genint_t s;
  size_t i;
  
  m_genint_init(s, (unsigned int) n);
  for(int k = 0; k < 3; k++) {
    for(size_t j = 0; j< n ; j++) {
      i = m_genint_pop(s);
      assert (i == j);
    }
    i = m_genint_pop(s);
    assert (i == M_GENINT_ERROR);
    assert (m_genint_pop(s) == M_GENINT_ERROR);
    
    for(size_t j = 0; j< n ; j++) {
      m_genint_push(s, (unsigned int) ((j + (size_t) k) % n ));
    }
  }
  
  m_genint_clear(s);
}

/*******************************************************/

m_genint_t global;
atomic_bool tab_g[MAX_N];

static void conso(void *p)
{
  size_t n = *(size_t*)p;
  for(int i = 0; i < 100000; i++) {
    unsigned int tab[4];
    for(int j= 0; j < 4; j++) {
      tab[j] = m_genint_pop(global);
      if (tab[j] != M_GENINT_ERROR) {
        assert (tab[j] < n);
        assert (atomic_load(&tab_g[tab[j]]) == false);
        atomic_store(&tab_g[tab[j]], true);
      }
    }
    for(int j = 0; j < 4; j++) {
      if (tab[j] != M_GENINT_ERROR) {
        assert (tab[j] < n);
        assert (atomic_load(&tab_g[tab[j]]) == true);
        atomic_store(&tab_g[tab[j]], false);
        m_genint_push(global, tab[j]);
      }
    }
  }
}

static void test2(size_t n)
{
  m_thread_t idx[4];

  m_genint_init(global, (unsigned int) n);
  for(int i = 0; i < MAX_N; i++)
    atomic_init(&tab_g[i], false);
  
  for(int i = 0; i < 4; i++) {
    m_thread_create (idx[i], conso, (void*)&n);
  }
  for(int i = 0; i < 4;i++) {
    m_thread_join(idx[i]);
  }

  m_genint_clear(global);
}

// In this test, the dimension of the global and the number of threads
// match: we cannot have a failure.
static void conso2(void *p)
{
  size_t n = *(size_t*)p;
  for(int i = 0; i < 1000000; i++) {
    unsigned int j = m_genint_pop(global);
    assert (j != M_GENINT_ERROR);
    assert (j < n);
    m_genint_push(global, j);
  }
}

static void test3(size_t n)
{
  m_thread_t idx[MAX_N];
  assert (n <= MAX_N);
  
  m_genint_init(global, (unsigned int) n);
  for(size_t i = 0; i < n; i++) {
    m_thread_create (idx[i], conso2, (void*)&n);
  }
  for(size_t i = 0; i < n;i++) {
    m_thread_join(idx[i]);
  }
  m_genint_clear(global);
}

int main(void)
{
  for(size_t n = 1; n < MAX_N; n++) {
    test(n);
  }
  for(size_t n = 1; n < MAX_N; n+=17) {
    test2(n);
  }
  for(size_t n = 2; n < 6; n++) {
    test3(n);
  }
  exit(0);
}
