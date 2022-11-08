/*
 * This source file is part of the bstring string library as cppbench.cpp
 * The bstring string library is written by Paul Hsieh in 2002-2015, 
 * and is covered by the BSD open source license. See :
 * * https://github.com/websnarf/bstrlib
 * * http://bstring.sourceforge.net/
 * * http://prdownloads.sourceforge.net/bstring/examples-09282006.zip?download

 Copyright (c) 2014, Paul Hsieh
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 * Neither the name of bstrlib nor the names of its
 contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Update of integrating M*LIB, LIBSRT, SDS, GLIB & POTTERY
   Copyright (c) 2018-2022 Patrick Pelissier
 */

/*
 *  Benchmark based on the features tested by benchmark seen here:
 *  http://www.utilitycode.com/str/performance.aspx
 *
 *  The idea is to measure the performance empty constructors, char * 
 *  constructors, assignment, concatenation and scanning. 
 *
 */

#define NDEBUG

#include <stdio.h>
#include <time.h>
#include <limits.h>

#define M_USE_SMALL_NAME 0
#include "common.h"

#ifdef BENCH_CAN_USE_MSTARLIB
#include "m-string.h"
#endif

#ifdef BENCH_CAN_USE_BSTRLIB
#include "bstrlib.h"
#include "bstrwrap.h"
#include "bstraux.h"
#endif

#ifdef BENCH_CAN_USE_LIBSRT
#include "sstring.h"
#endif

#ifdef BENCH_CAN_USE_SDS
extern "C" {
#include "sds.h"
};
#endif

#if defined (BENCH_CAN_USE_STL)
#include <string>
#endif

#if defined(BENCH_CAN_USE_GLIB)
#include "gmodule.h"
#endif

#if defined(BENCH_CAN_USE_POTTERY)
#include "pottery/string/string.h"
#endif

/* Time of a test */
#define TEST_SECONDS (5)

/*
 * This macro puts a memory barrier and prevents the optimizer from
 * optimizing too much the inline functions and literraly breaking 
 * the benchmark by doing nothing.
 * Better than nothing, but far from perfect.
 */
#define BARRIER(addr) compiler_barrier((void*) addr)

bool print_csv = false;

int timeTest (double &res, int (*testfn) (int count), int count) {
  clock_t t0, t1;
  int p = 0;
  double x, c = 0;
  
  t0 = clock ();
  
  do {
    p += testfn (count);
    t1 = clock ();
    
    c += count;
    if (t1 == t0) {
      if (count < (INT_MAX / 2)) count += count;
      continue;
    }
    
    if (t1 - t0 >= TEST_SECONDS*CLOCKS_PER_SEC) break;
    
    x = (TEST_SECONDS*CLOCKS_PER_SEC - (t1 - t0)) * c / (t1 - t0);
    
    if (x > INT_MAX) x = INT_MAX;
    count = (int) x;
    if (count < 1000) count = 1000;
    
  } while (1);
  
  res = c * CLOCKS_PER_SEC / (t1 - t0);
  return p;
}

#define SMALLTESTSTRING1 ("hello")

#define TESTSTRING1 ("<sometag name=\"John Doe\" position=\"Executive VP Marketing\"/>")

#if defined (BENCH_CAN_USE_STL)
int testSTL_emptyCtor (int count) {
  int i, c = 0;
  for (i=0; i < count; i++) {
    std::string b;
    BARRIER(&b);
    c += b.length () ^ i;
  }
  return c;
}

int testSTL_nonemptyCtor (int count) {
  int i, c = 0;
  for (i=0; i < count; i++) {
    std::string b (TESTSTRING1);
    BARRIER(&b);
    c += b.length () ^ i;
  }
  return c;
}

int testSTL_smallnonemptyCtor (int count) {
  int i, c = 0;
  for (i=0; i < count; i++) {
    std::string b (SMALLTESTSTRING1);
    BARRIER(&b);
    c += b.length () ^ i;
  }
  return c;
}

int testSTL_cstrAssignment (int count) {
  int i, c = 0;
  std::string b;
  
  for (i=0; i < count; i++) {
    b = TESTSTRING1;
    BARRIER(&b);
    c += b.length () ^ i;
  }
  return c;
}

