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
#include "m-serial-bin.h"
#include "m-tuple.h"
#include "m-array.h"
#include "m-variant.h"
#include "m-list.h"
#include "m-dict.h"

// Serial bin is not supported for standard types if not C11
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L  

ARRAY_DEF(a2, int)
#define M_OPL_a2_t() ARRAY_OPLIST(a2, M_BASIC_OPLIST)

ARRAY_DEF(l2, int)
#define M_OPL_l2_t() ARRAY_OPLIST(l2, M_BASIC_OPLIST)

DICT_DEF2(d2, string_t, STRING_OPLIST, int, M_BASIC_OPLIST )
#define M_OPL_d2_t() DICT_OPLIST(d2, STRING_OPLIST, M_BASIC_OPLIST)

VARIANT_DEF2(v2,
             (is_int, int),
             (is_bool, bool) )
#define M_OPL_v2_t() VARIANT_OPLIST(v2, M_BASIC_OPLIST, M_BASIC_OPLIST)
            
TUPLE_DEF2(my,
           (vala, int),
           (valb, float),
           (valc, bool),
           (vald, string_t),
           (vale, a2_t),
           (valf, v2_t),
           (valg, l2_t),
           (valh, d2_t)
           )
#define M_OPL_my_t() TUPLE_OPLIST(my, M_BASIC_OPLIST, M_BASIC_OPLIST, M_BASIC_OPLIST, STRING_OPLIST, M_OPL_a2_t(), M_OPL_v2_t(), M_OPL_l2_t(), M_OPL_d2_t() )

TUPLE_DEF2(my2,
           (activated, bool),
           (data, my_t, M_OPL_my_t() ) )

static void test_out_empty(void)
{
  m_serial_return_code_t ret;
  my2_t el1, el2;
  my2_init(el1);
  my2_init(el2);
  
  FILE *f = m_core_fopen ("a-mbin.dat", "wt");
  if (!f) abort();
  M_LET( (serial, f), m_serial_bin_write_t) {
    ret = my2_out_serial(serial, el1);
    assert (ret == M_SERIAL_OK_DONE);
  }
  fclose(f);

  f = m_core_fopen ("a-mbin.dat", "rt");
  if (!f) abort();
  M_LET( (serial, f), m_serial_bin_read_t) {
    ret = my2_in_serial(el2, serial);
    assert (ret == M_SERIAL_OK_DONE);
  }
  fclose(f);
  
  assert (my2_equal_p (el1, el2));

  my2_clear(el1);
  my2_clear(el2);
}
  my2_t el1, el2;

static void test_out_fill(void)
{
  m_serial_read_t  in;
  m_serial_write_t out;
  m_serial_return_code_t ret;
  my2_init(el1);
  my2_init(el2);
  
  FILE *f;

  el2->activated = true;
  el2->data->vala = 145788;
  el2->data->valb = -0.1f;
  el2->data->valc = false;
  string_set_str(el2->data->vald, "This is a string test.");
  a2_push_back(el2->data->vale, 1);
  a2_push_back(el2->data->vale, 4);
  a2_push_back(el2->data->vale, -5);
  a2_push_back(el2->data->vale, 1458);
  v2_set_is_int(el2->data->valf, 12356789);
  l2_push_back(el2->data->valg, 1345);
  l2_push_back(el2->data->valg, 46543);
  l2_push_back(el2->data->valg, -5678);
  d2_set_at(el2->data->valh, STRING_CTE("Paul"), 1);
  d2_set_at(el2->data->valh, STRING_CTE("Smith"), 2);
  
  f = m_core_fopen ("a-mbin.dat", "wt");
  if (!f) abort();
  m_serial_bin_write_init(out, f);
  ret = my2_out_serial(out, el2);
  assert (ret == M_SERIAL_OK_DONE);
  m_serial_bin_write_clear(out);
  fclose(f);

  f = m_core_fopen ("a-mbin.dat", "rt");
  if (!f) abort();
  m_serial_bin_read_init(in, f);
  ret = my2_in_serial(el1, in);
  assert (ret == M_SERIAL_OK_DONE);
  m_serial_bin_read_clear(in);
  fclose(f);
  
  assert (my2_equal_p (el1, el2));

  my2_clear(el1);
  my2_clear(el2);
}

int main(void)
{
  test_out_empty();
  test_out_fill();
  exit(0);    
}

#else
int main(void)
{
  exit(0);    
}
#endif
