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
#include "m-array.h"
#include "m-string.h"
#include "coverage.h"

START_COVERAGE
ARRAY_DEF(array_uint, unsigned int)
ARRAY_DEF(array_mpz, testobj_t, TESTOBJ_OPLIST)
END_COVERAGE
#define ARRAY_UINT_OPLIST ARRAY_OPLIST(array_uint)

// Array with the minimum number of methods.
ARRAY_DEF(array_min_z, testobj_t, (INIT_SET(testobj_init_set), SET(testobj_set), CLEAR(testobj_clear)))
ARRAY_DEF(array_min2_z, testobj_t, (INIT(testobj_init), CLEAR(testobj_clear)))
ARRAY_DEF(array_min3_z, testobj_t, (CLEAR(testobj_clear)))

ARRAY_DEF(array_ulong, uint64_t)
ARRAY_DEF(array_string, string_t)
ARRAY_DEF_AS(array_double, ArrayDouble, ArrayDoubleIt, double)
#define M_OPL_ArrayDouble() ARRAY_OPLIST(array_double, M_BASIC_OPLIST)

ArrayDouble g_array = ARRAY_INIT_VALUE();

static void test_uint(void)
{
  array_uint_t v;
  array_uint_init(v);

  assert (array_uint_empty_p(v) == true);

  array_uint_push_back(v, 2);
  assert (array_uint_empty_p(v) == false);
  unsigned int x;
  array_uint_pop_back(&x, v);
  assert( x == 2);

  for(unsigned int i = 0; i < 100; i++)
    array_uint_push_back(v, i);

  unsigned int s = 0;
  for(size_t i = 0; i < array_uint_size(v) ; i++)
    s += *array_uint_cget(v, i);
  assert( s == 100 * 99 /2);

  s = 0;
  for M_EACH(item, v, ARRAY_UINT_OPLIST) {
    s += *item;
  }
  assert( s == 100 * 99 /2);

  array_uint_pop_at(&s, v, 49);
  assert (s == 49);
  array_uint_push_at(v, 49, 49);
  
  array_uint_pop_at (NULL, v, 0);
  assert(array_uint_size(v) == 99);
  s= 0;
  for(size_t i = 0; i < array_uint_size(v) ; i++)
    s += *array_uint_cget(v, i);
  assert( s == 100 * 99 /2);

  unsigned int *p = array_uint_get(v, 1);
  *p += 999;
  s= 0;
  for(size_t i = 0; i < array_uint_size(v) ; i++)
    s += *array_uint_cget(v, i);
  assert( s == 100 * 99 /2 + 999);

  array_uint_swap_at(v, 2, 3);
  assert(*array_uint_cget(v, 2) == 4);
  assert(*array_uint_cget(v, 3) == 3);

  array_uint_reserve(v, 0);
  assert(array_uint_capacity(v) == array_uint_size(v));

  const unsigned int *src = array_uint_cget(v, 4);
  assert (*src == 5);

  array_uint_remove_v(v, 1, array_uint_size(v) -1);
  assert(array_uint_size(v) == 2);
  assert(*array_uint_cget(v, 0) == 1);
  assert(*array_uint_cget(v, 1) == 99);

  for(unsigned int i = 3; i < 100; i++)
    array_uint_push_back(v, i);

  array_uint_resize(v, 10);
  assert(array_uint_size(v) == 10);
  assert(*array_uint_cget(v, 9) == 10);
  array_uint_resize(v, 1000);
  assert(array_uint_size(v) == 1000);
  assert(*array_uint_cget(v, 999) == 0);

  array_uint_t v2, v3;
  array_uint_init_set(v2,v);
  array_uint_init(v3);
  array_uint_push_back(v3, 459320);
  array_uint_set(v3, v);
  assert (array_uint_equal_p(v3, v));
  array_uint_push_back(v3, 459320);  
  array_uint_set(v3, v);
  assert (array_uint_equal_p(v3, v));
  array_uint_clear(v);
  array_uint_clear(v3);
  assert(array_uint_size(v2) == 1000);
  assert(*array_uint_cget(v2, 999) == 0);
  array_uint_push_at (v2, 0, 1742);
  assert (array_uint_size(v2) == 1001);
  assert(*array_uint_cget(v2, 0) == 1742);
  assert(*array_uint_cget(v2, 1000) == 0);
  
  array_uint_insert_v (v2, 500, 4);
  assert (array_uint_size(v2) == 1005);
  assert(*array_uint_cget(v2, 0) == 1742);
  assert(*array_uint_cget(v2, 500) == 0);
  assert(array_uint_size(v2) == 1005);
  assert(*array_uint_cget(v2, 1004) == 0);
  array_uint_set_at(v2, 1004, 465487);
  array_uint_insert_v (v2, 500, 0);
  assert(array_uint_size(v2) == 1005);
  assert(*array_uint_cget(v2, 1004) == 465487);
  array_uint_insert_v (v2, 1004, 10000);
  assert(array_uint_size(v2) == 11005);
  assert(*array_uint_cget(v2, 11004) == 465487);
  array_uint_resize(v2, 1005);
  
  array_uint_init(v);
  array_uint_swap(v,v2);
  assert(array_uint_size(v) == 1005);
  assert(array_uint_size(v2) == 0);

  array_uint_reserve(v, 100);
  assert(array_uint_size(v) == 1005);
  assert(array_uint_capacity(v) == 1005);
  array_uint_reserve(v, 10000);
  assert(array_uint_size(v) == 1005);
  assert(array_uint_capacity(v) == 10000);

  array_uint_set (v, v);
  array_uint_set (v2, v);
  assert(array_uint_size(v2) == 1005);

  array_uint_reset(v);
  assert (array_uint_capacity(v) > 0);
  array_uint_reserve(v, 0);
  assert (array_uint_capacity(v) == 0);

  array_uint_reset(v2);
  array_uint_splice(v, v2);
  assert (array_uint_size(v) == 0);
  assert (array_uint_size(v2) == 0);

  for(unsigned i = 0; i < 100; i++) {
    array_uint_push_back(v, i);
    array_uint_push_back(v2, 100+i);
  }
  array_uint_splice(v, v2);
  assert (array_uint_size(v) == 200);
  assert (array_uint_size(v2) == 0);
  for(unsigned i = 0; i < 200; i++) {
    assert (i == *array_uint_get(v, i));
  }
  
  x = 1478963;
  s = *array_uint_back(v);
  array_uint_push_move(v, &x);
  assert (*array_uint_back(v) == 1478963);
  x = 0;
  array_uint_pop_move(&x, v);
  assert (x == 1478963);
  assert (s == *array_uint_back(v));

  s = (unsigned int) array_uint_size (v);
  bool b = array_uint_erase (v, 12459);
  assert (b == false);
  assert (s == array_uint_size (v));
  b = array_uint_erase (v, 0);
  assert (b == true);
  assert (s == array_uint_size (v)+1);
  
  array_uint_clear(v);
  array_uint_clear(v2);
}

