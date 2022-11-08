#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "m-array.h"
#include "m-list.h"
#include "m-rbtree.h"
#include "m-bptree.h"
#include "m-deque.h"
#include "m-dict.h"
#include "m-algo.h"
#include "m-mempool.h"
#include "m-string.h"
#include "m-buffer.h"
#include "m-mutex.h"
#include "m-deque.h"
#include "m-concurrent.h"
#include "m-tuple.h"
#include "m-serial-bin.h"
#include "m-serial-json.h"

#include "common.h"

/********************************************************************************************/

ARRAY_DEF(array_uint, unsigned int)

static void test_array(size_t n)
{
  M_LET(a1, a2, ARRAY_OPLIST(array_uint)) {
    for(size_t i = 0; i < n; i++) {
      array_uint_push_back(a1, rand_get() );
      array_uint_push_back(a2, rand_get() );
    }
    unsigned int s = 0;
    for(unsigned long i = 0; i < n; i++) {
      s += *array_uint_cget(a1, i ) * *array_uint_cget(a2, i );
    }
    g_result = s;
  }
}

/********************************************************************************************/

DEQUE_DEF(deque_uint, unsigned int)

static void test_deque(size_t n)
{
  M_LET(a1, a2, DEQUE_OPLIST(deque_uint)) {
    for(size_t i = 0; i < n; i++) {
      deque_uint_push_back(a1, rand_get() );
      deque_uint_push_back(a2, rand_get() );
    }
    unsigned int s = 0;
    deque_uint_it_t it1, it2;
    for(deque_uint_it(it1, a1), deque_uint_it(it2, a2) ; !deque_uint_end_p(it1) ; deque_uint_next(it1), deque_uint_next(it2)) {
      s += *deque_uint_cref(it1 ) * *deque_uint_cref(it2 );
    }
    g_result = s;
  }
}

/********************************************************************************************/

// Two benched mode: with or without mempool
#ifdef USE_MEMPOOL
LIST_DEF(list_uint, unsigned int, (MEMPOOL( list_mpool), MEMPOOL_LINKAGE(static)))
#else
LIST_DEF(list_uint, unsigned int)
#endif

static void test_list (size_t n)
{
#ifdef USE_MEMPOOL
  list_uint_mempool_init(list_mpool);
#endif
  M_LET(a1, a2, LIST_OPLIST(list_uint)) {
    for(size_t i = 0; i < n; i++) {
      list_uint_push_back(a1, rand_get() );
      list_uint_push_back(a2, rand_get() );
    }
    unsigned int s = 0;
    list_uint_it_t it1, it2;
    for(list_uint_it(it1, a1), list_uint_it(it2,a2); !list_uint_end_p(it1); list_uint_next(it1), list_uint_next(it2)) {
      s += *list_uint_cref(it1) * *list_uint_cref(it2);
    }
    g_result = s;
  }
#ifdef USE_MEMPOOL
  list_uint_mempool_clear(list_mpool);
#endif
}

/********************************************************************************************/

// Two benched mode: with or without mempool
#ifdef USE_MEMPOOL
LIST_DUAL_PUSH_DEF(dlist_uint, unsigned int, (MEMPOOL( dlist_mpool), MEMPOOL_LINKAGE(static)))
#else
LIST_DUAL_PUSH_DEF(dlist_uint, unsigned int)
#endif

static void test_dlist (size_t n)
{
#ifdef USE_MEMPOOL
  dlist_uint_mempool_init(dlist_mpool);
#endif
  M_LET(a1, a2, LIST_OPLIST(dlist_uint)) {
    for(size_t i = 0; i < n; i++) {
      dlist_uint_push_back(a1, rand_get() );
      dlist_uint_push_back(a2, rand_get() );
    }
    unsigned int s = 0;
    dlist_uint_it_t it1, it2;
    for(dlist_uint_it(it1, a1), dlist_uint_it(it2,a2); !dlist_uint_end_p(it1); dlist_uint_next(it1), dlist_uint_next(it2)) {
      s += *dlist_uint_cref(it1) * *dlist_uint_cref(it2);
    }
    g_result = s;
  }
#ifdef USE_MEMPOOL
  dlist_uint_mempool_clear(dlist_mpool);
#endif
}

/********************************************************************************************/

#ifdef USE_MEMPOOL
RBTREE_DEF(rbtree_ulong, unsigned long, (MEMPOOL( rbtree_mpool), MEMPOOL_LINKAGE(static)))
#else
RBTREE_DEF(rbtree_ulong, unsigned long)
#endif

