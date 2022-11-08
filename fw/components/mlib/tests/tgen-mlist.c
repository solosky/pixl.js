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

#include "m-list.h"
#include "m-i-list.h"

LIST_DEF(list_dbl, double)
LIST_DUAL_PUSH_DEF(list_dbl2, double)

void f_push_back(list_dbl_t list);
double f_iterator(list_dbl_t list);

void f_push_back(list_dbl_t list)
{
  list_dbl_push_back(list, 2.0);
}

double f_iterator(list_dbl_t list)
{
  list_dbl_it_t it;
  double s = 0.0;
  for(list_dbl_it(it, list); !list_dbl_end_p(it); list_dbl_next(it))
    s += *list_dbl_cref(it);
  return s;
}

void f2_push_back(list_dbl2_t list);
double f2_iterator(list_dbl2_t list);

void f2_push_back(list_dbl2_t list)
{
  list_dbl2_push_back(list, 2.0);
}

double f2_iterator(list_dbl2_t list)
{
  list_dbl2_it_t it;
  double s = 0.0;
  for(list_dbl2_it(it, list); !list_dbl2_end_p(it); list_dbl2_next(it))
    s += *list_dbl2_cref(it);
  return s;
}


struct itest {
  int n;
  ILIST_INTERFACE (ilist_list, struct itest);
};

ILIST_DEF(ilist_list, struct itest)

extern void f_init(ilist_list_t l);
void f_init(ilist_list_t l)
{
  ilist_list_init(l);
}

