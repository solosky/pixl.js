#include <string.h>

#include "benchmark.h"
#include "m-dict.h"
#include "m-mempool.h"

static inline bool oor_equal_p(int k, unsigned char n)
{
  return k == (int)-n - 1;
}

static inline void oor_set(int *k, unsigned char n)
{
  *k = (int)-n - 1;
}

DICT_OA_DEF2(dict_int,
             int, M_OPEXTEND(M_DEFAULT_OPLIST, OOR_EQUAL(oor_equal_p), OOR_SET(oor_set M_IPTR)),
             int, M_DEFAULT_OPLIST)

static const char *const oor[2] = { "EMPTY", "DELETED" };

static inline bool str_oor_equal_p(const char *k, unsigned char n)
{
return k == oor[n];
}

static inline void str_oor_set(const char * *k, unsigned char n)
{
*k = oor[n];
}

DICT_OA_DEF2(dict_str, const char *,
          M_OPEXTEND (M_CSTR_OPLIST, OOR_EQUAL(str_oor_equal_p), OOR_SET(str_oor_set M_IPTR)),
          int, M_DEFAULT_OPLIST)


int test_int(int N, const unsigned *data)
{
  int ret;
  M_LET(h, DICT_OPLIST(dict_int)) {
    for (int i = 0; i < N; ++i) {
      if (dict_int_get (h, data[i]) == NULL) {
        dict_int_set_at (h, data[i], i);
      } else {
        dict_int_erase(h, data[i]);
      }
    }
    ret = (int)dict_int_size(h);
  }
  return ret;
}

int test_str(int N, char * const *data)
{
  int ret;
  M_LET(h, DICT_OPLIST(dict_str)) {
    for (int i = 0; i < N; ++i) {
      if (dict_str_get (h, data[i]) == NULL) {
        dict_str_set_at (h, data[i], i);
      } else {
        dict_str_erase(h, data[i]);
      }
    }
    ret = (int)dict_str_size(h);
  }
  return ret;
}

int main(int argc, char *argv[])
{
  return udb_benchmark(argc, argv, test_int, test_str);
}