static void test_rbtree(size_t n)
{
#ifdef USE_MEMPOOL
  rbtree_ulong_mempool_init(rbtree_mpool);
#endif
  M_LET(tree, RBTREE_OPLIST(rbtree_ulong)) {
    for (size_t i = 0; i < n; i++) {
      rbtree_ulong_push(tree, rand_get());
    }
    rand_init();
    unsigned int s = 0;
    for (size_t i = 0; i < n; i++) {
      unsigned long *p = rbtree_ulong_get(tree, rand_get());
      if (p)
        s += *p;
    }
    g_result = s;
  }
#ifdef USE_MEMPOOL
  rbtree_ulong_mempool_clear(rbtree_mpool);
#endif
}

/********************************************************************************************/

BPTREE_DEF(bptree_ulong, 21, unsigned long, M_DEFAULT_OPLIST)

static void test_bptree(size_t n)
{
  M_LET(tree, BPTREE_OPLIST(bptree_ulong)) {
      for (size_t i = 0; i < n; i++) {
        bptree_ulong_push(tree, rand_get());
      }
      rand_init();
      unsigned int s = 0;
      for (size_t i = 0; i < n; i++) {
        unsigned long *p = bptree_ulong_get(tree, rand_get());
        if (p)
          s += *p;
      }
      g_result = s;
  }
}

/********************************************************************************************/

#ifdef USE_MEMPOOL
DICT_DEF2(dict_ulong, unsigned long, M_OPEXTEND(M_DEFAULT_OPLIST, MEMPOOL(dict_mpool), MEMPOOL_LINKAGE(static)), unsigned long, M_DEFAULT_OPLIST)
#else
DICT_DEF2(dict_ulong, unsigned long, M_DEFAULT_OPLIST, unsigned long, M_DEFAULT_OPLIST)
#endif

static void
test_dict(size_t  n)
{
#ifdef USE_MEMPOOL
  dict_ulong_list_pair_mempool_init(dict_mpool);
#endif
  M_LET(dict, DICT_OPLIST(dict_ulong)) {
    for (size_t i = 0; i < n; i++) {
      unsigned long value = rand_get();
      unsigned long key = rand_get();
      dict_ulong_set_at(dict, key, value);
    }
    rand_init();
    unsigned int s = 0;
    for (size_t i = 0; i < n; i++) {
      unsigned long *p = dict_ulong_get(dict, rand_get());
      if (p)
        s += *p;
    }
    g_result = s;
  }
#ifdef USE_MEMPOOL
  dict_ulong_list_pair_mempool_clear(dict_mpool);
#endif
}

/********************************************************************************************/

static inline bool oor_equal_p(unsigned long k, unsigned char n)
{
  return k == (unsigned long)n;
}

static inline void oor_set(unsigned long *k, unsigned char n)
{
  *k = (unsigned long)n;
}

DICT_OA_DEF2(dict_oa_ulong,
	     unsigned long, M_OPEXTEND(M_DEFAULT_OPLIST, OOR_EQUAL(oor_equal_p), OOR_SET(oor_set M_IPTR)),
	     unsigned long, M_DEFAULT_OPLIST)

static void
test_dict_oa(size_t  n)
{
  M_LET(dict, DICT_OPLIST(dict_oa_ulong)) {
    for (size_t i = 0; i < n; i++) {
      dict_oa_ulong_set_at(dict, rand_get(), rand_get() );
    }
    rand_init();
    unsigned int s = 0;
    for (size_t i = 0; i < n; i++) {
      unsigned long *p = dict_oa_ulong_get(dict, rand_get());
      if (p)
        s += *p;
    }
    g_result = s;
  }
}

static void
test_dict_oa_linear(size_t  n)
{
  M_LET(dict, DICT_OPLIST(dict_oa_ulong)) {
    for (size_t i = 0; i < n; i++) {
      dict_oa_ulong_set_at(dict, i, rand_get() );
    }
    rand_init();
    unsigned int s = 0;
    for (size_t i = 0; i < n; i++) {
      unsigned long *p = dict_oa_ulong_get(dict, i);
      if (p)
        s += *p;
    }
    g_result = s;
  }
}

/********************************************************************************************/
#ifndef CHAR_ARRAY_SIZE
#define CHAR_ARRAY_SIZE 256
#endif
typedef char char_array_t[CHAR_ARRAY_SIZE];
static void char_init (char_array_t a) { a[0] = 0; }
static void char_set (char_array_t a, const char_array_t b) { strcpy(a, b); }
static bool char_equal_p (const char_array_t a, const char_array_t b) { return strcmp(a,b)==0; }
static size_t char_hash(const char_array_t a) { return m_core_hash (a, strlen(a)); }
static bool char_oor_equal_p(const char_array_t a, unsigned char n) { return a[0] == 1+n; }
static void char_oor_set(char_array_t a, unsigned char n) { a[0] = 1+n; }
#define M_OPL_char_array_t() (INIT(char_init), INIT_SET(char_set), SET(char_set), CLEAR(char_init), HASH(char_hash), EQUAL(char_equal_p), OOR_EQUAL(char_oor_equal_p), OOR_SET(char_oor_set))