int testSTL_extraction (int count) {
  int i, c = 0;
  std::string b (TESTSTRING1);

  for (i=0; i < count; i++) {
    c += b[(i & 7)];
    c += b[(i & 7) ^ 8];
    c += b.c_str()[(i & 7) ^ 4] ^ i;
    BARRIER(&b);
  }
  return c;
}

int testSTL_scan (int count) {
int i, c = 0;
 std::string b ("Dot. 123. Some more data.");
 
 for (i=0; i < count; i++) {
   c += b.find ('.');
   c += b.find ("123");
   c += b.find_first_of ("sm") ^i;
   BARRIER(&b);
 }
 return c;
}

int testSTL_concat (int count) {
  int i, j, c = 0;
  std::string a (TESTSTRING1);
  std::string accum;
  
  for (j=0; j < count; j++) {
    accum = "";
    for (i=0; i < 250; i++) {
      accum += a;
      accum += "!!";
      BARRIER(&accum);
      c += accum.length() ^i;
    }
  }
  return c;
}

int testSTL_replace (int count) {
  int j, c = 0;
  std::string a (TESTSTRING1);
  
  for (j=0; j < count; j++) {
    a.replace (11, 4, "XXXXXX");
    a.replace (23, 2, "XXXXXX");
    a.replace ( 4, 8, "XX");
    BARRIER(&a);
    c += a.length () ^ j;
  }
  return c;
}

#endif

#ifdef BENCH_CAN_USE_BSTRLIB
int testCBS_emptyCtor (int count) {
  int i, c = 0;
  for (i=0; i < count; i++) {
    CBString b;
    BARRIER(&b);
    c += b.length ()^i;
  }
  return c;
}

int testCBS_nonemptyCtor (int count) {
  int i, c = 0;
  for (i=0; i < count; i++) {
    CBString b (TESTSTRING1);
    BARRIER(&b);
    c += b.length () ^i;
  }
  return c;
}

int testCBS_smallnonemptyCtor (int count) {
  int i, c = 0;
  for (i=0; i < count; i++) {
    CBString b (SMALLTESTSTRING1);
    BARRIER(&b);
    c += b.length () ^i;
  }
  return c;
}

int testCBS_cstrAssignment (int count) {
  int i, c = 0;
  CBString b;
  
  for (i=0; i < count; i++) {
    b = TESTSTRING1;
    BARRIER(&b);
    c += b.length () ^i;
  }
  return c;
}

int testCBS_extraction (int count) {
  int i, c = 0;
  CBString b (TESTSTRING1);
  
  for (i=0; i < count; i++) {
    c += b[(i & 7)];
    c += b[(i & 7) ^ 8];
    c += ((const char *)b)[(i & 7) ^ 4] ^i;
    BARRIER(&b);
  }
  return c;
}

int testCBS_scan (int count) {
  int i, c = 0;
  CBString b ("Dot. 123. Some more data.");
  
  for (c=i=0; i < count; i++) {
    c += b.find ('.');
    c += b.find ("123");
    c += b.findchr ("sm") ^i;
    BARRIER(&b);
  }
  return c;
}

int testCBS_concat (int count) {
  int i, j, c = 0;
  CBString a (TESTSTRING1);
  CBString accum;
  
  for (j=0; j < count; j++) {
    accum = "";
    for (i=0; i < 250; i++) {
      accum += a;
      accum += "!!";
      BARRIER(&accum);
      c+= accum.length() ^i;
    }
  }
  return c;
}

int testCBS_replace (int count) {
  int j, c = 0;
  CBString a (TESTSTRING1);

  for (j=0; j < count; j++) {
    a.replace (11, 4, "XXXXXX");
    a.replace (23, 2, "XXXXXX");
    a.replace ( 4, 8, "XX");
    BARRIER(&a);
    c += a.length () ^j;
  }
  return c;
}
#endif

#ifdef BENCH_CAN_USE_LIBSRT
int testSRT_emptyCtor (int count) {
  int i, c = 0;
  for (c=i=0; i < count; i++) {
    srt_string *b = ss_alloc(0);
    BARRIER(b);
    c += ss_size(b) ^i;
    ss_free(&b);
  }
  return c;
}

