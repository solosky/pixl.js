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
#include "test-obj.h"

#include "m-snapshot.h"
#include "m-mutex.h"

#include "coverage.h"

// Long Structure with a simple CRC to check for proper passing
typedef struct {
  unsigned int n;
  unsigned long long spare1;
  unsigned int c;
  unsigned long long spare2;
  unsigned int p;
  void *v;
} data_t;

static void data_crc(data_t *p) { p->p = 0-p->n; p->c = ~p->n; }
static void data_init(data_t *p) { p->n = 0; p->spare1 = 0; p->spare2 = 0; data_crc(p); p->v = calloc(1, 1); assert(p->v); }
static void data_valid_p(const data_t *p) { assert (p->n == (0-p->p) && p->n == ~p->c); }
static void data_clear(data_t *p) { free(p->v); }
static void data_set(data_t *p, data_t o) { p->n = o.n; data_crc(p); }
static void data_init_set(data_t *p, data_t o) { data_init(p); data_set(p, o); }

#define DATA_OPLIST \
  (INIT(data_init M_IPTR), INIT_SET(data_init_set M_IPTR), SET(data_set M_IPTR), CLEAR(data_clear M_IPTR))

START_COVERAGE
SNAPSHOT_SPSC_DEF(snapshot_uint, unsigned int)
SNAPSHOT_SPSC_DEF(snapshot_mpz, testobj_t, TESTOBJ_OPLIST)
SNAPSHOT_SPMC_DEF(snapshot_mrsw_data, data_t, DATA_OPLIST)
SNAPSHOT_MPMC_DEF(snapshot_mrmw_data, data_t, DATA_OPLIST)
END_COVERAGE

SNAPSHOT_SPSC_DEF(snapshot_data, data_t, DATA_OPLIST )

SNAPSHOT_SPSC_DEF_AS(SnapshotDouble, SnapshotDouble, double)
SNAPSHOT_SPMC_DEF_AS(SnapshotDoubleSPMC, SnapshotDoubleSPMC, double)
SNAPSHOT_MPMC_DEF_AS(SnapshotDoubleMPMC, SnapshotDoubleMPMC, double)

static void test_uint(void)
{
  unsigned int *p_w;
  const unsigned int *p_r;
  snapshot_uint_t t;
  snapshot_uint_init(t);

  p_r = snapshot_uint_read(t);
  assert (p_r == snapshot_uint_get_read_buffer(t));
  assert(*p_r == 0);
  assert(snapshot_uint_read(t) == p_r);
  
  p_w = snapshot_uint_write(t);
  assert (snapshot_uint_updated_p (t) == true);
  assert (p_w != p_r);
  *p_w = 1;
  assert (*p_r == 0);
  assert (snapshot_uint_read(t) != p_r);
  assert (snapshot_uint_updated_p (t) == false);
  assert (*snapshot_uint_read(t) == 0);
  p_r = snapshot_uint_read(t);
  assert (*p_r == 0);
  
  p_w = snapshot_uint_write(t);
  *p_w = 2;
  assert (*snapshot_uint_read(t) == 1);
  
  p_w = snapshot_uint_write(t);
  assert (p_w == snapshot_uint_get_write_buffer(t));
  *p_w = 3;
  p_w = snapshot_uint_write(t);
  *p_w = 4;
  assert (*snapshot_uint_read(t) == 3);

  snapshot_uint_t t2;
  snapshot_uint_init_set (t2, t);
  snapshot_uint_clear(t);
  assert (*snapshot_uint_read(t) == 3);
  snapshot_uint_init(t);
  snapshot_uint_set (t, t2);
  assert (*snapshot_uint_read(t) == 3);
}

snapshot_data_t g_buff;

static void conso(void *arg)
{
  assert (arg == NULL);
  while (true) {
    const data_t *p = snapshot_data_read(g_buff);
    data_valid_p(p);
    if (p->n == 0)
      return;
  }
}

static void prod(void *arg)
{
  assert (arg == NULL);
  data_t *p = snapshot_data_get_write_buffer(g_buff);
  for(unsigned int i = 1; i < 100000;i++) {
    p->n = i * i;
    data_crc(p);
    p = snapshot_data_write(g_buff);
  }
  p->n = 0;
  data_crc(p);
  p = snapshot_data_write(g_buff);
  assert (p != NULL);
}

static void test_global(void)
{
  m_thread_t idx[2];

  snapshot_data_init(g_buff);
  data_t *p = snapshot_data_write(g_buff);
  assert (p != NULL);
  p->n = 42;
  data_crc(p);
  p = snapshot_data_write(g_buff);
  assert (p != NULL);

  m_thread_create (idx[0], conso, NULL);
  m_thread_create (idx[1], prod, NULL);

  m_thread_join(idx[0]);
  m_thread_join(idx[1]);

  snapshot_data_clear(g_buff);
}