static void test_mpz(void)
{
  array_mpz_t array1, array2;
  testobj_t z;
  
  array_mpz_init (array1);
  array_mpz_init (array2);
  testobj_init (z);

  // Test empty
  FILE *f = m_core_fopen ("a-marray.dat", "wt");
  if (!f) abort();
  array_mpz_out_str(f, array1);
  fclose (f);

  f = m_core_fopen ("a-marray.dat", "rt");
  if (!f) abort();
  bool b = array_mpz_in_str (array2, f);
  assert (b == true);
  assert (array_mpz_equal_p (array1, array2));
  fclose(f);
  
  // Test non empty
  for(int n = 0; n < 1000; n++) {
    testobj_set_ui (z, (unsigned int) n);
    array_mpz_push_back (array1, z);
  }
  
  f = m_core_fopen ("a-marray.dat", "wt");
  if (!f) abort();
  array_mpz_out_str(f, array1);
  fclose (f);

  f = m_core_fopen ("a-marray.dat", "rt");
  if (!f) abort();
  b = array_mpz_in_str (array2, f);
  assert (b == true);
  assert (array_mpz_equal_p (array1, array2));
  fclose(f);

  array_mpz_reset(array1);

  M_LET(str, STRING_OPLIST) {
    array_mpz_get_str(str, array1, false);
    assert (string_cmp_str (str, "[]") == 0);
    const char *sp;
    b = array_mpz_parse_str(array2, string_get_cstr(str), &sp);
    assert(b);
    assert(*sp == 0);
    assert(array_mpz_equal_p(array1, array2));

    testobj_set_ui (z, 17);
    array_mpz_push_back (array1, z);
    array_mpz_get_str(str, array1, false);
    assert (string_cmp_str (str, "[17]") == 0);
    b = array_mpz_parse_str(array2, string_get_cstr(str), &sp);
    assert(b);
    assert(*sp == 0);
    assert(array_mpz_equal_p(array1, array2));

    testobj_set_ui (z, 42);
    array_mpz_push_back (array1, z);
    array_mpz_get_str(str, array1, true);
    assert (string_cmp_str (str, "[17][17,42]") == 0);
    b = array_mpz_parse_str(array2, string_get_cstr(str), &sp);
    assert(b);
    assert(strcmp(sp, "[17,42]") == 0);
    assert(!array_mpz_equal_p(array1, array2));
    b = array_mpz_parse_str(array2, sp, &sp);
    assert(b);
    assert(strcmp(sp, "") == 0);
    assert(array_mpz_equal_p(array1, array2));
    // NOTE: Not the same order than list...
    }

  array_mpz_clear(array2);

  array_mpz_reset(array1);
  /* Test of the emplace functions.
   * 3 functions have been generated for testobj:
   * one from unsigned int (suffix _ui)
   * one from C string (suffix _str)
   * one from testobj_t itself (no suffix)
   */
  array_mpz_emplace_back_ui(array1, 42);
  assert(testobj_cmp_ui(*array_mpz_back(array1), 42) == 0);
  array_mpz_emplace_back_ui(array1, 426);
  assert(testobj_cmp_ui(*array_mpz_back(array1), 426) == 0);
  array_mpz_emplace_back_str(array1, "458");
  assert(testobj_cmp_ui(*array_mpz_back(array1), 458) == 0);
  testobj_set_ui(z, 789);
  array_mpz_emplace_back(array1, z);
  assert(testobj_cmp_ui(*array_mpz_back(array1), 789) == 0);

  testobj_clear (z);
  array_mpz_clear(array1);
}