int testSRT_nonemptyCtor (int count) {
  int i, c = 0;
  for (c=i=0; i < count; i++) {
    srt_string *b = ss_dup_c(TESTSTRING1);
    BARRIER(b);
    c += ss_size(b) ^i;
    ss_free(&b);
  }
  return c;
}

int testSRT_smallnonemptyCtor (int count) {
  int i, c = 0;
  for (c=i=0; i < count; i++) {
    srt_string *b = ss_dup_c(SMALLTESTSTRING1);
    BARRIER(b);
    c += ss_size(b) ^i;
    ss_free(&b);
  }
  return c;
}

int testSRT_cstrAssignment (int count) {
  int i, c = 0;
  srt_string *b = ss_alloc(0);
  for (c=i=0; i < count; i++) {
    ss_cpy_cn(&b, TESTSTRING1, strlen(TESTSTRING1));
    BARRIER(b);
    c += ss_size(b) ^i;
  }
  ss_free(&b);
  return c;
}

int testSRT_extraction (int count) {
  int i, c = 0;
  srt_string *b = ss_dup_c(TESTSTRING1);
  for (c=i=0; i < count; i++) {
    c += ss_at(b,(i & 7));
    c += ss_at(b,(i & 7) ^ 8);
    c += ss_at(b,(i & 7) ^ 4) ^i;
    BARRIER(b);
  }
  ss_free(&b);
  return c;
}

int testSRT_scan (int count) {
  int i, c = 0;
  srt_string *b = ss_dup_c("Dot. 123. Some more data.");
  for (c=i=0; i < count; i++) {
    c += ss_findc (b, 0, '.');
    c += ss_find_cn (b, 0, "123", strlen("123"));
    //c += ss_pbrk (b, 0, "sm") ^i;
    c += ss_findb (b, 0) ^i;
    BARRIER(b);
  }
  ss_free(&b);
  return c;
}


int testSRT_concat (int count) {
  int i, j, c = 0;
  srt_string *a = ss_dup_c(TESTSTRING1);
  srt_string *accum = ss_alloc(0);

  for (j=0; j < count; j++) {
    ss_cpy_c(&accum, "");
    for (i=0; i < 250; i++) {
      ss_cat(&accum, a, NULL);
      ss_cat_c(&accum, "!!", NULL);
      BARRIER(accum);
      c += ss_size(accum) ^i;
    }
  }
  ss_free(&a);
  ss_free(&accum);
  return c;
}

#endif

#ifdef BENCH_CAN_USE_MSTARLIB
int testMLIB_emptyCtor (int count) {
  int i, c = 0;
  for (c=i=0; i < count; i++) {
    m_string_t b;
    m_string_init(b);
    BARRIER(&b);
    c += m_string_size(b) ^i;
    m_string_clear(b);
  }
  return c;
}

int testMLIB_nonemptyCtor (int count) {
  int i, c = 0;
  for (c=i=0; i < count; i++) {
    m_string_t b;
    m_string_init_set_cstr(b, TESTSTRING1);
    BARRIER(&b);
    c += m_string_size(b) ^i;
    m_string_clear(b);
  }
  return c;
}

int testMLIB_smallnonemptyCtor (int count) {
  int i, c = 0;
  for (c=i=0; i < count; i++) {
    m_string_t b;
    m_string_init_set_cstr(b, SMALLTESTSTRING1);
    BARRIER(&b);
    c += m_string_size(b) ^i;
    m_string_clear(b);
  }
  return c;
}

int testMLIB_cstrAssignment (int count) {
  int i, c = 0;
  m_string_t b;
  m_string_init(b);
  for (c=i=0; i < count; i++) {
    m_string_set_cstr(b, TESTSTRING1);
    BARRIER(&b);
    c += m_string_size(b) ^i;
  }
  m_string_clear(b);
  return c;
}

int testMLIB_extraction (int count) {
  int i, c = 0;

  m_string_t b;
  m_string_init_set_cstr(b, TESTSTRING1);
  
  for (c=i=0; i < count; i++) {
    c += m_string_get_char(b, (i & 7));
    c += m_string_get_char(b, (i & 7) ^ 8);
    c += m_string_get_char(b, (i & 7) ^ 4) ^i;
    BARRIER(&b);
  }
  m_string_clear(b);
  return c;
}