static void test_mrsw_int1(void)
{
  // Test of internal interface.
  m_snapsh0t_mrsw_ct idx;
  m_snapsh0t_mrsw_init(idx, 1);

  assert (m_snapsh0t_mrsw_get_write_idx(idx) == 1);

  assert (m_snapsh0t_mrsw_write(idx) == 0);
  assert (m_snapsh0t_mrsw_write(idx) == 1);
  assert (m_snapsh0t_mrsw_write(idx) == 0);
  assert (m_snapsh0t_mrsw_write(idx) == 1);

  assert (m_snapsh0t_mrsw_read_start(idx) == 0);
  m_snapsh0t_mrsw_read_end(idx, 0);
  assert (m_snapsh0t_mrsw_read_start(idx) == 0);
  m_snapsh0t_mrsw_read_end(idx, 0);
  
  assert (m_snapsh0t_mrsw_write(idx) == 0);

  assert (m_snapsh0t_mrsw_read_start(idx) == 1);
  assert (m_snapsh0t_mrsw_write(idx) == 2);
  assert (m_snapsh0t_mrsw_write(idx) == 0);
  assert (m_snapsh0t_mrsw_write(idx) == 2);
  m_snapsh0t_mrsw_read_end(idx, 1);
  assert (m_snapsh0t_mrsw_read_start(idx) == 0);
  assert (m_snapsh0t_mrsw_write(idx) == 1);

  m_snapsh0t_mrsw_clear(idx);
}

static void test_mrsw_int2(void)
{
  // Test of internal interface
  m_snapsh0t_mrsw_ct idx;
  m_snapsh0t_mrsw_init(idx, 2);

  assert (m_snapsh0t_mrsw_get_write_idx(idx) == 1);

  assert (m_snapsh0t_mrsw_write(idx) == 0);
  assert (m_snapsh0t_mrsw_write(idx) == 1);
  assert (m_snapsh0t_mrsw_write(idx) == 0);
  assert (m_snapsh0t_mrsw_write(idx) == 1);

  assert (m_snapsh0t_mrsw_read_start(idx) == 0);
  m_snapsh0t_mrsw_read_end(idx, 0);
  assert (m_snapsh0t_mrsw_read_start(idx) == 0);
  m_snapsh0t_mrsw_read_end(idx, 0);
  
  assert (m_snapsh0t_mrsw_write(idx) == 0);

  assert (m_snapsh0t_mrsw_read_start(idx) == 1);
  assert (m_snapsh0t_mrsw_write(idx) == 2);
  assert (m_snapsh0t_mrsw_write(idx) == 0);
  assert (m_snapsh0t_mrsw_write(idx) == 2);
  assert (m_snapsh0t_mrsw_read_start(idx) == 0);
  assert (m_snapsh0t_mrsw_write(idx) == 3);
  m_snapsh0t_mrsw_read_end(idx, 1);
  assert (m_snapsh0t_mrsw_read_start(idx) == 2);
  assert (m_snapsh0t_mrsw_write(idx) == 1);
  m_snapsh0t_mrsw_read_end(idx, 0);

  m_snapsh0t_mrsw_clear(idx);
}

snapshot_mrsw_data_t global_mrsw;

static void conso2(void *arg)
{
  unsigned int idx = (unsigned) (uintptr_t) arg;
  while (true) {
    const data_t *p = snapshot_mrsw_data_read_start(global_mrsw);
    assert (p != NULL);
    unsigned int n = p->n;
    data_valid_p(p);
    if (p->n == 0)
      break;
    if (idx > 0) {
      // Simulate slow readers if idx > 0
      m_thread_sleep(idx * 10);
      data_valid_p(p);
      assert (n == p->n);
    }
    snapshot_mrsw_data_read_end(global_mrsw, p);
  }
}

static void prod2(void *arg)
{
  assert (arg == NULL);
  data_t *p = snapshot_mrsw_data_get_write_buffer(global_mrsw);
  for(unsigned int i = 1; i < 190000;i++) {
    assert (p != NULL);
    p->n = i * i;
    data_crc(p);
    p = snapshot_mrsw_data_write(global_mrsw);
  }
  for(unsigned int i = 190000; i < 200000;i++) {
    assert (p != NULL);
    p->n = i * i;
    data_crc(p);
    // Simulate slow writes
    m_thread_sleep(1);
    m_thread_yield();
    p = snapshot_mrsw_data_write(global_mrsw);
  }
  p->n = 0;
  data_crc(p);
  p = snapshot_mrsw_data_write(global_mrsw);
  assert (p != NULL);
}

