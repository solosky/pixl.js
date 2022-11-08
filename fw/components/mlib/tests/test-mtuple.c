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
#include "test-obj.h"
#include "m-string.h"

#include "m-tuple.h"

#include "coverage.h"
START_COVERAGE
TUPLE_DEF2(pair,
           (key, string_t, (INIT(string_init), INIT_SET(string_init_set), SET(string_set), CLEAR(string_clear))),
           (value, testobj_t, TESTOBJ_OPLIST) )
END_COVERAGE

// Minimum definition
TUPLE_DEF2(pair_min,
           (key1, string_t, (INIT_SET(string_init_set), SET(string_set), CLEAR(string_clear))),
           (key2, string_t, (INIT_SET(string_init_set), SET(string_set), CLEAR(string_clear))))

// No cmp defined
TUPLE_DEF2(triple,
           (key, string_t, (INIT(string_init), INIT_SET(string_init_set), SET(string_set), CLEAR(string_clear), CMP(string_cmp))),
           (value, testobj_t, TESTOBJ_OPLIST ),
           (data, const char *, ()))

// cmp shall be defined
TUPLE_DEF2(pair3,
           (key, string_t, (INIT(string_init), INIT_SET(string_init_set), SET(string_set), CLEAR(string_clear), CMP(string_cmp), HASH(string_hash), EQUAL(string_equal_p))),
           (value, testobj_t, M_OPEXTEND( TESTOBJ_OPLIST, CMP(testobj_cmp)) ) )

TUPLE_DEF2(pair_str,
           (vala, string_t, STRING_OPLIST),
           (valb, string_t, STRING_OPLIST))
#define M_OPL_pair_str_t() TUPLE_OPLIST(pair_str, STRING_OPLIST, STRING_OPLIST)

TUPLE_DEF2(single_str, (vala, string_t, STRING_OPLIST))

TUPLE_DEF2(pair2_str,
           (str, string_t),
           (value, unsigned long))

TUPLE_DEF2(int_float,
           (vala, int),
           (valb, float))

// Recursive tuple definition.
TUPLE_DEF2(rtuple,
           (ituple, pair_str_t),
           (name, string_t))
#define M_OPL_rtuple_t()     TUPLE_OPLIST(rtuple, M_OPL_pair_str_t(), STRING_OPLIST)

TUPLE_DEF2(rtuple2, (name, string_t), (value, int))
#define M_OPL_rtuple2_t() TUPLE_OPLIST(rtuple2, STRING_OPLIST, M_BASIC_OPLIST)

/* Chain of tuple on top of tuple to check if it works */
TUPLE_DEF2(rtuple3, (name, string_t), (ituple2, rtuple2_t), (ituple, rtuple_t) )
#define M_OPL_rtuple3_t() TUPLE_OPLIST(rtuple3, STRING_OPLIST, M_OPL_rtuple2_t(), M_OPL_rtuple_t())

/* Chain of tuple on top of tuple to check if it works */
TUPLE_DEF2(rtuple4, (value, rtuple3_t) )
#define M_OPL_rtuple4_t() TUPLE_OPLIST(rtuple4, M_OPL_rtuple4_t() )

TUPLE_DEF2_AS(vector, Vector, (x, double), (y, double), (z, double) )
#define M_OPL_Vector() TUPLE_OPLIST(vector, M_BASIC_OPLIST )

TUPLE_DEF2(rtuple5, (x, int, M_OPL_int()), (priority, int, M_OPEXTEND( M_OPL_int(), CMP(0), HASH(0), EQUAL(0) ) ) )

static void check_swap(void)
{
  pair_str_t p1, p2;
  pair_str_init (p1);
  pair_str_init (p2);

  string_set_str(p1->vala, "Hello");
  string_set_str(p1->valb, "World");
  pair_str_swap(p1, p2);
  assert(string_equal_str_p (p1->vala, ""));
  assert(string_equal_str_p (p1->valb, ""));
  assert(string_equal_str_p (p2->vala, "Hello"));
  assert(string_equal_str_p (p2->valb, "World"));

  pair_str_clear(p1);
  pair_str_clear(p2);
}

static void check_reset(void)
{
  single_str_t p1;
  single_str_init (p1);

  string_set_str(p1->vala, "Hello");
  single_str_reset(p1);
  assert(string_equal_str_p (p1->vala, ""));

  single_str_clear(p1);

  M_LET(r, rtuple_t) {
    string_set_str(r->name, "Hello");
    rtuple_reset(r);
    assert(string_equal_str_p (r->name, ""));    
  }
  
  M_LET( (r, ("Hello"), 4), TUPLE_OPLIST(pair2_str, STRING_OPLIST, M_BASIC_OPLIST) )  {
    assert(string_equal_str_p (r->str, "Hello"));
    assert(r->value == 4);
    pair2_str_reset(r);
    assert(string_equal_str_p (r->str, ""));
  }

  M_LET( (s, ("Goldy") ), STRING_OPLIST) 
  M_LET( (r, s, 46), TUPLE_OPLIST(pair2_str, STRING_OPLIST, M_BASIC_OPLIST) )
  M_LET( (q, r), TUPLE_OPLIST(pair2_str, STRING_OPLIST, M_BASIC_OPLIST) ) {
    assert(string_equal_str_p (q->str, "Goldy"));
    assert(q->value == 46);
    pair2_str_reset(q);
    assert(string_equal_str_p (q->str, ""));
  }

  // Wrong oplist usage: fall back to _init_emplace.
  M_LET( (s, ("Goldy McDuck") ), STRING_OPLIST) 
  M_LET((r, s, 460), TUPLE_OPLIST(pair2_str, STRING_OPLIST) )  {
    assert(string_equal_str_p (r->str, "Goldy McDuck"));
    assert(r->value == 460);
    pair2_str_reset(r);
    assert(string_equal_str_p (r->str, ""));
  }

  // Two level of parenthesis needed: 
  //  one to perform an emplace with INIT_WITH for the tuple
  //  another level to emplace the string_t
  M_LET( (r, (("Two emplace"))), TUPLE_OPLIST(single_str, STRING_OPLIST) ) {
    assert(string_equal_str_p (r->vala, "Two emplace"));
    single_str_reset(r);
    assert(string_equal_str_p (r->vala, ""));
  }

  M_LET( (s, ("Goldy McDuck") ), STRING_OPLIST) 
  M_LET( (r, (s)), TUPLE_OPLIST(single_str, STRING_OPLIST) ) {
    assert(string_equal_str_p (r->vala, "Goldy McDuck"));
    single_str_reset(r);
    assert(string_equal_str_p (r->vala, ""));
  }

}

