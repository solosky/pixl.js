/*
 * Copyright (c) 2021-2022-2022, Patrick Pelissier
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
#include <queue>
#include "m-prioqueue.h"

// Invert the order of priority for the integers to match the C++ one
#define CMP_REVERT(a,b)      ((a) > (b) ? -1 : (a) < (b))
PRIOQUEUE_DEF(prio_int, int, M_OPEXTEND(M_BASIC_OPLIST, CMP(CMP_REVERT) ))

// Transform an integer 'i' into a M*LIB base item 'a'
#define TYPE_SET_INT(a, i) (a) = (i)

// Container OPLIST (disable iterators & reset)
#define CONT_OPL                                                        \
  M_OPEXTEND( PRIOQUEUE_OPLIST(prio_int, M_OPL_int()),                  \
              RESET(0),                                                 \
              IT_TYPE(0),                                               \
              IT_FIRST(0),                                              \
              IT_END(0),                                                \
              IT_SET(0),                                                \
              IT_END_P(0),                                              \
              IT_EQUAL_P(0),                                            \
              IT_LAST_P(0),                                             \
              IT_NEXT(0),                                               \
              IT_CREF(0) )

// C++ Base class of the item in the container
#define BASE_CLASS int

// Transform an integer 'i' into a C++ base item 'a'
#define CLASS_SET_INT(a, i) (a) = (i)

// C++ Container class
#define CONT_CLASS std::priority_queue<int>

// Compare the M*LIB container a to the C++ container b
#define CMP_CONT(a, b) cmp_cont(a, b)

// Compare the M*LIB base object to the C++ base object
#define CMP_BASE(a, b) assert( (a) == (b) )

void cmp_cont(const prio_int_t a, const std::priority_queue<int> &b)
{
  std::priority_queue<int> cb = b;
  prio_int_t ca;
  prio_int_init_set(ca, a);
  //printf("ca="); prio_int_out_str(stdout, ca); printf("\n");
  while (!cb.empty()) {
    int n;
    prio_int_pop(&n, ca);
    //printf("N=%d TOP=%d\n", n, cb.top());
    assert(n == cb.top());
    cb.pop();
  }
  assert(prio_int_empty_p(ca));
  prio_int_clear(ca);
}

// There is no reset, so the prioqueue keeps increasing ==> Limit the number of operation.
#define DEFAULT_NUMBER 1500

#define push_back push
#define pop_back  pop
#define back      top
#include "check-generic.hpp"
