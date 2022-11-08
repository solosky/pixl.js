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
#ifndef TESTOBJ_T
#define TESTOBJ_T

#include <stdbool.h>
#include <assert.h>

#include "m-string.h"

M_BEGIN_PROTECTED_CODE

/* This is a trivial encapsulation of an opaque structure.
   Used for test purpose only.*/
typedef struct testobj_s {
  unsigned int n;
  unsigned int a;
  bool         allocated;
  unsigned int *ptr;
} testobj_t[1];

static inline void testobj_init(testobj_t z)
{
  z->n = 1;
  z->a = 1;
  z->ptr = (unsigned int*) calloc(1, sizeof(unsigned int));
  assert(z->ptr != NULL);
  z->allocated = true;
}

static inline void testobj_clear(testobj_t z)
{
  assert (z->allocated);
  assert (z->ptr != NULL);
  free(z->ptr);
  z->ptr = NULL;
  z->allocated = false;
}

static inline void testobj_init_set(testobj_t d, const testobj_t s)
{
  d->n = s->n;
  d->a = s->a;
  d->ptr = (unsigned int *) calloc (s->n, sizeof(unsigned int));
  assert(d->ptr != NULL);
  memcpy(d->ptr, s->ptr, sizeof(unsigned int) * s->n);
  d->allocated = true;
}

static inline void testobj_set(testobj_t d, const testobj_t s)
{
  if (d != s) {
    testobj_clear(d);
    testobj_init_set(d, s);
  }
}

static inline void testobj_set_ui(testobj_t d, unsigned int v)
{
  d->n = 1;
  d->ptr[0] = v;
}

static inline void testobj_set_id(testobj_t d, unsigned int id)
{
  d->a = id;
}

static inline void testobj_init_set_ui(testobj_t d, unsigned int v)
{
  testobj_init(d);
  testobj_set_ui(d, v);
}

static inline void testobj_init_set_str(testobj_t d, const char v[])
{
  testobj_init(d);
  testobj_set_ui(d, (unsigned int) atoi(v) );
}

static inline unsigned int testobj_get_ui(const testobj_t z)
{
  return z->ptr[0];
}

static inline void testobj_add(testobj_t d, const testobj_t a, const testobj_t b)
{
  d->ptr[0] = a->ptr[0] + b->ptr[0];
}

static inline void testobj_out_str(FILE *f, const testobj_t z)
{
  assert(z->n == 1);
  fprintf(f, "%u", z->ptr[0]);
}

static inline bool testobj_in_str(testobj_t z, FILE *f)
{
  z->n = 1;
  return m_core_fscanf(f, "%u", &z->ptr[0]) == 1;
}

static inline bool testobj_parse_str(testobj_t z, const char str[], const char **endptr)
{
  z->n = 1;
  char *end;
  z->ptr[0] = (unsigned int) strtol(str, &end, 0);
  if (endptr) { *endptr = (const char*) end; }
  return (uintptr_t) end != (uintptr_t) str;
}

static inline bool testobj_equal_p(const testobj_t z1, const testobj_t z2)
{
  if (z1->n != z2->n) return false;
  return memcmp(z1->ptr, z2->ptr, z1->n*sizeof(unsigned int)) == 0;
}

static inline int testobj_cmp(const testobj_t z1, const testobj_t z2)
{
  int ret = 0;
  if (z1->n != z2->n) {
    ret = z1-> n < z2->n ? -1 : 1;
  } else {
    for(unsigned int i = 0; i < z1->n; i++) {
      if (z1->ptr[i] != z2->ptr[i]) {
        ret = z1->ptr[i] < z2->ptr[i] ? -1 : 1;
        break;
      }
    }
  }
  return ret;
}

static inline int testobj_cmp_ui(const testobj_t z1, unsigned int z2)
{
  if (z1->n != 1) return z1-> n < 1 ? -1 : 1;
  return z1->ptr[0] < z2 ? -1 : z1->ptr[0] > z2;
}

static inline void testobj_str(string_t str, const testobj_t z, bool append)
{
  if (append) string_cat_printf (str, "%u", z->ptr[0]);
  else        string_printf (str, "%u", z->ptr[0]);
}

// Go through a macro that forward to the right function.
// This enables testing good macro expansion in OPLIST usage
#define testobj_clear_indirect(op, x) testobj_clear(x)

#define TESTOBJ_OPLIST							\
  (INIT(testobj_init),                                                  \
   INIT_SET(testobj_init_set),                                          \
   SET(testobj_set),                                                    \
   CLEAR(API_1(testobj_clear_indirect)),                                \
   TYPE(testobj_t),                                                     \
   OUT_STR(testobj_out_str),                                            \
   IN_STR(testobj_in_str),                                              \
   PARSE_STR(testobj_parse_str),                                        \
   GET_STR(testobj_str),                                                \
   EQUAL(testobj_equal_p),                                              \
   EMPLACE_TYPE( LIST( (_ui, testobj_init_set_ui, unsigned int), (_str, testobj_init_set_str, const char *), ( /*empty*/, testobj_init_set, testobj_t) ) ) \
   )

#define TESTOBJ_CMP_OPLIST						\
  (INIT(testobj_init),                                                  \
   INIT_SET(testobj_init_set),                                          \
   SET(testobj_set),                                                    \
   CLEAR(testobj_clear),                                                \
   TYPE(testobj_t),                                                     \
   OUT_STR(testobj_out_str),                                            \
   IN_STR(testobj_in_str),                                              \
   PARSE_STR(testobj_parse_str),                                        \
   GET_STR(testobj_str),                                                \
   EQUAL(testobj_equal_p),                                              \
   CMP(testobj_cmp),                                                    \
   EMPLACE_TYPE( LIST( (_ui, testobj_init_set_ui, unsigned int), (_str, testobj_init_set_str, const char *), ( /*empty*/, testobj_init_set, testobj_t) ) ) \
   )

M_END_PROTECTED_CODE

#endif
