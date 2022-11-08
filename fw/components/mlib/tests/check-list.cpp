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
#include <forward_list>
#include "m-list.h"

LIST_DEF(list_int, int)

// Transform an integer 'i' into a M*LIB base item 'a'
#define TYPE_SET_INT(a, i) (a) = (i)

// Container OPLIST
#define CONT_OPL   LIST_OPLIST(list_int, M_OPL_int())

// C++ Base class of the item in the container
#define BASE_CLASS int

// Transform an integer 'i' into a C++ base item 'a'
#define CLASS_SET_INT(a, i) (a) = (i)

// C++ Container class
#define CONT_CLASS std::forward_list<int>

// Compare the M*LIB container a to the C++ container b
#define CMP_CONT(a, b) cmp_cont(a, b)

// Compare the M*LIB base object to the C++ base object
#define CMP_BASE(a, b) assert( (a) == (b) )

void cmp_cont(list_int_t a, std::forward_list<int> b)
{
  list_int_it_t ita;
  list_int_it(ita, a);
  auto          itb = b.begin();
  while (itb != b.end()) {
    assert(!list_int_end_p(ita));
    const int *b0 = list_int_cref(ita);
    int &b1 = *itb;
    CMP_BASE(*b0, b1);
    itb++;
    list_int_next(ita);
  }
  assert(list_int_end_p(ita));
}

#define HAVE_INSERT_AFTER
#define HAVE_ERASE_AFTER
#define DEFAULT_NUMBER 100000

// C++ forward_list pushs in front instead of in back
#define push_back push_front
#define pop_back  pop_front
#define back      front
#include "check-generic.hpp"
