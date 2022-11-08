#include "../common.c"
#define M_USE_DICT_OA_PROBING(s) 1
#include "m-dict.h"

static inline bool oor_equal_p(unsigned int k, unsigned char n)
{
  return k == (unsigned int)n;
}
static inline void oor_set(unsigned int *k, unsigned char n)
{
  *k = (unsigned int)n;
}

DICT_OA_DEF2(dict_oa_uint,
	     unsigned int, M_OPEXTEND(M_DEFAULT_OPLIST, HASH(hash_fn), OOR_EQUAL(oor_equal_p), OOR_SET(oor_set M_IPTR)),
	     unsigned int, M_DEFAULT_OPLIST)

uint32_t test_int(uint32_t n, uint32_t x0)
{
  uint32_t i, x, z = 0;
  dict_oa_uint_t h;
  dict_oa_uint_init(h);
  for (i = 0, x = x0; i < n; ++i) {
    x = hash32(x);
    unsigned int key = get_key(n, x);
    unsigned int *ptr = dict_oa_uint_get_at(h, key);
    (*ptr)++;
    z += *ptr;
  }
  fprintf(stderr, "# unique keys: %d; checksum: %u\n", (int) dict_oa_uint_size(h), z);
  z = dict_oa_uint_size(h);
  dict_oa_uint_clear(h);
  return z;
}
