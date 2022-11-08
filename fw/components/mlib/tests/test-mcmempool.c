/*
 * M*LIB - Test for Concurrent memory pool allocator
 *
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

#include "m-c-mempool.h"
#include "m-mutex.h"

C_MEMPOOL_DEF(lf_mempool, int)

m_gc_t gc;

/**********************************************************************/

#define MAX_VALUE 100
#define MAX_THREAD 4

lf_mempool_t g;

typedef struct {
  int step;
  int *ptr[MAX_VALUE];
  int value[MAX_VALUE];
} p_t;

p_t g_tab[MAX_THREAD];

static void do_stuff(m_gc_tid_t id)
{
  // Mix of local and more global allocation
  int *d[MAX_VALUE];
  for(int i = 0; i < MAX_VALUE; i++) {
    d[i] = lf_mempool_new(g, id);
    *d[i] = i*i;
  }
  switch (g_tab[id].step)
    {
    case 0:
      for(int i = 0; i < 50; i++) {
        g_tab[id].value[i] += i;
        g_tab[id].ptr[i] = lf_mempool_new(g, id);
        *g_tab[id].ptr[i] = g_tab[id].value[i];
      }
      g_tab[id].step = 1;
      break;
    case 1:
      for(int i = 50; i < MAX_VALUE; i++) {
        g_tab[id].value[i] += i;
        g_tab[id].ptr[i] = lf_mempool_new(g, id);
        *g_tab[id].ptr[i] = g_tab[id].value[i];
      }
      g_tab[id].step = 2;
      break;
    case 2:
      for(int i = 0; i < 50; i++) {
        assert(g_tab[id].ptr[i] != NULL);
        assert(*g_tab[id].ptr[i] == g_tab[id].value[i] );
        lf_mempool_del(g, g_tab[id].ptr[i], id);
        g_tab[id].ptr[i] = NULL;
      }
      g_tab[id].step = 3;
      break;
    case 3:
      for(int i = 50; i < MAX_VALUE; i++) {
        assert(g_tab[id].ptr[i] != NULL);
        assert(*g_tab[id].ptr[i] == g_tab[id].value[i] );
        lf_mempool_del(g, g_tab[id].ptr[i], id);
        g_tab[id].ptr[i] = NULL;
      }
      g_tab[id].step = 0;
      break;
    default:
      break;
    }
  for(int i = 0; i < MAX_VALUE; i++) {
    assert (*d[i] == i*i);
    lf_mempool_del(g, d[i], id);
  }
}

static void thread1(void *arg)
{
  (void)arg;
  /* Attach this thread to the GC */
  m_gc_tid_t id = m_gc_attach_thread(gc);
  for(int n = 0; n < 100000; n ++) {
    /* Awake the thread: now we can alloc nodes */
    m_gc_awake(gc, id);
    /* Do some stuff */
    do_stuff(id);
    /* Sleep the thread. A Garbage Collect may occur if possible */
    m_gc_sleep(gc, id);
  }
  /* Detach the thread from the GC */
  m_gc_detach_thread(gc, id);
}

static void thread2(void *arg)
{
  (void)arg;
  m_gc_tid_t id = m_gc_attach_thread(gc);
  for(int n = 0; n < 10000; n ++) {
    m_gc_awake(gc, id);
    do_stuff(id);
    m_thread_sleep(1);
    m_gc_sleep(gc, id);
  }
  m_gc_detach_thread(gc, id);
}

static void test(void)
{
  /* First create the Garbage Collector */
  m_gc_init (gc, MAX_THREAD);
  /* Create a mempool and attach it to the Garbage Collector */
  lf_mempool_init(g, gc, 256, MAX_THREAD);

  m_thread_t id[MAX_THREAD];
  for(int i = 0; i < MAX_THREAD; i+=2) {
    m_thread_create(id[i], thread1, NULL);
    m_thread_create(id[i+1], thread2, NULL);
  }
  for(int i = 0; i < MAX_THREAD; i++) {
    m_thread_join(id[i]);
  }
  /* Destroy the mempool */
  lf_mempool_clear(g);
  /* Destroy the Garbage Collector */
  m_gc_clear (gc);
}

static void test2(void)
{
  /* First create the Garbage Collector */
  m_gc_init (gc, MAX_THREAD);
  /* Create a mempool and attach it to the Garbage Collector */
  lf_mempool_init(g, gc, 256, MAX_THREAD);
  /* Create another mempool and attach it to the Garbage Collector */
  lf_mempool_t g2;
  lf_mempool_init(g2, gc, 256, MAX_THREAD);
  /* Register VLA */
  m_vlapool_t vla;
  m_vlapool_init(vla, gc);

  /* Attach the thread */
  m_gc_tid_t id = m_gc_attach_thread(gc);
  /* Awake the thread: now we can alloc nodes */
  m_gc_awake(gc, id);
  
  /* Test some allocation */
  void *ptr = m_vlapool_new(vla, id, sizeof (int));
  *(int*)ptr = 42;
  m_vlapool_del(vla, ptr, id);

  int *pi = lf_mempool_new(g, id);
  *pi = 44;
  lf_mempool_del(g, pi, id);
  
  int *pi2 = lf_mempool_new(g2, id);
  *pi2 = 46;
  lf_mempool_del(g2, pi2, id);

  /* Test that the data remain accessible
     as it has only be logically deleted */
  assert (*(int*)ptr == 42);
  assert (*pi        == 44);
  assert (*pi2       == 46);

  /* Sleep the thread */
  m_gc_sleep(gc, id);

  /* Detach the thread */
  m_gc_detach_thread(gc, id);

  /* Clear everything */
  m_vlapool_clear(vla);
  lf_mempool_clear(g);
  lf_mempool_clear(g2);
  m_gc_clear(gc);
}

int main(void)
{
  test();
  test2();
  exit(0);
}

