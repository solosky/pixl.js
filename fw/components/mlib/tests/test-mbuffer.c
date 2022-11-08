/*
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
#include "m-buffer.h"

#include "test-obj.h"
#include "coverage.h"
START_COVERAGE
// Define a fixed queue of unsigned int
BUFFER_DEF(buffer_uint, unsigned int, 10, BUFFER_QUEUE|BUFFER_BLOCKING)
QUEUE_MPMC_DEF(queue_uint, unsigned int, BUFFER_QUEUE)
QUEUE_SPSC_DEF(squeue_uint, unsigned int, BUFFER_QUEUE)
END_COVERAGE

// Define a variable stack of float
BUFFER_DEF(buffer_floats, float, 0, BUFFER_STACK|BUFFER_BLOCKING)

// Define a fixed stack of char
BUFFER_DEF(buffer_char, char, 10, BUFFER_STACK|BUFFER_UNBLOCKING)

// Define a fixed queue of long long
BUFFER_DEF(buffer_llong, long long, 16, BUFFER_QUEUE|BUFFER_THREAD_UNSAFE|BUFFER_UNBLOCKING)

// Define a buffer of complex structure.
BUFFER_DEF(buffer_mpz, testobj_t, 32, BUFFER_QUEUE, TESTOBJ_OPLIST)
QUEUE_MPMC_DEF(queue_z, testobj_t, BUFFER_QUEUE, TESTOBJ_OPLIST)
QUEUE_SPSC_DEF(squeue_a, testobj_t, BUFFER_QUEUE, TESTOBJ_OPLIST)

// Define other buffers
BUFFER_DEF_AS(BufferDouble1, BufferDouble1, double, 4, BUFFER_QUEUE)
QUEUE_MPMC_DEF_AS(BufferDouble2, BufferDouble2, double, BUFFER_QUEUE)
QUEUE_SPSC_DEF_AS(BufferDouble3, BufferDouble3, double, BUFFER_QUEUE)
#define M_OPL_BufferDouble1() BUFFER_OPLIST(BufferDouble1, M_BASIC_OPLIST)
#define M_OPL_BufferDouble2() BUFFER_OPLIST(BufferDouble2, M_BASIC_OPLIST)
#define M_OPL_BufferDouble3() BUFFER_OPLIST(BufferDouble3, M_BASIC_OPLIST)

// Define buffer with ENUM oplist (with API_1 usage)
typedef enum { OK, KO } state_t;
#define M_OPL_state_t() M_ENUM_OPLIST(state_t, OK)
BUFFER_DEF(array_state, state_t, 0, M_BUFFER_QUEUE)

buffer_uint_t g_buff;
buffer_uint_t g_buffB;

// Number of thread created by the test (twice this amount).
#define MAX_TEST_THREAD  100
#define MAX_TEST_THREAD2 (MAX_TEST_THREAD*2/3)
#define MAX_COUNT 1024

/* Global counter.
   Each thread has its own row of the matrix (exclusive use). */
unsigned int g_count[MAX_TEST_THREAD][MAX_COUNT];

static void conso(void *arg)
{
  unsigned int j;
  uintptr_t thread_id = (uintptr_t) arg;
  assert(thread_id < MAX_TEST_THREAD);
  for(int i = 0; i < MAX_COUNT;i++) {
    buffer_uint_pop(&j, g_buff);
    assert (j < MAX_COUNT);
    // Each thread fill in its own row of the matrix, ensuring separate access.
    // We don't want atomic access which may perturbate the test
    // by adding memory barries that will hide some issues in the code.
    g_count[thread_id][j] ++;
  }
}

static void prod(void *arg)
{
  assert (arg == NULL);
  for(unsigned int i = 0; i < MAX_COUNT;i++)
    buffer_uint_push(g_buff, i);
}

