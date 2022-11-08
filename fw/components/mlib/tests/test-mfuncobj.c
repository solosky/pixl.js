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
#include "m-funcobj.h"
#include "coverage.h"

START_COVERAGE

/* Define one interface with some params
   and 2 instances with some data and no data*/
FUNC_OBJ_ITF_DEF(interface1, int, int, const int *)

FUNC_OBJ_INS_DEF(instance11,
                 interface1,
                 (a, b), {
                   assert(string_equal_str_p(self->c, "INIT"));
                   string_set_str(self->c, "CALLED");
                   return self->sort_field*(a - *b);
                 },
                 (sort_field, int),
                 (c, string_t, STRING_OPLIST) )

FUNC_OBJ_INS_DEF(instance12,
                 interface1,
                 (a, b), { 
                   return a  - *b;
                 })

/* Define one interface with no param
   and 2 instances with some data and no data*/
FUNC_OBJ_ITF_DEF(interface2, int)

FUNC_OBJ_INS_DEF(instance21,
                 interface2,
                 (), { 
                   return 1;
                 })

FUNC_OBJ_INS_DEF(instance22,
                 interface2,
                 (), {
                   self->a ++;
                   return self->a;
                 }, (a, int) )

END_COVERAGE

#define M_OPL_instance22_t() FUNC_OBJ_INS_OPLIST(instance22, M_BASIC_OPLIST)

FUNC_OBJ_ITF_DEF_AS(Interface, Interface, double, double)

FUNC_OBJ_INS_DEF_AS(Instance, Instance,
                    Interface,
                    (a), {
                      return a * self->a + self->b;
                    },
                    (a, double),
                    (b, double) )
#define M_OPL_Instance() FUNC_OBJ_INS_OPLIST(Instance, M_BASIC_OPLIST, M_BASIC_OPLIST)

static void test_instance11(void)
{
  instance11_t cmp;
  int n = 2;
  instance11_init_with(cmp, -1, STRING_CTE("INIT") );
  int x = interface1_call(instance11_as_interface(cmp), 10, &n);
  assert(x == -8);
  assert(string_equal_str_p(cmp->c, "CALLED"));
  instance11_clear(cmp);

  instance11_init_with(cmp, 1, STRING_CTE("INIT") );
  x = interface1_call(instance11_as_interface(cmp), 100, &n);
  assert(x == 98);
  assert(string_equal_str_p(cmp->c, "CALLED"));
  instance11_clear(cmp);
}

static void test_instance12(void)
{
  instance12_t cmp;
  int n = 0;
  instance12_init(cmp);
  int x = interface1_call(instance12_as_interface(cmp), 10, &n);
  assert(x == 10);
  instance12_clear(cmp);
}

static void test_instance21(void)
{
  instance21_t cmp;
  instance21_init(cmp);
  int x = interface2_call(instance21_as_interface(cmp));
  assert(x == 1);
  instance21_clear(cmp);
}

static void test_instance22(void)
{
  M_LET( (cmp, 1), instance22_t) {
    int x = interface2_call(instance22_as_interface(cmp));
    assert(x == 2);
    x = interface2_call(instance22_as_interface(cmp));
    assert(x == 3);
    x = interface2_call(instance22_as_interface(cmp));
    assert(x == 4);
  }
}

static void test_double(void)
{
  M_LET( (func, 2.0, 3.0), Instance) {
    double d = Interface_call(Instance_as_interface(func), 4.0);
    assert (d == 2.0 * 4.0 + 3.0);
  }
}

int main(void)
{
  test_instance11();
  test_instance12();
  test_instance21();
  test_instance22();
  test_double();
  return 0;
}
