#define NDEBUG

#include <vector>
#include <deque>
#include <list>
#include <iterator>
#include <map>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <sstream>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

using namespace std;

#include "common.h"

/********************************************************************************************/

static void test_list(size_t n)
{
  list<unsigned int> a1, a2;
  for(size_t i = 0; i < n; i++) {
    a1.push_back(rand_get() );
    a2.push_back(rand_get() );
  }
  unsigned int s = 0;
  for (list<unsigned int>::const_iterator ci1 = a1.begin(), ci2 = a2.begin (); ci1 != a1.end(); ++ci1, ++ci2) {
    s += *ci1 * *ci2;
  }
  g_result = s;
}

/********************************************************************************************/

static void test_array(size_t n)
{
  vector<unsigned int> a1, a2;
  for(size_t i = 0; i < n; i++) {
    a1.push_back(rand_get() );
    a2.push_back(rand_get() );
  }
  unsigned int s = 0;
  for(unsigned long i = 0; i < n; i++) {
    s += a1[i] * a2[i];
  }
  g_result = s;
}

/********************************************************************************************/

static void test_deque(size_t n)
{
  deque<unsigned int> a1, a2;
  for(size_t i = 0; i < n; i++) {
    a1.push_back(rand_get() );
    a2.push_back(rand_get() );
  }
  unsigned int s = 0;
  for(auto it1 = a1.cbegin(), it2 = a2.cbegin();
      it1 != a1.cend() ;it1++, it2++) {
    s += *it1 * *it2;
  }
  g_result = s;
}

/********************************************************************************************/

static void test_rbtree(size_t n)
{
  set<unsigned long> tree;

  for (size_t i = 0; i < n; i++) {
    tree.insert(rand_get());
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    set<unsigned long>::iterator it = tree.find(rand_get());
    if (it != tree.end())
      s += *it;
  }
  g_result = s;
}

/********************************************************************************************/

static void
test_dict1(size_t  n)
{
  map<unsigned long, unsigned long> dict;

  for (size_t i = 0; i < n; i++) {
    dict[rand_get()] = rand_get();
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    map<unsigned long, unsigned long>::iterator it = dict.find(rand_get());
    if (it != dict.end())
      s += it->second;
  }
  g_result = s;
}

/********************************************************************************************/

static void
test_dict2(size_t  n)
{
  unordered_map<unsigned long, unsigned long> dict;

  for (size_t i = 0; i < n; i++) {
    dict[rand_get()] = rand_get();
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    unordered_map<unsigned long, unsigned long>::iterator it = dict.find(rand_get());
    if (it != dict.end())
      s += it->second;
  }
  g_result = s;
}

static void
test_dict2_linear(size_t  n)
{
  unordered_map<unsigned long, unsigned long> dict;

  for (size_t i = 0; i < n; i++) {
    dict[i] = rand_get();
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    unordered_map<unsigned long, unsigned long>::iterator it = dict.find(i);
    if (it != dict.end())
      s += it->second;
  }
  g_result = s;
}

/********************************************************************************************/

struct char_array_s {
  char a[256];
  char_array_s () { a[0] = 0 ; }
  char_array_s ( const char_array_s & other) { strcpy(a, other.a); }
  bool operator==(const char_array_s &other) const { return strcmp(a, other.a) == 0; }
};

namespace std {
  template <> struct hash<char_array_s> {
    std::size_t operator()(const char_array_s &k) const {
      size_t hash = 0;
      const char *s = k.a;
      while (*s) hash = hash * 31421 + (*s++) + 6927;
      return hash;
    };
  };
}

static void
test_dict_big(size_t  n)
{
  unordered_map<char_array_s, char_array_s> dict;

  for (size_t i = 0; i < n; i++) {
    char_array_s s1, s2;
    sprintf(s1.a, "%u", rand_get());
    sprintf(s2.a, "%u", rand_get());
    dict[s1] = s2;
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    char_array_s s1;
    sprintf(s1.a, "%u", rand_get());
    unordered_map<char_array_s, char_array_s>::iterator it = dict.find(s1);
    if (it != dict.end())
      s ++;
  }
  g_result = s;
}

/********************************************************************************************/
static void
test_dict_str(size_t  n)
{
  unordered_map<string, string> dict;

  for (size_t i = 0; i < n; i++) {
    string s1 = to_string(rand_get());
    string s2 = to_string(rand_get());
    dict[s1] = s2;
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    string s1 = to_string (rand_get());
    unordered_map<string, string>::iterator it = dict.find(s1);
    if (it != dict.end())
      s ++;
  }
  g_result = s;
}

static void
test_dict_str2(size_t  n)
{
  map<string, string> dict;

  for (size_t i = 0; i < n; i++) {
    string s1 = to_string(rand_get());
    string s2 = to_string(rand_get());
    dict[s1] = s2;
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    string s1 = to_string(rand_get());
    map<string, string>::iterator it = dict.find(s1);
    if (it != dict.end())
      s ++;
  }
  g_result = s;
}

/********************************************************************************************/

static void test_sort(size_t n)
{
  vector<float> a1;
  for(size_t i = 0; i < n; i++) {
    a1.push_back(rand_get() );
  }
  sort(a1.begin(), a1.end());
  g_result = a1[0];
}

static void test_stable_sort(size_t n)
{
  vector<float> a1;
  for(size_t i = 0; i < n; i++) {
    a1.push_back(rand_get() );
  }
  stable_sort(a1.begin(), a1.end());
  g_result = a1[0];
}

/********************************************************************************************/

const config_func_t table[] = {
  { 10,    "List", 10000000, 0, test_list, 0},
  { 20,   "Array", 100000000, 0, test_array, 0},
  { 21,   "Deque", 100000000, 0, test_deque, 0},
  { 30,  "Rbtree", 1000000, 0, test_rbtree, 0},
  { 40,    "dict(m)", 1000000, 0, test_dict1, 0},
  { 41, "dictBig", 1000000, 0, test_dict_big, 0},
  { 42,"dict(u)", 1000000, 0, test_dict2, 0},
  { 43,"DictStr(u)", 1000000, 0, test_dict_str, 0},
  { 44,"DictStr(m)", 1000000, 0, test_dict_str2, 0},
  { 46, "dictLinear(u)", 1000000, 0, test_dict2_linear, 0},
  { 50,           "Sort",10000000, 0, test_sort, 0},
  { 51,    "Stable Sort",10000000, 0, test_stable_sort, 0}
};

int main(int argc, const char *argv[])
{
  test("STL", numberof(table), table, argc, argv);
  exit(0);
}