#ifdef USE_MEMPOOL
DICT_STOREHASH_DEF2(dict_char, char_array_t, M_OPEXTEND(M_OPL_char_array_t(),MEMPOOL(dict_mpool2), MEMPOOL_LINKAGE(static)), char_array_t, M_OPL_char_array_t())
#else
DICT_STOREHASH_DEF2(dict_char, char_array_t, M_OPL_char_array_t(), char_array_t, M_OPL_char_array_t())
#endif


static void
test_dict_big(size_t  n)
{
#ifdef USE_MEMPOOL
  dict_char_list_pair_mempool_init(dict_mpool2);
#endif
  M_LET(dict, DICT_OPLIST(dict_char)) {
    for (size_t i = 0; i < n; i++) {
      char_array_t s1, s2;
      sprintf(s1, "%u", rand_get());
      sprintf(s2, "%u", rand_get());
      dict_char_set_at(dict, s1, s2);
    }
    rand_init();
    unsigned int s = 0;
    for (size_t i = 0; i < n; i++) {
      char_array_t s1;
      sprintf(s1, "%u", rand_get());
      char_array_t *p = dict_char_get(dict, s1);
      if (p)
        s ++;
    }
    g_result = s;
  }
#ifdef USE_MEMPOOL
  dict_char_list_pair_mempool_clear(dict_mpool2);
#endif
}

DICT_OA_DEF2(dict_oa_char, char_array_t, char_array_t)

static void
test_dict_oa_big(size_t  n)
{
  M_LET(dict, DICT_OPLIST(dict_oa_char)) {
    for (size_t i = 0; i < n; i++) {
      char_array_t s1, s2;
      sprintf(s1, "%u", rand_get());
      sprintf(s2, "%u", rand_get());
      dict_oa_char_set_at(dict, s1, s2);
    }
    rand_init();
    unsigned int s = 0;
    for (size_t i = 0; i < n; i++) {
      char_array_t s1;
      sprintf(s1, "%u", rand_get());
      char_array_t *p = dict_oa_char_get(dict, s1);
      if (p)
        s ++;
    }
    g_result = s;
  }
}

/********************************************************************************************/

#ifdef USE_MEMPOOL
DICT_DEF2(dict_str, string_t, M_OPEXTEND(STRING_OPLIST, MEMPOOL(dict_mpool3), MEMPOOL_LINKAGE(static)), string_t, STRING_OPLIST)
#else
DICT_DEF2(dict_str, string_t, STRING_OPLIST, string_t, STRING_OPLIST)
#endif

static void
test_dict_str(size_t  n)
{
#ifdef USE_MEMPOOL
  dict_str_list_pair_mempool_init(dict_mpool3);
#endif
  M_LET(s1, s2, STRING_OPLIST)
  M_LET(dict, DICT_OPLIST(dict_str)) {
    for (size_t i = 0; i < n; i++) {
      string_printf(s1, "%u", rand_get());
      string_printf(s2, "%u", rand_get());
      dict_str_set_at(dict, s1, s2);
    }
    rand_init();
    unsigned int s = 0;
    for (size_t i = 0; i < n; i++) {
      string_printf(s1, "%u", rand_get());
      string_t *p = dict_str_get(dict, s1);
      if (p)
        s ++;
    }
    g_result = s;
  }
#ifdef USE_MEMPOOL
  dict_str_list_pair_mempool_clear(dict_mpool3);
#endif
}

/********************************************************************************************/

ARRAY_DEF(array_float, float)
ALGO_DEF(array_float, ARRAY_OPLIST(array_float))

static void test_sort(size_t n)
{
  M_LET(a1, ARRAY_OPLIST(array_float)) {
    for(size_t i = 0; i < n; i++) {
      array_float_push_back(a1, rand_get() );
    }
    array_float_sort(a1);
    g_result = *array_float_get(a1, 0);
  }
}

static void test_stable_sort(size_t n)
{
  M_LET(a1, ARRAY_OPLIST(array_float)) {
    for(size_t i = 0; i < n; i++) {
      array_float_push_back(a1, rand_get() );
    }
    array_float_special_stable_sort(a1);
    g_result = *array_float_get(a1, 0);
  }
}

/********************************************************************************************/

#define SIZE_LIMIT (UINT_MAX/2)

