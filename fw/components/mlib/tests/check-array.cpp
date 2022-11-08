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
#include <vector>
#include "m-array.h"

ARRAY_DEF(array_int, int)

// Transform an integer 'i' into a M*LIB base item 'a'
#define TYPE_SET_INT(a, i) (a) = (i)

// Container OPLIST
#define CONT_OPL   ARRAY_OPLIST(array_int, M_OPL_int())

// C++ Base class of the item in the container
#define BASE_CLASS int

// Transform an integer 'i' into a C++ base item 'a'
#define CLASS_SET_INT(a, i) (a) = (i)

// C++ Container class
#define CONT_CLASS std::vector<int>

// Compare the M*LIB container a to the C++ container b
#define CMP_CONT(a, b) cmp_cont(a, b)

// Compare the M*LIB base object to the C++ base object
#define CMP_BASE(a, b) assert( (a) == (b) )

void cmp_cont(array_int_t a, std::vector<int> b)
{
  size_t s = array_int_size(a);
  assert (s == b.size());
  for(size_t i = 0; i < s; i++) {
    int *b0 = array_int_get(a, i);
    int &b1 = b.at(i);
    CMP_BASE(*b0, b1);
  }
}

#define DEFAULT_NUMBER 1000000
#define HAVE_RESERVE
#define HAVE_RESIZE
#include "check-generic.hpp"