int testMLIB_scan (int count) {
  int i, c = 0;
  m_string_t b;
  m_string_init_set_cstr(b, "Dot. 123. Some more data.");

  for (c=i=0; i < count; i++) {
    c += m_string_search_char (b, '.');
    c += m_string_search_cstr (b, "123");
    c += m_string_search_pbrk (b, "sm") ^i;
    BARRIER(&b);
  }
  m_string_clear(b);
  return c;
}

int testMLIB_concat (int count) {
  int i, j, c = 0;
  m_string_t a, accum;
  m_string_init_set_cstr(a, TESTSTRING1);
  m_string_init (accum);

  for (j=0; j < count; j++) {
    m_string_set_cstr(accum, "");
    for (i=0; i < 250; i++) {
      m_string_cat(accum, a);
      m_string_cat_cstr(accum, "!!");
      BARRIER(&accum);
      c += m_string_size(accum) ^i;
    }
  }
  m_string_clear(a);
  m_string_clear(accum);
  return c;
}

int testMLIB_replace (int count) {
  int j, c = 0;
  m_string_t a;
  m_string_init_set_cstr(a, TESTSTRING1);
  
  for (j=0; j < count; j++) {
    m_string_replace_at(a, 11, 4, "XXXXXX");
    m_string_replace_at(a, 23, 2, "XXXXXX");
    m_string_replace_at(a, 4, 8, "XX");
    BARRIER(&a);
    c += m_string_size(a) ^j;
  }
  
  m_string_clear(a);
  return c;
}
#endif

#ifdef BENCH_CAN_USE_SDS
int testSDS_emptyCtor (int count) {
  int i, c = 0;
  for (c=i=0; i < count; i++) {
    sds b;
    b = sdsempty();
    BARRIER(b);
    c += sdslen(b) ^ i;
    sdsfree(b);
  }
  return c;
}

int testSDS_nonemptyCtor (int count) {
  int i, c = 0;
  for (c=i=0; i < count; i++) {
    sds b;
    b = sdsnew(TESTSTRING1);
    BARRIER(b);
    c += sdslen(b) ^i;
    sdsfree(b);
  }
  return c;
}

int testSDS_smallnonemptyCtor (int count) {
  int i, c = 0;
  for (c=i=0; i < count; i++) {
    sds b;
    b = sdsnew(SMALLTESTSTRING1);
    BARRIER(b);
    c += sdslen(b) ^i;
    sdsfree(b);
  }
  return c;
}

int testSDS_cstrAssignment (int count) {
  int i, c = 0;
  sds b;
  b = sdsempty();
  for (c=i=0; i < count; i++) {
    b = sdscpy(b, TESTSTRING1);
    BARRIER(b);
    c += sdslen(b) ^i;
  }
  sdsfree(b);
  return c;
}

int testSDS_extraction (int count) {
  int i, c = 0;

  sds b;
  b = sdsnew(TESTSTRING1);
  
  for (c=i=0; i < count; i++) {
    c += b[(i & 7)];
    c += b[(i & 7) ^ 8];
    c += b[(i & 7) ^ 4] ^i;
    BARRIER(b);
  }
  sdsfree(b);
  return c;
}

int testSDS_scan (int count) {
  int i, c = 0;
  sds b;
  b = sdsnew("Dot. 123. Some more data.");

  for (c=i=0; i < count; i++) {
    c += (intptr_t) strchr(b, '.');
    c += (intptr_t) strstr (b, "123");
    c += (intptr_t) strpbrk (b, "sm") ^i;
    BARRIER(b);
  }
  sdsfree(b);
  return c;
}


int testSDS_concat (int count) {
  int i, j, c = 0;
  sds a, accum;
  a = sdsnew(TESTSTRING1);
  accum = sdsempty();

  for (j=0; j < count; j++) {
    accum = sdscpy(accum, "");
    for (i=0; i < 250; i++) {
      accum = sdscat(accum, a);
      accum = sdscat(accum, "!!");
      BARRIER(accum);
      c += sdslen(accum) ^i;
    }
  }
  sdsfree(a);
  sdsfree(accum);
  return c;
}