static void test_global(void)
{
  m_thread_t idx_p[MAX_TEST_THREAD];
  m_thread_t idx_c[MAX_TEST_THREAD];

  buffer_uint_init(g_buff, 10);
  assert (buffer_uint_capacity(g_buff) == 10);
  assert (buffer_uint_empty_p(g_buff));
  assert (!buffer_uint_full_p(g_buff));
  assert (buffer_uint_size(g_buff) == 0);

  // Run multiple producer & consummer threads in parallel
  for(int i = 0; i < MAX_TEST_THREAD; i++) {
    m_thread_create (idx_p[i], conso, (void*)(uintptr_t) i);
    m_thread_create (idx_c[i], prod, NULL);
  }
  for(int i = 0; i < MAX_TEST_THREAD;i++) {
    m_thread_join(idx_p[i]);
    m_thread_join(idx_c[i]);
  }

  // Check if the data were correctly transmitted
  // from the producer threads to the consummer threads
  // First, consolidate the data
  for(int i = 1 ; i < MAX_TEST_THREAD; i++) {
    for(int j = 0 ; j < MAX_COUNT; j++) {
      g_count[0][j] += g_count[i][j];
    }
  }
  // Then check then
  for(int j = 0 ; j < MAX_COUNT; j++) {
    assert(g_count[0][j] == MAX_TEST_THREAD);
  }

  assert (buffer_uint_empty_p(g_buff));
  assert (!buffer_uint_full_p(g_buff));
  assert (buffer_uint_size(g_buff) == 0);

  buffer_uint_reset(g_buff);
  assert (buffer_uint_empty_p(g_buff));
  assert (!buffer_uint_full_p(g_buff));
  assert (buffer_uint_size(g_buff) == 0);

  for(unsigned int i = 0; i < 5;i++)
    buffer_uint_push(g_buff, i);

  assert (!buffer_uint_empty_p(g_buff));
  assert (!buffer_uint_full_p(g_buff));
  assert (buffer_uint_size(g_buff) == 5);

  for(unsigned int i = 0; i < 5;i++)
    buffer_uint_push(g_buff, i);

  assert (!buffer_uint_empty_p(g_buff));
  assert (buffer_uint_full_p(g_buff));
  assert (buffer_uint_size(g_buff) == 10);
  assert (buffer_uint_push_blocking(g_buff, 15, false) == false);
  
  buffer_uint_reset(g_buff);
  assert (buffer_uint_empty_p(g_buff));
  assert (!buffer_uint_full_p(g_buff));
  assert (buffer_uint_size(g_buff) == 0);
  unsigned int s;
  assert (buffer_uint_pop_blocking(&s, g_buff, false) == false);

  assert (buffer_uint_overwrite(g_buff) == 0);

  buffer_uint_init_set(g_buffB, g_buff);
  assert (buffer_uint_empty_p(g_buffB));
  buffer_uint_clear(g_buffB);
  for(unsigned int i = 0; i < 5;i++)
    buffer_uint_push(g_buff, i);
  buffer_uint_init_set(g_buffB, g_buff);
  assert (!buffer_uint_empty_p(g_buffB));
  assert (!buffer_uint_full_p(g_buffB));
  assert (buffer_uint_size(g_buffB) == 5);
  for(unsigned int i = 0; i < 5;i++) {
    unsigned int j;
    bool b= buffer_uint_pop(&j, g_buffB);
    assert(b);
    assert(j == i);
  }
  assert (buffer_uint_empty_p(g_buffB));

  buffer_uint_set(g_buffB, g_buff);
  assert (!buffer_uint_empty_p(g_buffB));
  assert (!buffer_uint_full_p(g_buffB));
  assert (buffer_uint_size(g_buffB) == 5);
  for(unsigned int i = 0; i < 5;i++) {
    unsigned int j;
    bool b= buffer_uint_pop(&j, g_buffB);
    assert(b);
    assert(j == i);
  }
  assert (buffer_uint_empty_p(g_buffB));
  buffer_uint_clear(g_buffB);  

  buffer_uint_clear(g_buff);
}

static void test_stack(void)
{
  buffer_floats_t buff;
  buffer_floats_init(buff, 100);
  assert (buffer_floats_capacity(buff) == 100);
  for(unsigned int i = 0; i < 100; i++) {
    float f = (float) i;
    buffer_floats_push(buff, f);
    assert (buffer_floats_empty_p(buff) == false);
  }
  assert (buffer_floats_full_p(buff) == true);
  for(unsigned int i = 0; i < 100; i++) {
    float j;
    buffer_floats_pop(&j, buff);
    assert(j == (float) (99-i) );
    assert (buffer_floats_full_p(buff) == false);
  }
  assert (buffer_floats_empty_p(buff) == true);
  buffer_floats_clear(buff);
}

