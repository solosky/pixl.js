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

#include "rigtorp/HashMap.h"

using namespace rigtorp;
using namespace std;

#include "common.h"

/********************************************************************************************/

// Hash for using unsigned long as lookup key
struct UL_Hash {
  size_t operator()(const unsigned long v) { return v * 7; }
};

// Equal comparison for using unsigned long as lookup key
struct UL_Equal {
  bool operator()(const unsigned long lhs, const unsigned long rhs) {
    return lhs == rhs;
  }
};

static void
test_dict2(size_t  n)
{
  HashMap<unsigned long, unsigned long, UL_Hash, UL_Equal> dict(16, -1UL);

  for (size_t i = 0; i < n; i++) {
    dict[rand_get()] = rand_get();
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    HashMap<unsigned long, unsigned long, UL_Hash, UL_Equal>::iterator it = dict.find(rand_get());
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

struct BIG_Hash {
  size_t operator()(const char_array_s v) { return hash<char_array_s>()(v); }
};

struct BIG_Equal {
  bool operator()(const char_array_s lhs, const char_array_s rhs) {
    return lhs == rhs;
  }
};

static void
test_dict_big(size_t  n)
{
  char_array_s empty;
  HashMap<char_array_s, char_array_s, BIG_Hash, BIG_Equal> dict(16, empty);

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
    HashMap<char_array_s, char_array_s, BIG_Hash, BIG_Equal>::iterator it = dict.find(s1);
    if (it != dict.end())
      s ++;
  }
  g_result = s;
}

/********************************************************************************************/

struct STR_Hash {
  size_t operator()(const string v) { return hash<string>()(v); }
};

struct STR_Equal {
  bool operator()(const string lhs, const string rhs) {
    return lhs == rhs;
  }
};


static void
test_dict_str(size_t  n)
{
  string empty;
  HashMap<string, string, STR_Hash, STR_Equal> dict(16, empty);

  for (size_t i = 0; i < n; i++) {
    string s1 = static_cast<ostringstream*>( &(ostringstream() << rand_get()) )->str();
    string s2 = static_cast<ostringstream*>( &(ostringstream() << rand_get()) )->str();
    dict[s1] = s2;
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    string s1 = static_cast<ostringstream*>( &(ostringstream() << rand_get()) )->str();
    HashMap<string, string, STR_Hash, STR_Equal>::iterator it = dict.find(s1);
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
  test("RIGTORP-HASHMAP", numberof(table), table, argc, argv);
  exit(0);
}