static void test_str(void)
{
  array_string_t a;
  array_string_init(a);

  // Test of emplace with const char * ==> string_t
  array_string_emplace_back(a, "Hello");
  assert(string_equal_str_p(*array_string_back(a), "Hello"));

  array_string_emplace_back(a, "world");
  assert(string_equal_str_p(*array_string_back(a), "world"));

  array_string_emplace_back(a, "How");
  assert(string_equal_str_p(*array_string_back(a), "How"));

  array_string_emplace_back(a, "are");
  assert(string_equal_str_p(*array_string_back(a), "are"));

  array_string_emplace_back(a, "you");
  assert(string_equal_str_p(*array_string_back(a), "you"));

  // Test stable sort with string_t
  array_string_special_stable_sort(a);

  // Test INIT_WITH of array of string_t wihtin M_LET 
  M_LET( (b, ("Hello"), ("How"), ("are"), ("world"), ("you")), ARRAY_OPLIST(array_string, STRING_OPLIST)){
    assert( array_string_equal_p(a, b));
  }

  array_string_clear(a);
}

static void test_d(void)
{
  array_uint_t a1, a2;
  
  array_uint_init(a1);
  array_uint_init_move(a2, a1);
  assert (array_uint_empty_p (a2));
  array_uint_init(a1);
  array_uint_move(a1, a2);
  assert (array_uint_empty_p (a1));
  for(int i = 0; i < 10; i++)
    array_uint_push_back (a1, (unsigned int) i);
  array_uint_set_at (a1, 0, 17);
  assert (*array_uint_get (a1, 0) == 17);
  assert (*array_uint_back(a1) == 9);
  unsigned int *p = array_uint_push_new(a1);
  assert (*p == 0);
  *p = 10;
  assert (*array_uint_back(a1) == 10);
  for(int i = 0; i < 10; i++)
    array_uint_push_at (a1, 9, (unsigned int) i);
  assert (*array_uint_back(a1) == 10);
  for(int i = 9; i < 19; i++)
    assert (*array_uint_get (a1, (size_t) i) == 18U -(unsigned int) i);

  *array_uint_safe_get(a1, 100) = 100;
  assert (*array_uint_back(a1) == 100);
  assert (array_uint_size(a1) == 101);
  array_uint_pop_back (NULL, a1);
  assert (*array_uint_back(a1) == 0);
  assert (array_uint_size(a1) == 100);

  array_uint_insert_v (a1, 10, 200);
  assert (array_uint_size(a1) == 300);
  for(int i = 10; i < 210; i++) {
    assert (*array_uint_get (a1, (size_t)i) == 0);
  }
  array_uint_it_t it;
  array_uint_it(it, a1);
  array_uint_remove (a1, it);
  assert (array_uint_size(a1) == 299);

  array_uint_it_last (it, a1);
  assert (array_uint_last_p(it));
  array_uint_it_end (it, a1);
  assert (array_uint_end_p(it));
  array_uint_previous (it);
  assert (array_uint_last_p(it));
  array_uint_it_t it2;
  array_uint_it_set (it2, it);
  assert (array_uint_it_equal_p(it2, it));
  assert (*array_uint_ref(it2) == 0);
  array_uint_insert(a1, it2, 17);
  assert (array_uint_size(a1) == 300);
  assert (*array_uint_ref(it2) == 17);

  array_uint_it_end(it2, a1);
  array_uint_insert(a1, it2, 17789);
  assert (array_uint_size(a1) == 301);
  assert (*array_uint_ref(it2) == 17789);
  array_uint_it(it, a1);
  assert (array_uint_it_equal_p (it, it2));
  
  array_uint_init_set (a2, a1);
  assert (array_uint_equal_p (a2, a1));

  size_t h = array_uint_hash(a2);
  assert (h != 0);

  array_uint_reset(a1);
  array_uint_it(it, a1);
  assert (array_uint_end_p (it));
  assert (array_uint_last_p (it));
  
  array_uint_clear(a1);
  array_uint_clear(a2);
}