static void test_stack2(void)
{
  buffer_char_t buff;
  bool b;
  char c;
  buffer_char_init(buff, 10);
  for(int i = 0; i < 10; i++) {
    c = (char) i;
    b = buffer_char_push(buff, c);
    assert (b == true);
    assert (buffer_char_empty_p(buff) == false);
  }
  b = buffer_char_push(buff, c);
  assert (b == false);
  assert (buffer_char_full_p(buff) == true);
  for(int i = 0; i < 10; i++) {
    buffer_char_pop(&c, buff);
    assert(c == 9-i);
    assert (buffer_char_full_p(buff) == false);
  }
  b = buffer_char_pop(&c, buff);
  assert (b == false);
  assert (buffer_char_empty_p(buff) == true);
  buffer_char_clear(buff);
}

static void test_no_thread(void)
{
  buffer_llong_t buff;
  bool b;
  long long c;
  buffer_llong_init(buff, 16);
  for(int i = 0; i < 16; i++) {
    c = i;
    b = buffer_llong_push(buff, c);
    assert (b == true);
    assert (buffer_llong_empty_p(buff) == false);
  }
  b = buffer_llong_push(buff, c);
  assert (b == false);
  assert (buffer_llong_full_p(buff) == true);
  for(int i = 0; i < 16; i++) {
    buffer_llong_pop(&c, buff);
    assert(c == i);
    assert (buffer_llong_full_p(buff) == false);
  }
  b = buffer_llong_pop(&c, buff);
  assert (b == false);
  assert (buffer_llong_empty_p(buff) == true);

  b = buffer_llong_push(buff, c);
  assert (b == true);
  buffer_llong_reset(buff);
  assert (buffer_llong_empty_p(buff) == true);
  
  buffer_llong_clear(buff);
}

/********************************************************************************************/

/* Test intrusive shared pointer + buffer */
#include "m-i-shared.h"

// Tiny test structure
typedef struct test_s {
  ISHARED_PTR_INTERFACE(ishared_itest, struct test_s);
  char buffer[52];
  char bigbuffer[1000000];
} test_t;
static void test_init(test_t *p)  { memset(p->buffer, 0x00, 52); }
static void test_clear(test_t *p) { memset(p->buffer, 0xFF, 52); }
ISHARED_PTR_DEF(ishared_itest, test_t,
                (INIT(test_init M_IPTR), CLEAR(test_clear M_IPTR), DEL(free)))

typedef struct test2_s {
  ISHARED_PTR_INTERFACE(ishared_itest2, struct test_s);
  char buffer[52];
} test2_t;
static inline void ishared_itest2_init_data(test2_t *d)
{
  memset(d->buffer, 0, sizeof (d->buffer));
}
ISHARED_PTR_DEF(ishared_itest2, test2_t, (INIT(API_2(ishared_itest2_init_data)), CLEAR(M_NOTHING_DEFAULT)))

typedef struct test3_s {
  ISHARED_PTR_INTERFACE(ishared_itest3, struct test_s);
  char buffer[52];
} test3_t;
ISHARED_PTR_DEF(ishared_itest3, test3_t, (CLEAR(M_NOTHING_DEFAULT)))

typedef struct test4_s {
  ISHARED_PTR_INTERFACE(ishared_itest4, struct test_s);
  char buffer[52];
} test4_t;
ISHARED_PTR_DEF(ishared_itest4, test4_t, (INIT(0), DEL(0), INIT(M_MEMSET_DEFAULT), CLEAR(M_NOTHING_DEFAULT)))

static test_t *test_new(void)
{
  test_t *p = M_ASSIGN_CAST(test_t*, malloc (sizeof (test_t)));
  assert (p != NULL);
  test_init (p);
  return ishared_itest_init(p);
}

BUFFER_DEF(buffer_itest, test_t *, 16, BUFFER_PUSH_INIT_POP_MOVE,
           ISHARED_PTR_OPLIST(ishared_itest))

static buffer_itest_t comm1;
static buffer_itest_t comm2;

static void test_conso1(void *arg)
{
  test_t *p;
  assert (arg == NULL);
  for(int i = 0; i < 10;i++) {
    buffer_itest_pop(&p, comm1);
    for(int j = 0; j < 52; j++)
      assert (p->buffer[j] == (char) ((j * j * 17) + j * 42 + 1));
    ishared_itest_clear(p);
  }
}

static void test_conso2(void *arg)
{
  test_t *p;
  assert (arg == NULL);
  for(int i = 0; i < 10;i++) {
    buffer_itest_pop(&p, comm2);
    for(int j = 0; j < 52; j++)
      assert (p->buffer[j] == (char) ((j * j * 17) + j * 42 + 1));
    ishared_itest_clear(p);
  }
}