sds SDS_replace_at(sds str, size_t pos, size_t len, const char str2[])
{
  // simple implementation as replace is not available
  sds a = sdsnewlen((const void*) str, pos);
  a = sdscat(a, str2);
  a = sdscat(a, &str[pos+len]);
  sdsfree(str);
  return a;
}

int testSDS_replace (int count) {
  int j, c = 0;
  sds a;
  a = sdsnew(TESTSTRING1);
  for (j=0; j < count; j++) {
    a = SDS_replace_at(a, 11, 4, "XXXXXX");
    a = SDS_replace_at(a, 23, 2, "XXXXXX");
    a = SDS_replace_at(a, 4, 8, "XX");
    BARRIER(&a);
    c += sdslen(a) ^j;
  }  
  sdsfree(a);
  return c;
}

#endif


#ifdef BENCH_CAN_USE_GLIB
int testGLIB_emptyCtor (int count) {
  int i, c = 0;
  for (c=i=0; i < count; i++) {
    GString *b = g_string_new("");
    BARRIER(b);
    c += b->len;
    g_string_free(b, TRUE);
  }
  return c;
}

int testGLIB_nonemptyCtor (int count) {
  int i, c = 0;
  for (c=i=0; i < count; i++) {
    GString *b = g_string_new(TESTSTRING1);
    BARRIER(b);
    c += b->len;
    g_string_free(b, TRUE);
  }
  return c;
}

int testGLIB_smallnonemptyCtor (int count) {
  int i, c = 0;
  for (c=i=0; i < count; i++) {
    GString *b = g_string_new(SMALLTESTSTRING1);
    BARRIER(b);
    c += b->len;
    g_string_free(b, TRUE);
  }
  return c;
}

int testGLIB_cstrAssignment (int count) {
  int i, c = 0;
  GString *b = g_string_new("");
  for (c=i=0; i < count; i++) {
    b = g_string_assign(b, TESTSTRING1);
    BARRIER(b);
    c += b->len ^i;
  }
  g_string_free(b, TRUE);
  return c;
}

int testGLIB_extraction (int count) {
  int i, c = 0;
  GString *b = g_string_new(TESTSTRING1);
  for (c=i=0; i < count; i++) {
    c += b->str[(i & 7)];
    c += b->str[(i & 7) ^ 8];
    c += b->str[(i & 7) ^ 4] ^i;
    BARRIER(b);
  }
  g_string_free(b, TRUE);
  return c;
}

int testGLIB_scan (int count) {
  int i, c = 0;
  GString *b = g_string_new("Dot. 123. Some more data.");

  for (c=i=0; i < count; i++) {
    c += (strchr(b->str, '.')-b->str);
    c += (strstr (b->str, "123")-b->str);
    c += (strpbrk (b->str, "sm")-b->str) ^i;
    BARRIER(b);
  }
  g_string_free(b, TRUE);
  return c;
}


int testGLIB_concat (int count) {
  int i, j, c = 0;
  GString *a = g_string_new(TESTSTRING1);
  GString *accum = g_string_new("");

  for (j=0; j < count; j++) {
    accum = g_string_assign(accum, "");
    for (i=0; i < 250; i++) {
      accum = g_string_append(accum, a->str);
      accum = g_string_append(accum, "!!");
      BARRIER(accum);
      c += accum->len ^i;
    }
  }
  g_string_free(a, TRUE);
  g_string_free(accum, TRUE);
  return c;
}

//int testGLIB_replace (int count): not available
#endif

#ifdef BENCH_CAN_USE_POTTERY
int testPOTTERY_emptyCtor (int count) {
  int i, c = 0;
  for (c=i=0; i < count; i++) {
    string_t b;
    string_init(&b);
    BARRIER(&b);
    c += string_length(&b) ^i;
    string_clear(&b);
  }
  return c;
}

int testPOTTERY_nonemptyCtor (int count) {
  int i, c = 0;
  for (c=i=0; i < count; i++) {
    string_t b;
    string_init_cstr(&b, TESTSTRING1);
    BARRIER(&b);
    c += string_length(&b) ^i;
    string_clear(&b);
  }
  return c;
}

int testPOTTERY_smallnonemptyCtor (int count) {
  int i, c = 0;
  for (c=i=0; i < count; i++) {
    string_t b;
    string_init_cstr(&b, SMALLTESTSTRING1);
    BARRIER(&b);
    c += string_length(&b) ^i;
    string_clear(&b);
  }
  return c;
}