static void check_io(void)
{
  M_LET(s, STRING_OPLIST)
    M_LET(pair, TUPLE_OPLIST(pair_str, STRING_OPLIST, STRING_OPLIST))
    M_LET(pair2, TUPLE_OPLIST(pair_str, STRING_OPLIST, STRING_OPLIST)) {
    string_set_str(pair->vala, "Hello");
    string_set_str(pair->valb, "World");
    pair_str_get_str(s, pair, false);
    assert(string_cmp_str(s, "(\"Hello\",\"World\")") == 0);
    const char *end;
    bool b = pair_str_parse_str(pair2, string_get_cstr(s), &end);
    assert (b);
    assert (pair_str_equal_p(pair, pair2) == true);
    assert(*end == 0);

    FILE *f = m_core_fopen("a-mtuple.dat", "wt");
    if (!f) abort();
    pair_str_out_str(f, pair);
    fclose(f);
    
    f = m_core_fopen("a-mtuple.dat", "rt");
    if (!f) abort();
    b = pair_str_in_str(pair2, f);
    assert (b);
    fclose(f);
    assert (pair_str_equal_p(pair, pair2) == true);
  }
}

static void test1(void)
{
  pair_t p1, p2;

  pair_init (p1);
  string_set_str(p1->key, "HELLO");
  testobj_set_ui(p1->value, 1742);
  pair_init_set (p2, p1);
  assert(testobj_cmp_ui(p2->value, 1742) == 0);
  assert(string_equal_str_p(p2->key, "HELLO"));
  string_set_str(p2->key, "HELLO WORLD");
  testobj_set_ui(p2->value, 174217);
  pair_set(p1, p1);
  pair_set(p1, p2);
  assert(testobj_cmp_ui(p1->value, 174217) == 0);
  assert(string_equal_str_p(p1->key, "HELLO WORLD"));
  pair_clear(p1);
  pair_clear(p2);

  string_t s;
  testobj_t z;
  pair3_t p3, p4;
  string_init_set_str(s, "HELLO");
  testobj_init_set_ui (z, 1442);
  pair3_init_emplace(p3, s, z);
  pair3_init_emplace(p4, s, z);
  int i = pair3_cmp (p3, p4);
  assert (i == 0);
  pair3_emplace(p4, s, z);
  i = pair3_cmp (p3, p4);
  assert (i == 0);
  testobj_set_ui (z, 1443);
  pair3_set_value(p4, z);
  i = pair3_cmp (p3, p4);
  assert (i < 0);
  testobj_set_ui (z, 1442);
  string_set_str(s, "HELLN");
  pair3_set_key(p4, s);
  i = pair3_cmp (p3, p4);
  assert (i > 0);

  // p3 = HELLO, 1442
  // p4 = HELLN, 1443
  i = pair3_cmp_order(p3, p3, TUPLE_ORDER(pair3, ASC(value), DSC(key)));
  assert (i == 0);
  i = pair3_cmp_order(p3, p4, TUPLE_ORDER(pair3, ASC(key), ASC(value)));
  assert (i > 0);
  i = pair3_cmp_order(p3, p4, TUPLE_ORDER(pair3, ASC(value), ASC(key)));
  assert (i < 0);
  i = pair3_cmp_order(p3, p4, TUPLE_ORDER(pair3, DSC(key), ASC(value)));
  assert (i < 0);
  i = pair3_cmp_order(p3, p4, TUPLE_ORDER(pair3, DSC(value), ASC(key)));
  assert (i > 0);
  
  pair3_clear(p3);
  pair3_clear(p4);
  string_clear(s);
  testobj_clear(z);
}

static void check_chaining_tuple(void)
{
  rtuple4_t r;
  // rtuple4_init is only defined if ALL oplists export the INIT method 
  rtuple4_init(r);
  rtuple4_clear(r);

  rtuple5_t t0, t1;
  rtuple5_init_emplace(t0, 1, 0);
  rtuple5_init_emplace(t1, 1, 1);
  assert(rtuple5_equal_p(t0, t1));
  assert(rtuple5_cmp(t0, t1) == 0);
  assert(rtuple5_hash(t0) == rtuple5_hash(t1));
  assert(t0->priority != t1->priority);
  rtuple5_clear(t0);
  rtuple5_clear(t1);
}

static void test_double(void)
{
  M_LET( (v, 1.0, 2.0, 3.0), Vector ) {
    assert (*vector_cget_at_x(v) == 1.0 );
    assert (*vector_cget_at_y(v) == 2.0 );
    assert (*vector_cget_at_z(v) == 3.0 );
  }
}

int main(void)
{
  test1();
  check_io();
  check_swap();
  check_reset();
  check_chaining_tuple();
  test_double();
  exit(0);
}