#define MAX_WRITER 8
#define MAX_READER 8

static void test_mrsw_global(int reader)
{
  m_thread_t idx_w;
  m_thread_t idx[MAX_READER];

  snapshot_mrsw_data_init(global_mrsw, (size_t) reader);
  data_t *p = snapshot_mrsw_data_write(global_mrsw);
  assert (p != NULL);
  p->n = 42;
  data_crc(p);
  p = snapshot_mrsw_data_write(global_mrsw);
  assert (p != NULL);

  for(int i = 0 ; i < reader; i++)
    m_thread_create (idx[i], conso2, (void*) (uintptr_t) i);
  m_thread_create (idx_w, prod2, NULL);

  m_thread_join(idx_w);
  for(int i = 0 ; i < reader; i++)
    m_thread_join(idx[i]);

  snapshot_mrsw_data_clear(global_mrsw);
}

snapshot_mrmw_data_t global_mrmw;

static void conso3(void *arg)
{
  assert (arg == NULL);
  while (true) {
    const data_t *p = snapshot_mrmw_data_read_start(global_mrmw);
    assert (p != NULL);
    data_valid_p(p);
    if (p->n == 0)
      return;
    snapshot_mrmw_data_read_end(global_mrmw, p);
  }
}

static void prod3(void *arg)
{
  assert (arg == NULL);
  for(unsigned int i = 1; i < 20000000;i++) {
    data_t *p = snapshot_mrmw_data_write_start(global_mrmw);
    assert (p != NULL);
    p->n = i * i;
    data_crc(p);
    snapshot_mrmw_data_write_end(global_mrmw, p);
  }
  data_t *p = snapshot_mrmw_data_write_start(global_mrmw);
  p->n = 0;
  data_crc(p);
  snapshot_mrmw_data_write_end(global_mrmw, p);
}

static void test_mrmw_global(int reader, int writer)
{
  m_thread_t idx_w[MAX_WRITER];
  m_thread_t idx[MAX_READER];

  snapshot_mrmw_data_init(global_mrmw, (size_t) reader, (size_t) writer);
  data_t *p = snapshot_mrmw_data_write_start(global_mrmw);
  assert (p != NULL);
  p->n = 42;
  data_crc(p);
  snapshot_mrmw_data_write_end(global_mrmw, p);

  for(int i = 0 ; i < reader; i++)
    m_thread_create (idx[i], conso3, NULL);
  for(int i = 0 ; i < writer; i++)
    m_thread_create (idx_w[i], prod3, NULL);

  for(int i = 0 ; i < writer; i++)
    m_thread_join(idx_w[i]);
  for(int i = 0 ; i < reader; i++)
    m_thread_join(idx[i]);

  snapshot_mrmw_data_clear(global_mrmw);
}

static void test_double(void)
{
  SnapshotDouble s;
  SnapshotDouble_init(s);
  double *p = SnapshotDouble_get_write_buffer(s);
  *p = 42.0;
  p = SnapshotDouble_write(s);
  const double *q = SnapshotDouble_read(s);
  assert (*q == 42.0);
  SnapshotDouble_clear(s);
}

static void test_doubleSPMC(void)
{
  SnapshotDoubleSPMC s;
  SnapshotDoubleSPMC_init(s, 1);
  double *p = SnapshotDoubleSPMC_get_write_buffer(s);
  *p = 42.0;
  p = SnapshotDoubleSPMC_write(s);
  const double *q = SnapshotDoubleSPMC_read_start(s);
  assert (*q == 42.0);
  SnapshotDoubleSPMC_read_end(s, q);
  SnapshotDoubleSPMC_clear(s);
}

static void test_doubleMPMC(void)
{
  SnapshotDoubleMPMC s;
  SnapshotDoubleMPMC_init(s, 1, 1);
  double *p = SnapshotDoubleMPMC_write_start(s);
  *p = 42.0;
  SnapshotDoubleMPMC_write_end(s, p);
  const double *q = SnapshotDoubleMPMC_read_start(s);
  assert (*q == 42.0);
  SnapshotDoubleMPMC_read_end(s, q);
  SnapshotDoubleMPMC_clear(s);
}

int main(void)
{
  test_uint();
  test_global();
  test_mrsw_int1();
  test_mrsw_int2();
  test_mrsw_global(1);
  test_mrsw_global(2);
  test_mrsw_global(4);
  test_mrsw_global(MAX_READER);
  test_mrmw_global(1, 1);
  test_mrmw_global(MAX_READER/4, MAX_WRITER/4);
  test_double();
  test_doubleSPMC();
  test_doubleMPMC();
  exit(0);
}

