#include <string.h>

#include "benchmark.h"
#include "m-dict.h"
#include "m-mempool.h"

DICT_STOREHASH_DEF2(dict_int,
          int, M_OPEXTEND(M_DEFAULT_OPLIST, MEMPOOL(mpool_int), MEMPOOL_LINKAGE(static)),
          int, M_DEFAULT_OPLIST)
DICT_STOREHASH_DEF2(dict_str,
          const char *, M_OPEXTEND (M_CSTR_OPLIST, MEMPOOL(mpool_str), MEMPOOL_LINKAGE(static)),
          int, M_DEFAULT_OPLIST)

int test_int(int N, const unsigned *data)
{
  int ret;
  dict_int_list_pair_mempool_init(mpool_int);
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
  dict_int_list_pair_mempool_clear(mpool_int);  
  return ret;
}

int test_str(int N, char * const *data)
{
  int ret;
  dict_str_list_pair_mempool_init(mpool_str);
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
  dict_str_list_pair_mempool_clear(mpool_str);
  return ret;
}

int main(int argc, char *argv[])
{
  return udb_benchmark(argc, argv, test_int, test_str);
}