static void test_double(void)
{
  M_LET( (tab, 0.0, 1.0, 2.0, 3.0), ArrayDouble) {
    double ref = 0.0;
    for M_EACH(i, tab, ArrayDouble) {
      assert (*i == ref);
      ref += 1.0;
    }
    // Test of buffer overflow (with address sanitizer)
    // _init_set provides an allocated buffer of the exact needed size 
    ArrayDouble tab1;
    array_double_init_set(tab1, tab);
    array_double_pop_at(NULL, tab1, 0);
    array_double_clear(tab1);
    array_double_init_set(tab1, tab);
    array_double_insert_v(tab1, 0, 2);
    array_double_clear(tab1);
    array_double_init_set(tab1, tab);
    array_double_remove_v(tab1, 0, 1);
    array_double_clear(tab1);
    array_double_init_set(tab1, tab);
    array_double_push_at(tab1, 0, 2.);
    array_double_clear(tab1);
  }

  assert( array_double_empty_p(g_array));
  array_double_push_back(g_array, 34.);
  assert( array_double_size(g_array) == 1);
  array_double_clear(g_array);
}

// Test support of M*LIB for C++ class
#if defined(__cplusplus)

/* For this, we need new placement support */
#include <new>

/* Some Simple Oplist for a C++ class (using new placement feature)
   Some features may not be always available (like comparison or I/O).
   It isn't exported, since there is no meaning to support for M*LIB
   C++ class officialy (use the STL instead!)
*/
#define M_CLASS_CPP_INIT(opl,x)        (new (&(x)) (M_GET_NAME opl))
#define M_CLASS_CPP_INIT_SET(opl,x, y) (new (&(x)) M_GET_NAME opl(y))
#define M_CLASS_CPP_SET(opl, x, y)     ((x) = (y))
#define M_CLASS_CPP_CLEAR(opl, x)      ((&(x))->~M_GET_NAME opl())
#define M_CLASS_CPP_CLEAR2(x)          ((&(x))->~opl())
#define M_CLASS_CPP_INIT_MOVE(opl,x,y) (new (&(x)) M_GET_NAME opl(std::move(y)), ((&(y))->~M_GET_NAME opl()))
#define M_CLASS_CPP_CMP(opl, x, y)     ((x) < (y) ? -1 : (y) < (x) ? 1 : 0)
#define M_CLASS_CPP_HASH(opl, x)       (std::hash(x))
#define M_CLASS_OPLIST(_class)                  \
  (NAME(_class),                                \
   TYPE(_class),                                \
   INIT(API_1(M_CLASS_CPP_INIT)),               \
   INIT_SET(API_1(M_CLASS_CPP_INIT_SET)),       \
   SET(API_1(M_CLASS_CPP_SET)),                 \
   CLEAR(API_1(M_CLASS_CPP_CLEAR)),             \
   INIT_MOVE(API_1(M_CLASS_CPP_INIT_MOVE)) )

