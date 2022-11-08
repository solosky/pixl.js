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
#include <stdint.h>
#include "m-dict.h"
#include "m-string.h"

static inline bool my_oor_equal_p(uint64_t k, unsigned char n)
{
  return k == ~(uint64_t) n;
}
static inline void my_oor_set(uint64_t *k, unsigned char n)
{
  *k = ~(uint64_t)n;
}
DICT_OA_DEF2(dict,
             uint64_t, M_OPEXTEND(M_BASIC_OPLIST, OOR_EQUAL(my_oor_equal_p), OOR_SET(my_oor_set M_IPTR)),
             uint32_t, M_BASIC_OPLIST)

extern void set_at(dict_t d, uint64_t key, uint32_t value);
void set_at(dict_t d, uint64_t key, uint32_t value)
{
  dict_set_at(d, key, value);
}

extern uint32_t get(dict_t d, uint64_t key);
uint32_t get(dict_t d, uint64_t key)
{
  uint32_t *p = dict_get(d, key);
  return p == NULL ? 0: *p;
}

extern void increment(dict_t d, uint64_t key);
void increment(dict_t d, uint64_t key)
{
  uint32_t *p = dict_get(d, key);
  if (p != NULL) {
    (*p) ++;
  } else {
    dict_set_at(d, key, 1);
  }
}

extern void clean(dict_t d);
void clean(dict_t d)
{
  dict_reset(d);
}

extern bool testempty(uint64_t d);
bool testempty(uint64_t d)
{
  if (my_oor_equal_p(d, 0) || my_oor_equal_p(d, 1))
    return true;
  else
    return false;
}

extern bool testempty2(string_t d);
bool testempty2(string_t d)
{
  if (string_oor_equal_p(d, 0) || string_oor_equal_p(d, 1))
    return true;
  else
    return false;
}
