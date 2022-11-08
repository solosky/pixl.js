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
#include <stdio.h>
#include <assert.h>
#include "m-worker.h"

/* Compute Fibonacci number using thread systems. */
static worker_t w_g;
static int fib(int n);
struct fib2_s {
  int x, n;
};
atomic_bool resetFunc_called = ATOMIC_VAR_INIT(false);
static void resetFunc(void)
{
  atomic_store(&resetFunc_called, true);
}

static void subfunc_1 (void *data) {
  struct fib2_s *f = M_ASSIGN_CAST (struct fib2_s *, data);
  f->x = fib (f->n );
}
static int fib(int n)
{
  if (n < 2)
    return n;

  struct fib2_s f;
  worker_sync_t b;

  worker_start(b, w_g);
  f.n = n - 2;
  worker_spawn (b, subfunc_1, &f);
  int y = fib (n-1);
  worker_sync(b);
  return f.x + y;
}

static void test1(void)
{
  atomic_store(&resetFunc_called, false);
  worker_init(w_g, 0, 0, resetFunc);
  int result = fib(39);
  assert (result == 63245986);
  worker_clear(w_g);
  assert (atomic_load(&resetFunc_called) == true || m_work3r_get_cpu_count() == 1);
}

static void test1bis(void)
{
  atomic_store(&resetFunc_called, false);
  worker_init(w_g, 0, 0, NULL, resetFunc);
  int result = fib(4);
  assert (result == 3);
  worker_clear(w_g);
  assert (atomic_load(&resetFunc_called) == true || m_work3r_get_cpu_count() == 1);
}

#if defined(__GNUC__) && (!defined(__clang__) || (defined(WORKER_USE_CLANG_BLOCK) && WORKER_USE_CLANG_BLOCK) || (defined(WORKER_USE_CPP_FUNCTION) && WORKER_USE_CPP_FUNCTION))

/* The macro version will generate warnings about shadow variables.
   There is no way to avoid this. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"

static int fib2(int n)
{
  if (n < 2)
    return n;

  worker_sync_t b;
  int y1, y2;
  worker_start(b, w_g);
  WORKER_SPAWN(b, (n), { y2 = fib2(n-2); }, (y2));
  y1 = fib (n-1);
  worker_sync(b);
  return y1 + y2;
}

#pragma GCC diagnostic pop

static void test2(void)
{
  worker_init(w_g, 0, 0, NULL);
  int result = fib2(39);
  assert (result == 63245986);
  worker_clear(w_g);
}
#else
static void test2(void) {}
#endif

int main(void)
{
  test1();
  test1bis();
  test2();
  exit(0);
}