BUFFER_DEF(buffer_uint, unsigned int, 0, BUFFER_QUEUE|BUFFER_BLOCKING)
buffer_uint_t g_buff_lock;

BUFFER_DEF(buffer_ull, unsigned long long, 0, BUFFER_QUEUE|BUFFER_BLOCKING)
buffer_ull_t g_final_lock;

static void final_lock(void *arg)
{
  size_t *p_n = arg;
  size_t    n = *p_n;
  unsigned long long j, s = 0;
  for(int i = 0; i < n;i++) {
    buffer_ull_pop(&j, g_final_lock);
    s += j;
  }
  g_result = s;
}

static void conso_lock(void *arg)
{
  unsigned int j;
  size_t *p_n = arg;
  size_t n = *p_n;
  unsigned long long s = 0;
  for(int i = 0; i < n;i++) {
    buffer_uint_pop(&j, g_buff_lock);
    s += j;
  }
  buffer_ull_push(g_final_lock, s);
}

static void prod_lock(void *arg)
{
  size_t *p_n = arg;
  size_t n = *p_n;
  size_t r = n;
  for(unsigned int i = 0; i < n;i++) {
    buffer_uint_push(g_buff_lock, r );
    r = r * 31421U + 6927U;
  }
}

static void test_buffer(size_t n)
{
  const int cpu_count   = n > SIZE_LIMIT ? 2 : get_cpu_count();
  const int prod_count  = cpu_count/2;
  const int conso_count = cpu_count - prod_count;
  if (cpu_count < 2) {
    fprintf(stderr, "WARNING: Can not measure Buffer performance.\n");
    return;
  }
  n = n > SIZE_LIMIT ? n - SIZE_LIMIT : n;
  // Init
  buffer_uint_init(g_buff_lock, 64*cpu_count);
  buffer_ull_init (g_final_lock, 64*cpu_count);

  // Create thread
  m_thread_t idx_p[prod_count];
  m_thread_t idx_c[conso_count];
  m_thread_t idx_final;
  for(int i = 0; i < prod_count; i++) {
    m_thread_create (idx_p[i], prod_lock, &n);
  }
  for(int i = 0; i < conso_count; i++) {
    m_thread_create (idx_c[i], conso_lock, &n);
  }
  size_t n2 = conso_count;
  m_thread_create(idx_final, final_lock, &n2);

  // Wait for jobs to be done.
  for(int i = 0; i < prod_count; i++) {
    m_thread_join(idx_p[i]);
  }
  for(int i = 0; i < conso_count; i++) {
    m_thread_join(idx_c[i]);
  }
  m_thread_join(idx_final);

  // Clear & quit
  buffer_ull_clear(g_final_lock);
  buffer_uint_clear(g_buff_lock);
}

/********************************************************************************************/

QUEUE_MPMC_DEF(queue_uint, unsigned int, BUFFER_QUEUE)
queue_uint_t g_buff_mpmc;

QUEUE_MPMC_DEF(queue_ull, unsigned long long, BUFFER_QUEUE)
queue_ull_t g_final_mpmc;

static void final_mpmc(void *arg)
{
  size_t *p_n = arg;
  size_t    n = *p_n;
  unsigned long long j, s = 0;
  for(int i = 0; i < n;i++) {
    while (!queue_ull_pop(&j, g_final_mpmc)) m_thread_yield();
    s += j;
  }
  g_result = s;
}

static void conso_mpmc(void *arg)
{
  unsigned int j;
  size_t *p_n = arg;
  size_t n = *p_n;
  unsigned long long s = 0;
  for(int i = 0; i < n;i++) {
    while (!queue_uint_pop(&j, g_buff_mpmc)) m_thread_yield();
    s += j;
  }
  while (!queue_ull_push(g_final_mpmc, s));
}

static void prod_mpmc(void *arg)
{
  size_t *p_n = arg;
  size_t n = *p_n;
  size_t r = n;
  for(unsigned int i = 0; i < n;i++) {
    while (!queue_uint_push(g_buff_mpmc, r )) m_thread_yield();
    r = r * 31421U + 6927U;
  }
}

