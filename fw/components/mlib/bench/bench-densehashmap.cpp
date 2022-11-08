#define NDEBUG

#include <vector>
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

#include <google/dense_hash_map>

using google::dense_hash_map;
using namespace std;

#include "common.h"

/********************************************************************************************/

static void
test_dict2(size_t  n)
{
  dense_hash_map<unsigned long, unsigned long> dict;
  dict.set_empty_key(-1);
  dict.set_deleted_key(-2);

  for (size_t i = 0; i < n; i++) {
    dict[rand_get()] = rand_get();
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    dense_hash_map<unsigned long, unsigned long>::iterator it = dict.find(rand_get());
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
  namespace tr1 {
    template <> struct hash<char_array_s> {
      std::size_t operator()(char_array_s k) const {
	size_t hash = 0;
	const char *s = k.a;
	while (*s) hash = hash * 31421 + (*s++) + 6927;
	return hash;
      };
    };
  }
}

static void
test_dict_big(size_t  n)
{
  dense_hash_map<char_array_s, char_array_s> dict;
  struct char_array_s empty, del;
  memset(&empty, 0, sizeof empty);  empty.a[1] = 255;
  memset(&del, 0, sizeof del);  del.a[1] = 255;
  dict.set_empty_key(empty);
  dict.set_deleted_key(del);

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
    dense_hash_map<char_array_s, char_array_s>::iterator it = dict.find(s1);
    if (it != dict.end())
      s ++;
  }
  g_result = s;
}

/********************************************************************************************/
static void
test_dict_str(size_t  n)
{
  dense_hash_map<string, string> dict;
  string empty, del;
  empty = "";
  del = "DEL";
  dict.set_empty_key(empty);
  dict.set_deleted_key(del);

  for (size_t i = 0; i < n; i++) {
    string s1 = static_cast<ostringstream*>( &(ostringstream() << rand_get()) )->str();
    string s2 = static_cast<ostringstream*>( &(ostringstream() << rand_get()) )->str();
    dict[s1] = s2;
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    string s1 = static_cast<ostringstream*>( &(ostringstream() << rand_get()) )->str();
    dense_hash_map<string, string>::iterator it = dict.find(s1);
    if (it != dict.end())
      s ++;
  }
  g_result = s;
}

/********************************************************************************************/

const config_func_t table[] = {
  { 41, "dictBig", 1000000, 0, test_dict_big, 0},
  { 42,"dict", 1000000, 0, test_dict2, 0},
  { 43,    "DictStr", 1000000, 0, test_dict_str, 0}
};

int main(int argc, const char *argv[])
{
  test("DENSE-HASPMAP", numberof(table), table, argc, argv);
  exit(0);
}
