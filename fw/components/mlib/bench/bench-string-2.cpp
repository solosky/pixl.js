/*
 * M*LIB - String Bench
 *
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
#define NDEBUG

#include <string>
#include <sstream>
#include <string>
#include <vector>

#define M_USE_SMALL_NAME 0
#include "common.h"

#include "m-string.h"

#ifdef BENCH_CAN_USE_SDS
extern "C" {
#include "sds.h"
};
#endif

#ifdef BENCH_CAN_USE_BSTRLIB
#include "bstrlib.h"
#include "bstrwrap.h"
#include "bstraux.h"
#endif

#if defined(BENCH_CAN_USE_POTTERY)
#include "pottery/string/string.h"
#endif

/*
 * BENCH
 * 2000000
 * 1. Generate N strings of random numbers 32 bits.
 * 2. Concat all strings in a string usigned a random permutation of N.
 * 3. Replace "1234" in the string into "WELL" until nothing remains.
 * 4. Replace "56789" in the string into "DONE" until nothing remains.
 * Return the final string length.
 */

unsigned *permutation_tab = NULL;

void random_permutation(unsigned n)
{
  if (permutation_tab) free(permutation_tab);
  permutation_tab = (unsigned *) malloc(n * sizeof(unsigned));
  if (permutation_tab == NULL) abort();
  for(unsigned i = 0; i < n; i++)
    permutation_tab[i] = i;
  for(unsigned i = 0; i < n; i++) {
    unsigned j = rand_get() % n;
    unsigned k = rand_get() % n;
    unsigned l = permutation_tab[j];
    permutation_tab[j] = permutation_tab[k];
    permutation_tab[k] = l;
  }
}

using namespace std;

static void replace_all_stl(string &str, const char pattern[], const char replace[])
{
  size_t pos = 0;
  do {
    pos = str.find(pattern, pos);
    if (pos != string::npos) {
      str.replace (pos, strlen(pattern), replace);
    }
  } while (pos != string::npos);
}

size_t bench_stl(unsigned n)
{
  vector<string> tab;
  tab.resize(n);
  for(unsigned i = 0; i < n; i++)
    tab[i] = to_string(rand_get());

  string str;
  for(unsigned i = 0; i < n; i++) {
    str += tab[permutation_tab[i]];
  }

  replace_all_stl(str, "1234", "WELL");
  replace_all_stl(str, "56789", "DONE");

  return str.length();
}

size_t bench_mlib(unsigned n)
{
  m_string_t *tab = (m_string_t*) malloc (n * sizeof (m_string_t));
  assert (tab != 0);
  // P1
  for(unsigned i = 0; i < n; i++) {
    m_string_init(tab[i]);
    m_string_set_ui(tab[i], rand_get());
  }
  // P2
  m_string_t str;
  m_string_init(str);
  for(unsigned i = 0; i < n; i++) {
    m_string_cat(str, tab[permutation_tab[i]]);
  }
  // P3
  m_string_replace_all_cstr(str, "1234", "WELL");
  m_string_replace_all_cstr(str, "56789", "DONE");
  size_t length = m_string_size(str);

  // Clean
  m_string_clear(str);
  for(unsigned i = 0; i < n; i++) {
    m_string_clear(tab[i]);
  }
  free(tab);
  return length;
}


#ifdef BENCH_CAN_USE_BSTRLIB

static void replace_all_bstrlib(CBString &str, const char pattern[], const char replace[])
{
  size_t pos = 0;
  do {
    pos = str.find(pattern, pos);
    if (pos != string::npos) {
      str.replace (pos, strlen(pattern), replace);
    }
  } while (pos != string::npos);
}

size_t bench_bstrlib(unsigned n)
{
  vector<CBString> tab;
  tab.resize(n);
  for(unsigned i = 0; i < n; i++) {
    char tmp[10];
    sprintf(tmp, "%u", rand_get());
    tab[i] = tmp;
  }

  CBString str;
  for(unsigned i = 0; i < n; i++) {
    str += tab[permutation_tab[i]];
  }

  replace_all_bstrlib(str, "1234", "WELL");
  replace_all_bstrlib(str, "56789", "DONE");

  return str.length();
}
#endif