static void test_queue(size_t n)
{
  const int cpu_count   = n > SIZE_LIMIT ? 2 : get_cpu_count();
  const int prod_count  = cpu_count/2;
  const int conso_count = cpu_count - prod_count;
  if (cpu_count < 2) {
    fprintf(stderr, "WARNING: Can not measure Queue performance.\n");
    return;
  }
  n = n > SIZE_LIMIT ? n - SIZE_LIMIT : n;
  // Init
  queue_uint_init(g_buff_mpmc, 64*cpu_count);
  queue_ull_init (g_final_mpmc, 64*cpu_count);

  // Create thread
  m_thread_t idx_p[prod_count];
  m_thread_t idx_c[conso_count];
  m_thread_t idx_final;
  for(int i = 0; i < prod_count; i++) {
    m_thread_create (idx_p[i], prod_mpmc, &n);
  }
  for(int i = 0; i < conso_count; i++) {
    m_thread_create (idx_c[i], conso_mpmc, &n);
  }
  size_t n2 = conso_count;
  m_thread_create(idx_final, final_mpmc, &n2);

  // Wait for jobs to be done.
  for(int i = 0; i < prod_count; i++) {
    m_thread_join(idx_p[i]);
  }
  for(int i = 0; i < conso_count; i++) {
    m_thread_join(idx_c[i]);
  }
  m_thread_join(idx_final);

  // Clear & quit
  queue_ull_clear(g_final_mpmc);
  queue_uint_clear(g_buff_mpmc);
}

/********************************************************************************************/

QUEUE_SPSC_DEF(queue_single_uint, unsigned int, BUFFER_QUEUE)
queue_single_uint_t g_buff_spsc;

QUEUE_SPSC_DEF(queue_single_ull, unsigned long long, BUFFER_QUEUE)
queue_single_ull_t g_final_spsc;

static void final_spsc(void *arg)
{
  size_t *p_n = arg;
  size_t    n = *p_n;
  unsigned long long j, s = 0;
  for(int i = 0; i < n;i++) {
    while (!queue_single_ull_pop(&j, g_final_spsc)) m_thread_yield();
    s += j;
  }
  g_result = s;
}

static void conso_spsc(void *arg)
{
  unsigned int j;
  size_t *p_n = arg;
  size_t n = *p_n;
  unsigned long long s = 0;
  for(int i = 0; i < n;i++) {
    while (!queue_single_uint_pop(&j, g_buff_spsc)) m_thread_yield();
    s += j;
  }
  while (!queue_single_ull_push(g_final_spsc, s));
}

static void prod_spsc(void *arg)
{
  size_t *p_n = arg;
  size_t n = *p_n;
  size_t r = n;
  for(unsigned int i = 0; i < n;i++) {
    while (!queue_single_uint_push(g_buff_spsc, r )) m_thread_yield();
    r = r * 31421U + 6927U;
  }
}

static void test_queue_single(size_t n)
{
  const int cpu_count   = 2;
  const int prod_count  = cpu_count/2;
  const int conso_count = cpu_count - prod_count;
  // Init
  queue_single_uint_init(g_buff_spsc, 64*cpu_count);
  queue_single_ull_init (g_final_spsc, 64*cpu_count);

  // Create thread
  m_thread_t idx_p[prod_count];
  m_thread_t idx_c[conso_count];
  m_thread_t idx_final;
  for(int i = 0; i < prod_count; i++) {
    m_thread_create (idx_p[i], prod_spsc, &n);
  }
  for(int i = 0; i < conso_count; i++) {
    m_thread_create (idx_c[i], conso_spsc, &n);
  }
  size_t n2 = conso_count;
  m_thread_create(idx_final, final_spsc, &n2);

  // Wait for jobs to be done.
  for(int i = 0; i < prod_count; i++) {
    m_thread_join(idx_p[i]);
  }
  for(int i = 0; i < conso_count; i++) {
    m_thread_join(idx_c[i]);
  }
  m_thread_join(idx_final);

  // Clear & quit
  queue_single_ull_clear(g_final_spsc);
  queue_single_uint_clear(g_buff_spsc);
}

/********************************************************************************************/

#define BULK_SIZE 20

static void final_spsc_bulk(void *arg)
{
  size_t *p_n = arg;
  size_t    n = *p_n;
  unsigned long long j, s = 0;
  for(int i = 0; i < n;i++) {
    while (!queue_single_ull_pop(&j, g_final_spsc)) m_thread_yield();
    s += j;
  }
  g_result = s;
}

static void conso_spsc_bulk(void *arg)
{
  size_t *p_n = arg;
  size_t n = *p_n;
  unsigned long long s = 0;
  unsigned tab[BULK_SIZE];
  for(int i = 0; i < n;i+= BULK_SIZE) {
    unsigned k = queue_single_uint_pop_bulk(BULK_SIZE, tab, g_buff_spsc);
    while (k != BULK_SIZE) {
      m_thread_yield();
      k += queue_single_uint_pop_bulk(BULK_SIZE-k, tab+k, g_buff_spsc);
    }
    for(k = 0; k < BULK_SIZE;k++)
      s += tab[k];
  }
  while (!queue_single_ull_push(g_final_spsc, s));
}