// Define a basic C++ class
class Foo {
  int *x;
public:
  Foo() { x = new (int); };
  Foo(int n) { x = new (int) ; *x = n; }
  Foo(const Foo &y) {x = new (int) ; *x = *y.x;};
  ~Foo() { delete x; }
  void operator=(const Foo &y) { *x = *y.x; };
  void operator=(int n) { *x = n; }
  bool operator<(const Foo &y) const { return *x < *y.x; }
  bool operator==(int n) const { return *x == n; }
};

// Disable the warning due to using memcpy/memmove with this C++ class
#if defined(__GNUC__) && __GNUC__ >= 8
  _Pragma("GCC diagnostic push")
  _Pragma("GCC diagnostic ignored \"-Wclass-memaccess\"")
#endif

// Define a M*LIB array of such class
ARRAY_DEF_AS(array_foo, array_foo_t, array_foo_it_t, Foo, M_CLASS_OPLIST(Foo))
#define M_OPL_array_foo_t() ARRAY_OPLIST(array_foo, M_CLASS_OPLIST(Foo))

#if defined(__GNUC__) && __GNUC__ >= 8
  _Pragma("GCC diagnostic pop")
#endif

static void test_cplusplus(void)
{
  int n = 100;
  M_LET(a, array_foo_t) {
    Foo b;
    for(int i = 0; i < n; i++) {
      b = i;
      array_foo_push_back(a, b);
    }
    array_foo_it_t it;
    int j = 0;
    for(array_foo_it(it, a); !array_foo_end_p(it); array_foo_next(it)) {
      assert( *array_foo_cref(it) == j);
      j++;
    }
    assert(j == n);
  }
}
#else
static void test_cplusplus(void)
{
  // Nothing to do
}
#endif

int main(void)
{
  test_uint();
  test_mpz();
  test_d();
  test_str();
  test_double();
  test_cplusplus();
  exit(0);
}