int testPOTTERY_cstrAssignment (int count) {
  int i, c = 0;
  string_t b;
  string_init(&b);
  for (c=i=0; i < count; i++) {
    string_set_cstr(&b, TESTSTRING1);
    BARRIER(&b);
    c += string_length(&b) ^i;
  }
  string_clear(&b);
  return c;
}

int testPOTTERY_extraction (int count) {
  int i, c = 0;

  string_t b;
  string_init_cstr(&b, TESTSTRING1);
  
  for (c=i=0; i < count; i++) {
    c += string_cstr(&b)[(i & 7)];
    c += string_cstr(&b)[(i & 7) ^ 8];
    c += string_cstr(&b)[(i & 7) ^ 4] ^i;
    BARRIER(&b);
  }
  string_clear(&b);
  return c;
}

size_t pottery_find_cstr(string_t *b, const char *pattern)
{
  // I cannot find such function in the API
  const char *s = string_cstr(b);
  const char *r = strstr(s, pattern);
  return r == NULL ? SIZE_MAX : r - s;
}

int testPOTTERY_scan (int count) {
  int i, c = 0;
  string_t b;
  string_init_cstr(&b, "Dot. 123. Some more data.");

  for (c=i=0; i < count; i++) {
    c += string_find_char (&b, 0, '.');
    c += pottery_find_cstr (&b, "123");
    c += string_find_chars_cstr (&b, 0, "sm") ^i;
    BARRIER(&b);
  }
  string_clear(&b);
  return c;
}

int testPOTTERY_concat (int count) {
  int i, j, c = 0;
  string_t a, accum;
  string_init_cstr(&a, TESTSTRING1);
  string_init (&accum);

  for (j=0; j < count; j++) {
    string_set_cstr(&accum, "");
    for (i=0; i < 250; i++) {
      string_append_string(&accum, &a);
      string_append_cstr(&accum, "!!");
      BARRIER(&accum);
      c += string_length(&accum) ^i;
    }
  }
  string_clear(&a);
  string_clear(&accum);
  return c;
}

// No replace function available
#endif


#define NTESTS 8
struct flags {
  int runtest[NTESTS];
};

static void
print(const char library[], const char function[], double cps)
{
  if (print_csv == false)
    {
      printf ("%16s %30s : %20.1f per second\n", library, function, cps);
    }
  else
    {
      printf ("\"%s\";\"%s\";%f\n", library, function, cps);
    }
}