static void prod_spsc_bulk(void *arg)
{
  size_t *p_n = arg;
  size_t n = *p_n;
  if ((n % BULK_SIZE) != 0) abort();
  size_t r = n;
  unsigned tab[BULK_SIZE];
  for(unsigned int i = 0; i < n;i+= BULK_SIZE) {
    for(unsigned k = 0; k < BULK_SIZE; k++) {
      tab[k] = r;
      r = r * 31421U + 6927U;
    }
    unsigned k = queue_single_uint_push_bulk(g_buff_spsc, BULK_SIZE, tab);
    while (k != BULK_SIZE) {
      m_thread_yield();
      k += queue_single_uint_push_bulk(g_buff_spsc, BULK_SIZE-k, tab+k);
    }
  }
}

static void test_queue_single_bulk(size_t n)
{
  const int cpu_count   = 2;
  const int prod_count  = cpu_count/2;
  const int conso_count = cpu_count - prod_count;
  // Init
  queue_single_uint_init(g_buff_spsc, 64*cpu_count);
  queue_single_ull_init (g_final_spsc, 64*cpu_count);

  // Create thread
  m_thread_t idx_p[prod_count];
  m_thread_t idx_c[conso_count];
  m_thread_t idx_final;
  for(int i = 0; i < prod_count; i++) {
    m_thread_create (idx_p[i], prod_spsc_bulk, &n);
  }
  for(int i = 0; i < conso_count; i++) {
    m_thread_create (idx_c[i], conso_spsc_bulk, &n);
  }
  size_t n2 = conso_count;
  m_thread_create(idx_final, final_spsc_bulk, &n2);

  // Wait for jobs to be done.
  for(int i = 0; i < prod_count; i++) {
    m_thread_join(idx_p[i]);
  }
  for(int i = 0; i < conso_count; i++) {
    m_thread_join(idx_c[i]);
  }
  m_thread_join(idx_final);

  // Clear & quit
  queue_single_ull_clear(g_final_spsc);
  queue_single_uint_clear(g_buff_spsc);
}

/********************************************************************************************/

CONCURRENT_DEF(cdeque_uint, deque_uint_t, M_OPEXTEND(DEQUE_OPLIST(deque_uint, M_DEFAULT_OPLIST), PUSH(deque_uint_push_front)))

DEQUE_DEF(deque_ull, unsigned long long)
CONCURRENT_DEF(cdeque_ull, deque_ull_t, M_OPEXTEND(DEQUE_OPLIST(deque_ull, M_DEFAULT_OPLIST), PUSH(deque_ull_push_front)))

cdeque_uint_t g_buff_conc;
cdeque_ull_t g_final_conc;

static void final_conc(void *arg)
{
  size_t *p_n = arg;
  size_t    n = *p_n;
  unsigned long long j, s = 0;
  for(int i = 0; i < n;i++) {
    cdeque_ull_pop_blocking(&j, g_final_conc, true);
    s += j;
  }
  g_result = s;
}

static void conso_conc(void *arg)
{
  unsigned int j;
  size_t *p_n = arg;
  size_t n = *p_n;
  unsigned long long s = 0;
  for(int i = 0; i < n;i++) {
    cdeque_uint_pop_blocking(&j, g_buff_conc, true);
    s += j;
  }
  cdeque_ull_push(g_final_conc, s);
}

static void prod_conc(void *arg)
{
  size_t *p_n = arg;
  size_t n = *p_n;
  size_t r = n;
  for(unsigned int i = 0; i < n;i++) {
    cdeque_uint_push(g_buff_conc, r );
    r = r * 31421U + 6927U;
  }
}

static void test_queue_concurrent(size_t n)
{
  const int cpu_count   = n > SIZE_LIMIT ? 2 : get_cpu_count();
  const int prod_count  = cpu_count/2;
  const int conso_count = cpu_count - prod_count;
  if (cpu_count < 2) {
    fprintf(stderr, "WARNING: Can not measure CONCURRENT performance.\n");
    return;
  }
  n = n > SIZE_LIMIT ? n - SIZE_LIMIT : n;
  // Init
  cdeque_uint_init(g_buff_conc);
  cdeque_ull_init (g_final_conc);

  // Create thread
  m_thread_t idx_p[prod_count];
  m_thread_t idx_c[conso_count];
  m_thread_t idx_final;
  for(int i = 0; i < prod_count; i++) {
    m_thread_create (idx_p[i], prod_conc, &n);
  }
  for(int i = 0; i < conso_count; i++) {
    m_thread_create (idx_c[i], conso_conc, &n);
  }
  size_t n2 = conso_count;
  m_thread_create(idx_final, final_conc, &n2);

  // Wait for jobs to be done.
  for(int i = 0; i < prod_count; i++) {
    m_thread_join(idx_p[i]);
  }
  for(int i = 0; i < conso_count; i++) {
    m_thread_join(idx_c[i]);
  }
  m_thread_join(idx_final);

  // Clear & quit
  cdeque_ull_clear(g_final_conc);
  cdeque_uint_clear(g_buff_conc);
}

