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
#include <stdint.h>
#include <stdlib.h>

#define NDEBUG
#include "m-dict.h"

static inline bool oor_equal_p(int64_t k, unsigned char n)
{
  return k == (int)-n-1;
}
static inline void oor_set(int64_t *k, unsigned char n)
{
  *k = (int)-n-1;
}

DICT_OA_DEF2(dict_int64, int64_t, M_OPEXTEND(M_DEFAULT_OPLIST, OOR_EQUAL(oor_equal_p), OOR_SET(oor_set M_IPTR)), int64_t, M_DEFAULT_OPLIST)

static int64_t get_rand(void)
{
  int64_t n;
  do {
    n = (rand() * RAND_MAX + rand()) * RAND_MAX + rand();
  } while (n == -1 || n == -2);
  return n;
}

// Let's push value into the dict until the system abandons!
static void find_max(void)
{
  dict_int64_t dict;
  dict_int64_init(dict);
  size_t n = 0;
  while (true) {
    dict_int64_set_at (dict, get_rand(), get_rand());
    n++;
    if ((n & ((1<<20)-1)) == 0) {
      printf ("n=%zu\n", n);
    }
  }
  dict_int64_clear(dict);
}

int main()
{
  find_max();
}
