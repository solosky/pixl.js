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
#include "m-serial-json.h"
#include "m-tuple.h"
#include "m-array.h"
#include "m-variant.h"
#include "m-list.h"
#include "m-dict.h"

// Serial json is not supported for standard types if not C11
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

typedef enum {
  SUCCESS = 0, NULL_PARAM, INVALID_PARAM
} my_return_code_t;
#define M_OPL_my_return_code_t() M_ENUM_OPLIST(my_return_code_t, SUCCESS)

TUPLE_DEF2(my,
           (vala, int),
           (valb, float),
           (valc, bool),
           (vald, string_t),
           (vale, a2_t),
           (valf, v2_t),
           (valg, l2_t),
           (valh, d2_t),
           (vali, my_return_code_t)
           )
#define M_OPL_my_t() TUPLE_OPLIST(my, M_BASIC_OPLIST, M_BASIC_OPLIST, M_BASIC_OPLIST, STRING_OPLIST, M_OPL_a2_t(), M_OPL_v2_t(), M_OPL_l2_t(), M_OPL_d2_t(), M_OPL_my_return_code_t() )

TUPLE_DEF2(my2,
           (activated, bool),
           (data, my_t, M_OPL_my_t() ) )

static void test_out_empty(void)
{
  m_serial_read_t  in;
  m_serial_write_t out;
  m_serial_return_code_t ret;
  my2_t el1, el2;
  my2_init(el1);
  my2_init(el2);
  
  FILE *f = m_core_fopen ("a-mjson.dat", "wt");
  if (!f) abort();
  m_serial_json_write_init(out, f);
  ret = my2_out_serial(out, el1);
  assert (ret == M_SERIAL_OK_DONE);
  m_serial_json_write_clear(out);
  fclose(f);

  f = m_core_fopen ("a-mjson.dat", "rt");
  if (!f) abort();
  static const char expected[] = "{ \"activated\":false,\"data\":{ \"vala\":0,\"valb\":0.000000,\"valc\":false,\"vald\":\"\",\"vale\":[],\"valf\":{},\"valg\":[],\"valh\":{},\"vali\":0}";
  char get[sizeof expected];
  char *unused = fgets (get, sizeof expected , f);
  assert (unused != NULL && strcmp(get, expected) == 0);
  fclose(f);
  
  f = m_core_fopen ("a-mjson.dat", "rt");
  if (!f) abort();
  m_serial_json_read_init(in, f);
  ret = my2_in_serial(el2, in);
  assert (ret == M_SERIAL_OK_DONE);
  m_serial_json_read_clear(in);
  fclose(f);
  
  assert (my2_equal_p (el1, el2));

  my2_clear(el1);
  my2_clear(el2);
}

static void test_out_fill(void)
{
  m_serial_read_t  in;
  m_serial_write_t out;
  m_serial_return_code_t ret;
  my2_t el1, el2;
  my2_init(el1);
  my2_init(el2);
  
  FILE *f = m_core_fopen("a-mjson.dat", "wt");
  if (!f) abort();
  fprintf(f,
          "{\n"
          " \"activated\":false,\n"
          "\"data\":   {\n"
          "       \"valb\":  -2.300000 , \n"
          "\"vale\": [1,2,3],\n"
          "\"valg\": [1,2,3,4,5,6],\n"
          "\"valh\": { \"jane\": 3, \"steeve\": -4 },\n"
          "\"valf\": { \"is_bool\": true },\n"
          "              \"vala\":1742,\n"
          " \"vald\": \"This is a test\",\n"
          "\"vali\": 3,"
          "    \"valc\": true   } }\n");
  fclose(f);

  f = m_core_fopen ("a-mjson.dat", "rt");
  if (!f) abort();
  m_serial_json_read_init(in, f);
  ret = my2_in_serial(el2, in);
  assert (ret == M_SERIAL_OK_DONE);
  m_serial_json_read_clear(in);
  fclose(f);
  
  f = m_core_fopen ("a-mjson.dat", "wt");
  if (!f) abort();
  m_serial_json_write_init(out, f);
  ret = my2_out_serial(out, el2);
  assert (ret == M_SERIAL_OK_DONE);
  m_serial_json_write_clear(out);
  fclose(f);

  f = m_core_fopen ("a-mjson.dat", "rt");
  if (!f) abort();
  static const char expected[] = "{ \"activated\":false,\"data\":{ \"vala\":1742,\"valb\":-2.300000,\"valc\":true,\"vald\":\"This is a test\",\"vale\":[1,2,3],\"valf\":{\"is_bool\":true},\"valg\":[1,2,3,4,5,6],\"valh\":{\"steeve\":-4,\"jane\":3},\"vali\":3}}";
  static const char expected2[] = "{ \"activated\":false,\"data\":{ \"vala\":1742,\"valb\":-2.300000,\"valc\":true,\"vald\":\"This is a test\",\"vale\":[1,2,3],\"valf\":{\"is_bool\":true},\"valg\":[1,2,3,4,5,6],\"valh\":{\"jane\":3,\"steeve\":-4},\"vali\":3}}";
  char get[sizeof expected];
  char *unused2 = fgets (get, sizeof expected , f);
  assert(unused2 != NULL);
  // Workaround  string literal of length 4350 exceeds maximum length 4095
  if (!(strcmp(get, expected) == 0 || strcmp(get, expected2) == 0))
    assert (0);
  fclose(f);
  
  f = m_core_fopen ("a-mjson.dat", "rt");
  if (!f) abort();
  m_serial_json_read_init(in, f);
  ret = my2_in_serial(el1, in);
  assert (ret == M_SERIAL_OK_DONE);
  m_serial_json_read_clear(in);
  fclose(f);
  
  assert (my2_equal_p (el1, el2));

  my2_clear(el1);
  my2_clear(el2);
}

