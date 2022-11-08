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

#include "m-array.h"

ARRAY_DEF(array_uint, unsigned int)

unsigned int add_two(const array_uint_t tab, size_t i, size_t j);
void size_to_exactly(array_uint_t tab, size_t n);
void push_back (array_uint_t tab, unsigned int n);
void unpop(array_uint_t tab);
void unpop_if(array_uint_t tab, unsigned int x);
void construct(void *p);

unsigned int add_two(const array_uint_t tab, size_t i, size_t j)
{
  return *array_uint_cget(tab, i) + *array_uint_cget(tab, j);
}

void size_to_exactly(array_uint_t tab, size_t n)
{
  array_uint_resize(tab, 0);
  array_uint_resize(tab, n);
}

void push_back (array_uint_t tab, unsigned int n)
{
  array_uint_push_back(tab, n);
}

void unpop(array_uint_t tab)
{
  array_uint_pop_back(NULL, tab);
}

void unpop_if(array_uint_t tab, unsigned int x)
{
  if (x == *array_uint_back(tab))
    array_uint_pop_back(NULL, tab);
}

array_uint_t global;
extern void f(void *p);
void construct(void *p)
{
  array_uint_it_t mark;
  array_uint_it_end(mark, global);
  f(p);
  array_uint_pop_until(global, mark);
}
