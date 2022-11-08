/*
 * Copyright (c) 2021-2022, Patrick Pelissier
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
#include <deque>
#include "m-deque.h"

DEQUE_DEF(deque_float, float)

// Transform an integer 'i' into a M*LIB base item 'a'
#define TYPE_SET_INT(a, i) (a) = (i)

// Container OPLIST
#define CONT_OPL   DEQUE_OPLIST(deque_float, M_OPL_float())

// C++ Base class of the item in the container
#define BASE_CLASS float

// Transform an integer 'i' into a C++ base item 'a'
#define CLASS_SET_INT(a, i) (a) = (i)

// C++ Container class
#define CONT_CLASS std::deque<float>

// Compare the M*LIB container a to the C++ container b
#define CMP_CONT(a, b) cmp_cont(a, b)

// Compare the M*LIB base object to the C++ base object
#define CMP_BASE(a, b) assert( (a) == (b) )

void cmp_cont(deque_float_t a, std::deque<float> b)
{
  deque_float_it_t ita;
  deque_float_it(ita, a);
  auto          itb = b.begin();
  while (itb != b.end()) {
    assert(!deque_float_end_p(ita));
    const float *b0 = deque_float_cref(ita);
    float &b1 = *itb;
    CMP_BASE(*b0, b1);
    itb++;
    deque_float_next(ita);
  }
  assert(deque_float_end_p(ita));
}

#define DEFAULT_NUMBER 1000000
#define HAVE_PUSH_FRONT
#define HAVE_POP_FRONT
#include "check-generic.hpp"