static void test_prod(void *arg)
{
  assert (arg == NULL);
  for(unsigned int i = 0; i < 10;i++) {
    test_t *p = test_new();
    for(int j = 0; j < 52; j++) {
      p->buffer[j] = (char) ((j * j * 17) + j * 42 + 1);
    }
    buffer_itest_push(comm1, p);
    buffer_itest_push(comm2, p);
    ishared_itest_clear(p);
  }
}

static void test_global_ishared(void)
{
  m_thread_t idx_p[MAX_TEST_THREAD2];
  m_thread_t idx_c1[MAX_TEST_THREAD2];
  m_thread_t idx_c2[MAX_TEST_THREAD2];

  buffer_itest_init(comm1, 16);
  buffer_itest_init(comm2, 16);
  
  for(int i = 0; i < MAX_TEST_THREAD2; i++) {
    m_thread_create (idx_c1[i], test_conso1, NULL);
    m_thread_create (idx_c2[i], test_conso2, NULL);
    m_thread_create (idx_p[i], test_prod, NULL);
  }
  for(int i = 0; i < MAX_TEST_THREAD2;i++) {
    m_thread_join(idx_p[i]);
    m_thread_join(idx_c1[i]);
    m_thread_join(idx_c2[i]);
  }

  buffer_itest_clear(comm1);
  buffer_itest_clear(comm2);
}

/********************************************************************************************/

QUEUE_MPMC_DEF(queue_ull, unsigned long long, BUFFER_QUEUE)

queue_uint_t g_buff2;
queue_ull_t g_final2;
unsigned long long g_result;

static void final2(void *arg)
{
  size_t *p_n = M_ASSIGN_CAST(size_t *, arg);
  size_t    n = *p_n;
  unsigned long long j, s = 0;
  for(unsigned int i = 0; i < n;i++) {
    while (!queue_ull_pop(&j, g_final2));
    s += j;
  }
  g_result = s;
}

static void conso2(void *arg)
{
  unsigned int j;
  size_t *p_n = M_ASSIGN_CAST(size_t *, arg);
  size_t n = *p_n;
  unsigned long long s = 0;
  for(unsigned int i = 0; i < n;i++) {
    while (!queue_uint_pop(&j, g_buff2));
    s += j;
  }
  while (!queue_ull_push(g_final2, s));
}

static void prod2(void *arg)
{
  size_t *p_n = M_ASSIGN_CAST(size_t *, arg);
  size_t n = *p_n;
  size_t r = n;
  for(unsigned int i = 0; i < n;i++) {
    while (!queue_uint_push(g_buff2, (unsigned int) r ));
    r = r * 31421U + 6927U;
  }
}

static void test_queue(size_t n, int cpu_count, unsigned long long ref)
{
  cpu_count = M_MIN(cpu_count, 64);
  const int prod_count  = cpu_count / 2;
  const int conso_count = cpu_count - prod_count;

  // Init
  queue_uint_init(g_buff2, 64*2);
  queue_ull_init (g_final2, 64*2);
  assert (queue_uint_capacity(g_buff2) == 64*2);
  assert (queue_ull_capacity(g_final2) == 64*2);

  // Create thread
  m_thread_t idx_p[64];
  m_thread_t idx_c[64];
  m_thread_t idx_final;
  for(int i = 0; i < prod_count; i++) {
    m_thread_create (idx_p[i], prod2, &n);
  }
  for(int i = 0; i < conso_count; i++) {
    m_thread_create (idx_c[i], conso2, &n);
  }
  size_t n2 = (size_t) conso_count;
  m_thread_create(idx_final, final2, &n2);

  // Wait for jobs to be done.
  for(int i = 0; i < prod_count; i++) {
    m_thread_join(idx_p[i]);
  }
  for(int i = 0; i < conso_count; i++) {
    m_thread_join(idx_c[i]);
  }
  m_thread_join(idx_final);

  assert(g_result == ref);
  
  // Clear & quit
  queue_ull_clear(g_final2);
  queue_uint_clear(g_buff2);
}

/********************************************************************************************/

