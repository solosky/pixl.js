#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "liblfds711.h"

#include "common.h"

/********************************************************************************************/

struct lfds711_queue_bmm_state g_buff;
struct lfds711_queue_bmm_state g_final;

static void final(void *arg)
{
  size_t *p_n = arg;
  size_t    n = *p_n;
  unsigned long long j, s = 0;
  LFDS711_MISC_MAKE_VALID_ON_CURRENT_LOGICAL_CORE_INITS_COMPLETED_BEFORE_NOW_ON_ANY_OTHER_LOGICAL_CORE;
  for(int i = 0; i < n;i++) {
    void *p;
    while (0 == lfds711_queue_bmm_dequeue(&g_final, &p, NULL)) {  m_thread_yield(); }
    j = (uintptr_t)p;
    s += j;
  }
  g_result = s;
}

static void conso(void *arg)
{
  unsigned int j;
  size_t *p_n = arg;
  size_t n = *p_n;
  unsigned long long s = 0;
  LFDS711_MISC_MAKE_VALID_ON_CURRENT_LOGICAL_CORE_INITS_COMPLETED_BEFORE_NOW_ON_ANY_OTHER_LOGICAL_CORE;
  for(int i = 0; i < n;i++) {
    void *p;
    while (0 == lfds711_queue_bmm_dequeue(&g_buff, &p, NULL)) {  m_thread_yield(); }
    j = (uintptr_t) p;
    s += j;
  }
  while (0 == lfds711_queue_bmm_enqueue(&g_final, (void*)(uintptr_t)s, NULL)) {  m_thread_yield(); }
}

static void prod(void *arg)
{
  size_t *p_n = arg;
  size_t n = *p_n;
  size_t r = n;
  LFDS711_MISC_MAKE_VALID_ON_CURRENT_LOGICAL_CORE_INITS_COMPLETED_BEFORE_NOW_ON_ANY_OTHER_LOGICAL_CORE;
  for(unsigned int i = 0; i < n;i++) {
    while (0 == lfds711_queue_bmm_enqueue(&g_buff, (void*)(uintptr_t)r, NULL)) {  m_thread_yield(); }
    r = r * 31421U + 6927U;
  }
}

static void test_queue(size_t n)
{
  const int cpu_count   = get_cpu_count();
  const int prod_count  = cpu_count/2;
  const int conso_count = cpu_count - prod_count;
  if (cpu_count < 2) {
    fprintf(stderr, "WARNING: Can not measure Buffer performance.\n");
    return;
  }
  rand_get();
  // Init
  const size_t size = m_core_roundpow2(64*cpu_count);
  struct lfds711_queue_bmm_element g_buff_elements[size];
  struct lfds711_queue_bmm_element g_final_elements[size];
  lfds711_queue_bmm_init_valid_on_current_logical_core(&g_buff, g_buff_elements,
						       size, NULL);
  lfds711_queue_bmm_init_valid_on_current_logical_core(&g_final, g_final_elements,
						       size, NULL);
  
  // Create thread
  m_thread_t idx_p[prod_count];
  m_thread_t idx_c[conso_count];
  m_thread_t idx_final;
  for(int i = 0; i < prod_count; i++) {
    m_thread_create (idx_p[i], prod, &n);
  }
  for(int i = 0; i < conso_count; i++) {
    m_thread_create (idx_c[i], conso, &n);
  }
  size_t n2 = conso_count;
  m_thread_create(idx_final, final, &n2);

  // Wait for jobs to be done.
  for(int i = 0; i < prod_count; i++) {
    m_thread_join(idx_p[i]);
  }
  for(int i = 0; i < conso_count; i++) {
    m_thread_join(idx_c[i]);
  }
  m_thread_join(idx_final);

  // Clear & quit
  lfds711_queue_bmm_cleanup(&g_final, NULL);
  lfds711_queue_bmm_cleanup(&g_buff, NULL);
}


/********************************************************************************************/

const config_func_t table[] = {
  { 60,    "Queue MPMC", 1000000, 0, test_queue, 0}
};

int main(int argc, const char *argv[])
{
  test("LIBLFDS", numberof(table), table, argc, argv);
  exit(0);
}