#ifdef BENCH_CAN_USE_SDS
sds SDS_replace_at(sds str, size_t pos, size_t len, const char str2[])
{
  // simple implementation as replace is not available
  sds a = sdsnewlen((const void*) str, pos);
  a = sdscat(a, str2);
  a = sdscat(a, &str[pos+len]);
  sdsfree(str);
  return a;
}

sds replace_all_sds(sds str, const char pattern[], const char replace[])
{
  size_t pos = 0;
  do {
    char *p = strstr(&str[pos], pattern);
    pos = p == NULL ? -1U : (p - str);
    if (pos != -1U) {
      str = SDS_replace_at(str, pos, strlen(pattern), replace);
    }
  } while (pos != -1U);
  return str;
}

size_t bench_sds(unsigned n)
{
  sds *tab = (sds*) malloc (n * sizeof (sds));
  assert (tab != 0);
  // P1
  for(unsigned i = 0; i < n; i++) {
    tab[i] =  sdsfromlonglong(rand_get());
  }
  // P2
  sds str = sdsempty();
  for(unsigned i = 0; i < n; i++) {
    str = sdscat(str, tab[permutation_tab[i]]);
  }
  // P3
  str = replace_all_sds(str, "1234", "WELL");
  str = replace_all_sds(str, "56789", "DONE");
  // Clean
  size_t length = sdslen(str);
  sdsfree(str);
  for(unsigned i = 0; i < n; i++) {
    sdsfree(tab[i]);
  }
  free(tab);
  return length;
}
#endif

#if defined(BENCH_CAN_USE_POTTERY)

size_t pottery_find_cstr(string_t *b, size_t index, const char *pattern)
{
  // I cannot find such function in the API
  const char *s = string_cstr(b);
  const char *r = strstr(&s[index], pattern);
  return r == NULL ? SIZE_MAX : r - s;
}

void pottery_replace_all_str(string_t *str, const char *s, const char *d)
{
  size_t i = 0;
  while (true) {
    i = pottery_find_cstr(str, i, s);
    if (i==SIZE_MAX) return;
    string_remove(str, i, strlen(s));
    string_insert_cstr(str, i, d);
  }
}

size_t bench_pottery(unsigned n)
{
  string_t *tab = (string_t*) malloc (n * sizeof (string_t));
  assert (tab != 0);
  // P1
  for(unsigned i = 0; i < n; i++) {
    string_init(&tab[i]);
    string_append_format(&tab[i], "%u", rand_get());
  }
  // P2
  string_t str;
  string_init(&str);
  for(unsigned i = 0; i < n; i++) {
    string_append_string(&str, &tab[permutation_tab[i]]);
  }
  // P3
  
  pottery_replace_all_str(&str, "1234", "WELL");
  pottery_replace_all_str(&str, "56789", "DONE");
  size_t length = string_length(&str);

  // Clean
  string_clear(&str);
  for(unsigned i = 0; i < n; i++) {
    string_clear(&tab[i]);
  }
  free(tab);
  return length;
}
#endif

int main(int argc, const char *argv[])
{
  unsigned length, n = atoi(argv[1]);
  unsigned select = atoi(argv[2]);
  const char *name;
  random_permutation(n);
  rand_init();
  unsigned long long t0 = cputime();

  switch (select) {
  case 0:
    length = bench_mlib(n);
    name = "MLIB";
    break;
  case 1:
    length = bench_stl(n);
    name = "STL";
    break;
#ifdef BENCH_CAN_USE_SDS
  case 2:
    length = bench_sds(n);
    name = "SDS";
    break;
#endif
#ifdef BENCH_CAN_USE_BSTRLIB
  case 4:
    length = bench_bstrlib(n);
    name = "BSTRLIB";
    break;
#endif
#if defined(BENCH_CAN_USE_POTTERY)
  case 5:
    length = bench_pottery(n);
    name = "POTTERY";
    break;
#endif
  default:
    exit(0);
  }
  unsigned long long t1 = cputime();
  printf("%5.5s LENGTH=%u T= %f s\n", name, length, (t1-t0) / 1000000.0 );

  return 0;
}