static void test_spsc(void)
{
  squeue_uint_t q;
  bool b;
  unsigned j;
  
  squeue_uint_init(q, 256);

  assert(squeue_uint_empty_p(q));
  
  for(unsigned i = 0; i < 256; i++) {
    b = squeue_uint_push(q, i);
    assert(b);
  }
  assert(!squeue_uint_empty_p(q));
  assert(squeue_uint_full_p(q));
  assert(squeue_uint_size(q) == 256);
  assert(squeue_uint_capacity(q) == 256);

  b = squeue_uint_push(q, 256);
  assert (!b);
  for(unsigned i = 0; i < 256; i++) {
    b = squeue_uint_pop(&j, q);
    assert(b);
    assert(j == i);
  }
  assert(squeue_uint_empty_p(q));
  assert(!squeue_uint_full_p(q));
  assert(squeue_uint_size(q) == 0);
  assert(squeue_uint_capacity(q) == 256);

  for(unsigned i = 0; i < 256; i++) {
    b = squeue_uint_push(q, i);
    assert(b);
  }
  for(unsigned i = 256; i < 256+128; i++) {
    assert(squeue_uint_full_p(q));
    squeue_uint_push_force(q, i);
  }
  for(unsigned i = 128; i < 256+128; i++) {
    b = squeue_uint_pop(&j, q);
    assert(b);
    assert(j == i);
  }

  unsigned tab[16];
  for(unsigned i = 0; i < 16; i++)
    tab[i] = i * i;
  for(unsigned i = 0; i < 15; i++) {
    j = squeue_uint_push_bulk(q, 16, tab);
    assert(j == 16);
  }
  b = squeue_uint_push(q, 1024);
  assert(b);
  j = squeue_uint_push_bulk(q, 16, tab);
  assert(j == 15);
  
  for(unsigned i = 0; i < 16; i++)
    tab[i] = 0;
  j = squeue_uint_pop_bulk(16, tab, q);
  assert(j == 16);
  for(unsigned i = 0; i < 16; i++)
    assert( tab[i] == i * i);

  do {
    b = squeue_uint_pop(&j, q);
  } while (b);
  assert(squeue_uint_empty_p(q));
  unsigned l = 65536;
  b = squeue_uint_push_move(q, &l);
  assert(b);
  b = squeue_uint_pop(&j, q);
  assert(b);
  assert(j == 65536);
  
  squeue_uint_clear(q);
}


/********************************************************************************************/

static void test_double1(void)
{
  bool b;
  M_LET(buffer, BufferDouble1) {
    b = BufferDouble1_push(buffer, 0.0);
    assert(b);
    b= BufferDouble1_push(buffer, 1.0);
    assert(b);
    double d;
    b = BufferDouble1_pop(&d, buffer);
    assert(b);
    assert(d == 0.0);
    b = BufferDouble1_pop(&d, buffer);
    assert(b);
    assert(d == 1.0);
  }
}

static void test_double2(void)
{
  bool b;
  BufferDouble2 buffer;
  BufferDouble2_init(buffer, 2);
  b = BufferDouble2_push(buffer, 0.0);
  assert(b);
  b= BufferDouble2_push(buffer, 1.0);
  assert(b);
  double d;
  b = BufferDouble2_pop(&d, buffer);
  assert(b);
  assert(d == 0.0);
  b = BufferDouble2_pop(&d, buffer);
  assert(b);
  assert(d == 1.0);
  BufferDouble2_clear(buffer);
}

static void test_double3(void)
{
  bool b;
  BufferDouble3 buffer;
  BufferDouble3_init(buffer, 2);
  b = BufferDouble3_push(buffer, 0.0);
  assert(b);
  b= BufferDouble3_push(buffer, 1.0);
  assert(b);
  double d;
  b = BufferDouble3_pop(&d, buffer);
  assert(b);
  assert(d == 0.0);
  b = BufferDouble3_pop(&d, buffer);
  assert(b);
  assert(d == 1.0);
  BufferDouble3_clear(buffer);
}

/********************************************************************************************/

static void test_uint(void)
{
  unsigned int x, y;
  buffer_uint_t v;
  buffer_uint_init(v, 10);

  assert (buffer_uint_empty_p(v) == true);
  assert (buffer_uint_full_p(v) == false);

  x = 10;
  buffer_uint_push (v, x);

  assert (buffer_uint_empty_p(v) == false);
  assert (buffer_uint_full_p(v) == false);

  buffer_uint_pop (&y, v);

  assert (buffer_uint_empty_p(v) == true);
  assert (buffer_uint_full_p(v) == false);
  assert (y == 10);

  buffer_uint_clear(v);
}

int main(void)
{
  test_uint();
  test_global();
  test_stack();
  test_stack2();
  test_no_thread();
  test_global_ishared();
  test_queue(1000000, 2, 2148371710223136ULL);
  test_spsc();
  test_double1();
  test_double2();
  test_double3();
  exit(0);
}
