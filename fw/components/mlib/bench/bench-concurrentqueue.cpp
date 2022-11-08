#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "concurrentqueue.h"

#include "common.h"

/********************************************************************************************/
moodycamel::ConcurrentQueue<unsigned int> g_buff;
moodycamel::ConcurrentQueue<unsigned long long> g_final;

static void final(void *arg)
{
  size_t *p_n = (size_t*) arg;
  size_t    n = *p_n;
  unsigned long long j, s = 0;
  for(unsigned int i = 0; i < n;i++) {
    while (!g_final.try_dequeue(j));
    s += j;
  }
  g_result = s;
}

static void conso(void *arg)
{
  unsigned int j;
  size_t *p_n = (size_t*)arg;
  size_t n = *p_n;
  unsigned long long s = 0;
  for(unsigned int i = 0; i < n;i++) {
    while (!g_buff.try_dequeue(j));
    s += j;
  }
  while (!g_final.try_enqueue(s));
}

static void prod(void *arg)
{
  size_t *p_n = (size_t*)arg;
  size_t n = *p_n;
  size_t r = n;
  for(unsigned int i = 0; i < n;i++) {
    while (!g_buff.try_enqueue(r));
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

}


/********************************************************************************************/

const config_func_t table[] = {
  { 60,    "Queue MPMC", 1000000, 0, test_queue, 0}
};

int main(int argc, const char *argv[])
{
  test("CONCURRENT-QUEUE", numberof(table), table, argc, argv);
  exit(0);
}