/********************************************************************************************/

static unsigned long *g_p;

static void test_hash_prepare(size_t n)
{
  g_p = malloc (n * sizeof(unsigned long));
  if (g_p == NULL) abort();
  for(size_t i = 0; i < n ; i++)
    g_p[i] = ((rand_get()*RAND_MAX + rand_get())*RAND_MAX+rand_get())*RAND_MAX + rand_get();
}


static void test_hash_final(void)
{
  free(g_p);
}

static void test_hash(size_t n)
{
  M_HASH_DECL(hash);
  for(size_t i = 0; i < n; i++)
    M_HASH_UP(hash, g_p[i]);
  g_result = M_HASH_FINAL(hash);
}

static void test_core_hash(size_t n)
{
  g_result = m_core_hash(g_p, n*sizeof(unsigned long));
}


/********************************************************************************************/

ARRAY_DEF(vector_string, string_t)

vector_string_t vstring;
vector_string_t vstring2;

static void
bench_vector_string_init(size_t n)
{
    vector_string_init(vstring);
    for(size_t i = 0; i < n; i++)
      {
        M_LET( (s, "%zu", i), string_t) {
            vector_string_push_back(vstring, s);
        }
    }
    vector_string_init(vstring2);
}

static void
bench_vector_string_init_big(size_t n)
{
    vector_string_init(vstring);
    for(size_t i = 0; i < n; i++)
    {
        M_LET( (s, "%zu%zu%zu%zu%zu%zu", i, i, i, i, i, i), string_t) {
            vector_string_push_back(vstring, s);
        }
    }
    vector_string_init(vstring2);
}

static void
bench_vector_string_bin_run(size_t n)
{
    if (vector_string_size(vstring) != n) {
      fprintf(stderr, "ERROR: Size are different (%zu vs %zu)!\n", n, vector_string_size(vstring));
      abort();
    }

    FILE *f = fopen("tmp-serial.dat", "wb");
    if (!f) abort();
    M_LET( (serial, f), m_serial_bin_write_t) {
        vector_string_out_serial(serial, vstring);
    }
    fclose(f);
    
    f = fopen("tmp-serial.dat", "rb");
    if (!f) abort();
    M_LET( (serial, f), m_serial_bin_read_t) {
        vector_string_in_serial(vstring2, serial);
    }
    fclose(f);

    if (vector_string_size(vstring2) != n) {
      fprintf(stderr, "ERROR: Size are different (%zu vs %zu)!\n", n, vector_string_size(vstring2));
      abort();
    }
}

static void
bench_vector_string_json_run(size_t n)
{
    if (vector_string_size(vstring) != n) {
      fprintf(stderr, "ERROR: Size are different (%zu vs %zu)!\n", n, vector_string_size(vstring));
      abort();
    }

    FILE *f = fopen("tmp-serial.json", "wt");
    if (!f) abort();
    M_LET( (serial, f), m_serial_json_write_t) {
        vector_string_out_serial(serial, vstring);
    }
    fclose(f);
    
    f = fopen("tmp-serial.json", "rt");
    if (!f) abort();
    M_LET( (serial, f), m_serial_json_read_t) {
        vector_string_in_serial(vstring2, serial);
    }
    fclose(f);

    if (vector_string_size(vstring2) != n) {
      fprintf(stderr, "ERROR: Size are different (%zu vs %zu)!\n", n, vector_string_size(vstring2));
      abort();
    }
}

static void
bench_vector_string_clear(void)
{
    if (!vector_string_equal_p(vstring, vstring2)) {
      fprintf(stderr, "ERROR: Array are different after deserialization!\n");
      abort();
    }
    vector_string_clear(vstring);
    vector_string_clear(vstring2);
}

/********************************************************************************************/

ARRAY_DEF(vector_ulong, unsigned long)

vector_ulong_t vulong;
vector_ulong_t vulong2;

static void
bench_vector_ulong_init(size_t n)
{
    vector_ulong_init(vulong);
    for(size_t i = 0; i < n; i++)
    {
      vector_ulong_push_back(vulong, i*i);
    }
    vector_ulong_init(vulong2);
}

