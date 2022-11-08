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

#include "m-mempool.h"

#include "coverage.h"
START_COVERAGE
MEMPOOL_DEF(mempool_uint, unsigned int)
END_COVERAGE

MEMPOOL_DEF_AS(MempoolDouble, MempoolDouble, unsigned int)

static void test(void)
{
  mempool_uint_t m;
  mempool_uint_init(m);

  unsigned int *tab[100000];
  for(unsigned int i = 0; i < 100000; i++) {
    tab[i] = mempool_uint_alloc(m);
    *tab[i] = i;
  }
  for(unsigned int i = 0; i < 100000; i++) {
    assert (*tab[i] == i);
  }
  for(unsigned int i = 0; i < 100000; i+=2) {
    mempool_uint_free(m, tab[i]);
    tab[i] =  NULL;
  }
  for(unsigned int i = 1; i < 100000; i+=2) {
    assert (*tab[i] == i);
  }
  for(unsigned int i = 0; i < 100000; i+=2) {
    tab[i] = mempool_uint_alloc(m);
    *tab[i] = i;
  }
  for(unsigned int i = 0; i < 100000; i++) {
    assert (*tab[i] == i);
  }
  for(unsigned int i = 0; i < 100000; i++) {
    mempool_uint_free(m, tab[i]);
    tab[i] =  NULL;
  }
  mempool_uint_clear(m);
}

static void test_double(void)
{
  MempoolDouble m;
  MempoolDouble_init(m);

  unsigned int *tab[100000];
  for(unsigned int i = 0; i < 100000; i++) {
    tab[i] = MempoolDouble_alloc(m);
    *tab[i] = i;
  }
  for(unsigned int i = 0; i < 100000; i++) {
    assert (*tab[i] == i);
  }
  for(unsigned int i = 0; i < 100000; i+=2) {
    MempoolDouble_free(m, tab[i]);
    tab[i] =  NULL;
  }
  for(unsigned int i = 1; i < 100000; i+=2) {
    assert (*tab[i] == i);
  }
  for(unsigned int i = 0; i < 100000; i+=2) {
    tab[i] = MempoolDouble_alloc(m);
    *tab[i] = i;
  }
  for(unsigned int i = 0; i < 100000; i++) {
    assert (*tab[i] == i);
  }
  for(unsigned int i = 0; i < 100000; i++) {
    MempoolDouble_free(m, tab[i]);
    tab[i] =  NULL;
  }
  MempoolDouble_clear(m);
}

int main(void)
{
  test();
  test_double();
  exit(0);
}