int benchTest (const struct flags * runflags) {
  int c = 0;
  double cps;
  
#if defined (BENCH_CAN_USE_STL)
  if (runflags->runtest[0]) {
    c += timeTest (cps, testSTL_emptyCtor, 100000);
    print ("std::string", "empty constructor", cps);
  }
  if (runflags->runtest[1]) {
    c += timeTest (cps, testSTL_nonemptyCtor, 100000);
    print ("std::string", "non-empty constructor", cps);
  }
  if (runflags->runtest[2]) {
    c += timeTest (cps, testSTL_smallnonemptyCtor, 100000);
    print ("std::string", "small non-empty constructor", cps);
  }
  if (runflags->runtest[3]) {
    c += timeTest (cps, testSTL_cstrAssignment, 100000);
    print ("std::string", "char * assignment", cps);
  }
  if (runflags->runtest[4]) {
    c += timeTest (cps, testSTL_extraction, 100000);
    print ("std::string", "char extraction", cps);
  }
  if (runflags->runtest[5]) {
    c += timeTest (cps, testSTL_scan, 100000);
    print ("std::string", "scan", cps);
  }
  if (runflags->runtest[6]) {
    c += timeTest (cps, testSTL_concat, 10);
    print ("std::string", "concatenation", cps * 250);
  }
  if (runflags->runtest[7]) {
    c += timeTest (cps, testSTL_replace, 10000);
    print ("std::string", "replace", cps);
  }
#endif
  
#ifdef BENCH_CAN_USE_BSTRLIB
  if (runflags->runtest[0]) {
    c += timeTest (cps, testCBS_emptyCtor, 100000);
    print ("CBString", "empty constructor", cps);
  }
  if (runflags->runtest[1]) {
    c += timeTest (cps, testCBS_nonemptyCtor, 100000);
    print ("CBString", "non-empty constructor", cps);
  }
  if (runflags->runtest[2]) {
    c += timeTest (cps, testCBS_smallnonemptyCtor, 100000);
    print ("CBString", "small non-empty constructor", cps);
  }
  if (runflags->runtest[3]) {
    c += timeTest (cps, testCBS_cstrAssignment, 100000);
    print ("CBString", "char * assignment", cps);
  }
  if (runflags->runtest[4]) {
    c += timeTest (cps, testCBS_extraction, 100000);
    print ("CBString", "char extraction", cps);
  }
  if (runflags->runtest[5]) {
    c += timeTest (cps, testCBS_scan, 100000);
    print ("CBString", "scan", cps);
  }
  if (runflags->runtest[6]) {
    c += timeTest (cps, testCBS_concat, 10);
    print ("CBString", "concatenation", cps * 250);
  }
  if (runflags->runtest[7]) {
    c += timeTest (cps, testCBS_replace, 10000);
    print ("CBString", "replace", cps);
  }
#endif

#ifdef BENCH_CAN_USE_LIBSRT
  if (runflags->runtest[0]) {
    c += timeTest (cps, testSRT_emptyCtor, 100000);
    print ("LIBSRT", "empty constructor", cps);
  }
  if (runflags->runtest[1]) {
    c += timeTest (cps, testSRT_nonemptyCtor, 100000);
    print ("LIBSRT", "non-empty constructor", cps);
  }
  if (runflags->runtest[2]) {
    c += timeTest (cps, testSRT_smallnonemptyCtor, 100000);
    print ("LIBSRT", "small non-empty constructor", cps);
  }
  if (runflags->runtest[3]) {
    c += timeTest (cps, testSRT_cstrAssignment, 100000);
    print ("LIBSRT", "char * assignment", cps);
  }
  if (runflags->runtest[4]) {
    c += timeTest (cps, testSRT_extraction, 100000);
    print ("LIBSRT", "char extraction", cps);
  }
  if (runflags->runtest[5]) {
    c += timeTest (cps, testSRT_scan, 100000);
    print ("LIBSRT", "scan", cps);
  }
  if (runflags->runtest[6]) {
    c += timeTest (cps, testSRT_concat, 10);
    print ("LIBSRT", "concatenation", cps * 250);
  }
#endif
  
#ifdef BENCH_CAN_USE_MSTARLIB
  if (runflags->runtest[0]) {
    c += timeTest (cps, testMLIB_emptyCtor, 100000);
    print ("M*LIB", "empty constructor", cps);
  }
  if (runflags->runtest[1]) {
    c += timeTest (cps, testMLIB_nonemptyCtor, 100000);
    print ("M*LIB", "non-empty constructor", cps);
  }
  if (runflags->runtest[2]) {
    c += timeTest (cps, testMLIB_smallnonemptyCtor, 100000);
    print ("M*LIB", "small non-empty constructor", cps);
  }
  if (runflags->runtest[3]) {
    c += timeTest (cps, testMLIB_cstrAssignment, 100000);
    print ("M*LIB", "Char * assignment", cps);
  }
  if (runflags->runtest[4]) {
    c += timeTest (cps, testMLIB_extraction, 100000);
    print ("M*LIB", "char extraction", cps);
  }
  if (runflags->runtest[5]) {
    c += timeTest (cps, testMLIB_scan, 100000);
    print ("M*LIB", "scan", cps);
  }
  if (runflags->runtest[6]) {
    c += timeTest (cps, testMLIB_concat, 10);
    print ("M*LIB", "concatenation", cps * 250);
  }
  if (runflags->runtest[7]) {
    c += timeTest (cps, testMLIB_replace, 10000);
    print ("M*LIB", "replace", cps);
  }
#endif

#ifdef BENCH_CAN_USE_SDS
  if (runflags->runtest[0]) {
    c += timeTest (cps, testSDS_emptyCtor, 100000);
    print ("SDS", "empty constructor", cps);
  }
  if (runflags->runtest[1]) {
    c += timeTest (cps, testSDS_nonemptyCtor, 100000);
    print ("SDS", "non-empty constructor", cps);
  }
  if (runflags->runtest[2]) {
    c += timeTest (cps, testSDS_smallnonemptyCtor, 100000);
    print ("SDS", "small non-empty constructor", cps);
  }
  if (runflags->runtest[3]) {
    c += timeTest (cps, testSDS_cstrAssignment, 100000);
    print ("SDS", "char * assignment", cps);
  }
  if (runflags->runtest[4]) {
    c += timeTest (cps, testSDS_extraction, 100000);
    print ("SDS", "char extraction", cps);
  }
  if (runflags->runtest[5]) {
    c += timeTest (cps, testSDS_scan, 100000);
    print ("SDS", "scan", cps);
  }
  if (runflags->runtest[6]) {
    c += timeTest (cps, testSDS_concat, 10);
    print ("SDS", "concatenation", cps * 250);
  }
  if (runflags->runtest[7]) {
    c += timeTest (cps, testSDS_replace, 10);
    print ("SDS", "replace", cps);
  }
#endif

#ifdef BENCH_CAN_USE_GLIB
  if (runflags->runtest[0]) {
    c += timeTest (cps, testGLIB_emptyCtor, 100000);
    print ("GLIB", "empty constructor", cps);
  }
  if (runflags->runtest[1]) {
    c += timeTest (cps, testGLIB_nonemptyCtor, 100000);
    print ("GLIB", "non-empty constructor", cps);
  }
  if (runflags->runtest[2]) {
    c += timeTest (cps, testGLIB_smallnonemptyCtor, 100000);
    print ("GLIB", "small non-empty constructor", cps);
  }
  if (runflags->runtest[3]) {
    c += timeTest (cps, testGLIB_cstrAssignment, 100000);
    print ("GLIB", "Char * assignment", cps);
  }
  if (runflags->runtest[4]) {
    c += timeTest (cps, testGLIB_extraction, 100000);
    print ("GLIB", "char extraction", cps);
  }
  if (runflags->runtest[5]) {
    c += timeTest (cps, testGLIB_scan, 100000);
    print ("GLIB", "scan", cps);
  }
  if (runflags->runtest[6]) {
    c += timeTest (cps, testGLIB_concat, 10);
    print ("GLIB", "concatenation", cps * 250);
  }
#endif

#ifdef BENCH_CAN_USE_POTTERY
  if (runflags->runtest[0]) {
    c += timeTest (cps, testPOTTERY_emptyCtor, 100000);
    print ("POTTERY", "empty constructor", cps);
  }
  if (runflags->runtest[1]) {
    c += timeTest (cps, testPOTTERY_nonemptyCtor, 100000);
    print ("POTTERY", "non-empty constructor", cps);
  }
  if (runflags->runtest[2]) {
    c += timeTest (cps, testPOTTERY_smallnonemptyCtor, 100000);
    print ("POTTERY", "small non-empty constructor", cps);
  }
  if (runflags->runtest[3]) {
    c += timeTest (cps, testPOTTERY_cstrAssignment, 100000);
    print ("POTTERY", "Char * assignment", cps);
  }
  if (runflags->runtest[4]) {
    c += timeTest (cps, testPOTTERY_extraction, 100000);
    print ("POTTERY", "char extraction", cps);
  }
  if (runflags->runtest[5]) {
    c += timeTest (cps, testPOTTERY_scan, 100000);
    print ("POTTERY", "scan", cps);
  }
  if (runflags->runtest[6]) {
    c += timeTest (cps, testPOTTERY_concat, 10);
    print ("POTTERY", "concatenation", cps * 250);
  }
#endif

  return c;
}

int main (int argc, char * argv[]) {
  struct flags runflags;
  int i, j;
  
  if (argc > 1 && strcmp(argv[1], "--csv") == 0) {
    print_csv = true;
    argc --;
    argv++;
  }
  
  for (i=0; i < NTESTS; i++) runflags.runtest[i] = argc < 2;
  for (i=1; i < argc; i++) {
    if (1 == sscanf (argv[i], "%d", &j)) {
      if (j >= 1 && j <= NTESTS) runflags.runtest[j-1] = 1;
    }
  }
  
  benchTest (&runflags);
  return 0;
}