static void test_out_str_empty(void)
{
  m_serial_read_t  in;
  m_serial_write_t out;
  m_serial_return_code_t ret;
  my2_t el1, el2;
  my2_init(el1);
  my2_init(el2);
  
  string_t f;
  string_init(f);
  m_serial_str_json_write_init(out, f);
  ret = my2_out_serial(out, el1);
  assert (ret == M_SERIAL_OK_DONE);
  m_serial_str_json_write_clear(out);

  static const char expected[] = "{ \"activated\":false,\"data\":{ \"vala\":0,\"valb\":0.000000,\"valc\":false,\"vald\":\"\",\"vale\":[],\"valf\":{},\"valg\":[],\"valh\":{},\"vali\":0}";
  assert (string_equal_str_p(f, expected) == 0);
  
  m_serial_str_json_read_init(in, string_get_cstr(f) );
  ret = my2_in_serial(el2, in);
  assert (ret == M_SERIAL_OK_DONE);
  const char *end = m_serial_str_json_read_clear(in);
  assert (*end == 0);
  
  assert (my2_equal_p (el1, el2));

  my2_clear(el1);
  my2_clear(el2);
  string_clear(f);
}

static void test_out_str_fill(void)
{
  m_serial_read_t  in;
  m_serial_write_t out;
  m_serial_return_code_t ret;
  my2_t el1, el2;
  my2_init(el1);
  my2_init(el2);
  
  string_t f;
  string_init(f);
  string_init_set_str(f,
                      "{\n"
                      " \"activated\":false,\n"
                      "\"data\":   {\n"
                      "       \"valb\":  -2.300000 , \n"
                      "\"vale\": [1,2,3],\n"
                      "\"valg\": [1,2,3,4,5,6],\n"
                      "\"valh\": { \"jane\": 3, \"steeve\": -4 },\n"
                      "\"valf\": { \"is_bool\": true },\n"
                      "              \"vala\":1742,\n"
                      " \"vald\": \"This is a test\",\n"
                      "\"vali\": 3,"
                      "    \"valc\": true   } }\n");

  m_serial_str_json_read_init(in, string_get_cstr(f) );
  ret = my2_in_serial(el2, in);
  assert (ret == M_SERIAL_OK_DONE);
  const char *end = m_serial_str_json_read_clear(in);
  assert (*end == '\n');
  string_reset(f);

  m_serial_str_json_write_init(out, f);
  ret = my2_out_serial(out, el2);
  assert (ret == M_SERIAL_OK_DONE);
  m_serial_str_json_write_clear(out);

  // In function of the precise hashing (different between 64 bits & 32 bits), both strings are possible: 
  static const char expected[] = "{ \"activated\":false,\"data\":{ \"vala\":1742,\"valb\":-2.300000,\"valc\":true,\"vald\":\"This is a test\",\"vale\":[1,2,3],\"valf\":{\"is_bool\":true},\"valg\":[1,2,3,4,5,6],\"valh\":{\"steeve\":-4,\"jane\":3},\"vali\":3}}";
  static const char expected2[] = "{ \"activated\":false,\"data\":{ \"vala\":1742,\"valb\":-2.300000,\"valc\":true,\"vald\":\"This is a test\",\"vale\":[1,2,3],\"valf\":{\"is_bool\":true},\"valg\":[1,2,3,4,5,6],\"valh\":{\"jane\":3,\"steeve\":-4},\"vali\":3}}";
  assert(string_equal_str_p(f, expected) || string_equal_str_p(f, expected2) );
  
  m_serial_str_json_read_init(in, string_get_cstr(f) );
  ret = my2_in_serial(el1, in);
  assert (ret == M_SERIAL_OK_DONE);
  end = m_serial_str_json_read_clear(in);
  assert (*end == 0);
  
  assert (my2_equal_p (el1, el2));

  my2_clear(el1);
  my2_clear(el2);
  string_clear(f);
}


int main(void)
{
  test_out_empty();
  test_out_fill();
  test_out_str_empty();
  test_out_str_fill();
  exit(0);    
}

#else
int main(void)
{
  exit(0);    
}
#endif