static void
bench_vector_ulong_bin_run(size_t n)
{
    if (vector_ulong_size(vulong) != n) {
      fprintf(stderr, "ERROR: Size are different!\n");
      abort();
    }

    FILE *f = fopen("tmp-serial.dat", "wb");
    if (!f) abort();
    M_LET( (serial, f), m_serial_bin_write_t) {
        vector_ulong_out_serial(serial, vulong);
    }
    fclose(f);
    
    f = fopen("tmp-serial.dat", "rb");
    if (!f) abort();
    M_LET( (serial, f), m_serial_bin_read_t) {
        vector_ulong_in_serial(vulong2, serial);
    }
    fclose(f);

    if (vector_ulong_size(vulong2) != n) {
      fprintf(stderr, "ERROR: Size are different!\n");
      abort();
    }
}

static void
bench_vector_ulong_json_run(size_t n)
{
    if (vector_ulong_size(vulong) != n) {
      fprintf(stderr, "ERROR: Size are different!\n");
      abort();
    }

    FILE *f = fopen("tmp-serial.json", "wt");
    if (!f) abort();
    M_LET( (serial, f), m_serial_json_write_t) {
        vector_ulong_out_serial(serial, vulong);
    }
    fclose(f);
    
    f = fopen("tmp-serial.json", "rt");
    if (!f) abort();
    M_LET( (serial, f), m_serial_json_read_t) {
        vector_ulong_in_serial(vulong2, serial);
    }
    fclose(f);

    if (vector_ulong_size(vulong2) != n) {
      fprintf(stderr, "ERROR: Size are different!\n");
      abort();
    }
}

static void
bench_vector_ulong_clear(void)
{
    if (!vector_ulong_equal_p(vulong, vulong2)) {
      fprintf(stderr, "ERROR: Array are different after deserialization!\n");
      abort();
    }
    vector_ulong_clear(vulong);
    vector_ulong_clear(vulong2);
}

/********************************************************************************************/

const config_func_t table[] = {
  { 10,    "List", 10000000, 0, test_list, 0},
  { 11,  "DPList", 10000000, 0, test_dlist, 0},
  { 20,   "Array", 100000000, 0, test_array, 0},
  { 21,   "Deque", 100000000, 0, test_deque, 0},
  { 30,  "Rbtree", 1000000, 0, test_rbtree, 0},
  { 31,  "B+tree", 1000000, 0, test_bptree, 0},
  { 40,    "dict", 1000000, 0, test_dict, 0},
  { 41, "dictBig", 1000000, 0, test_dict_big, 0},
  { 42,"dict(OA)", 1000000, 0, test_dict_oa, 0},
  { 43,"DictStr", 1000000, 0, test_dict_str, 0},
  { 46, "dictLinear(OA)", 1000000, 0, test_dict_oa_linear, 0},
  { 47,    "dictBig(OA)", 1000000, 0, test_dict_oa_big, 0},
  { 50,           "Sort",10000000, 0, test_sort, 0},
  { 51,    "Stable Sort",10000000, 0, test_stable_sort, 0},
  { 60,"Buffer",  1000000, 0, test_buffer, 0},
  { 61,"Queue MPMC",  1000000, 0, test_queue, 0},
  { 62,"Buffer(P=2)",  SIZE_LIMIT+1000000, 0, test_buffer, 0},
  { 63,"Queue MPMC (P=2)",  SIZE_LIMIT+1000000, 0, test_queue, 0},
  { 64,"Queue SPSC (P=2)",  1000000, 0, test_queue_single, 0},
  { 65,"Queue Concurrent",  1000000, 0, test_queue_concurrent, 0},
  { 66,"Queue SPSC(Bulk)",  1000000, 0, test_queue_single_bulk, 0},
  { 70,"M_HASH",  100000000, test_hash_prepare, test_hash, test_hash_final},
  { 71,"Core Hash", 100000000, test_hash_prepare, test_core_hash, test_hash_final},
  {100,    "serial-bin STR", 10000000, bench_vector_string_init, bench_vector_string_bin_run, bench_vector_string_clear},
  {101,    "serial-bin STR.big", 10000000, bench_vector_string_init_big, bench_vector_string_bin_run, bench_vector_string_clear},
  {102,    "serial-bin INT", 10000000, bench_vector_ulong_init, bench_vector_ulong_bin_run, bench_vector_ulong_clear},
  {110,    "serial-json STR", 10000000, bench_vector_string_init, bench_vector_string_json_run, bench_vector_string_clear},
  {111,    "serial-json STR.big", 10000000, bench_vector_string_init_big, bench_vector_string_json_run, bench_vector_string_clear},
  {112,    "serial-json INT", 10000000, bench_vector_ulong_init, bench_vector_ulong_json_run, bench_vector_ulong_clear}
};

int main(int argc, const char *argv[])
{
  test("M*LIB", numberof(table), table, argc, argv);
  exit(0);
}
