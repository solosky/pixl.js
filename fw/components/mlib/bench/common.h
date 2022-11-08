/*
 * M*LIB - common functions for bench module
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
#ifndef __COMMON_BENCH__H
#define __COMMON_BENCH__H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Needed for M_THREAD_ATTR
#if defined(MULTI_THREAD_MEASURE)
#include "m-mutex.h"
#endif

#if defined(_WIN32)
# include <windows.h>
#elif (defined(__APPLE__) && defined(__MACH__)) \
  || defined(__DragonFly__) || defined(__FreeBSD__) \
  || defined(__NetBSD__) || defined(__OpenBSD__)
# include <sys/types.h>
# include <sys/resource.h>
# include <sys/time.h>
# include <sys/param.h>
# include <sys/sysctl.h>
# define USE_SYSCTL
#else
# include <sys/types.h>
# include <sys/resource.h>
# include <sys/time.h>
# include <unistd.h>
#endif

// Number of elements of a table
#define numberof(x) (sizeof(x)/sizeof(x[0]))

/* Get the current CPU time. */
#if defined(_WIN32)
/* WINDOWS variant */
static inline unsigned long long
cputime (void)
{
  LARGE_INTEGER freq, val;
  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&val);
  return (unsigned long long) (1000000. * (double) val.QuadPart / (double) freq.QuadPart);
}
#else
/* UNIX variant */
static inline unsigned long long
cputime (void)
{
#if defined(MULTI_THREAD_MEASURE)
  // Multi thread: Uses real time
  struct timeval tv;
  gettimeofday (&tv, NULL);
  return tv.tv_sec * 1000000ULL + tv.tv_usec;
#else
  // Single thread: Uses process usage time
  struct rusage rus;
  getrusage (0, &rus);
  return rus.ru_utime.tv_sec * 1000000ULL + rus.ru_utime.tv_usec;
#endif
}

#endif

// The result of the bench
extern unsigned long g_result;

// The rand value
extern
#if defined(MULTI_THREAD_MEASURE)
M_THREAD_ATTR
#endif
unsigned int randValue;

// Pseudo rand value generator.
// Not very random but sufficient and fast enough for the bench
static inline void rand_init(void)
{
  randValue = 0;
}
static inline unsigned int rand_get(void)
{
  randValue = randValue * 31421U + 6927U;
  return randValue;
}

// The function that performs the bench
static inline double
test_function(const char *str, size_t n, void (*func)(size_t))
{
  double result;
  unsigned long long start, end;
  rand_init();
  start = cputime();
  (*func)(n);
  end = cputime();
  result = (double)(end-start) / 1000.0;
  end = (end - start) / 1000U;
  if (str != NULL) {
    printf ("%20.20s time %lu ms for n = %lu [r=%lu]\n", str, (unsigned long) end, (unsigned long) n, g_result);
  }
  return result;
}

// Configuration table
typedef struct {
  int num;
  const char *funcname;
  size_t default_n;
  void (*init)(size_t);
  void (*func)(size_t);
  void (*clear)(void);
} config_func_t;

// Perform a parsing of the given arguments and do the bench according to the table
#ifdef __cplusplus
extern "C"
#endif
void test(const char library[], size_t n, const config_func_t functions[], int argc, const char *argv[]);

/* Return the number of CPU of the system */
static inline int get_cpu_count(void)
{
#if defined(_WIN32)
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
#elif defined(USE_SYSCTL)
  int nm[2];
  int count = 0;
  size_t len = sizeof (count);
  nm[0] = CTL_HW;
  nm[1] = HW_NCPU;
  sysctl(nm, 2, &count, &len, NULL, 0);
  return MAX(1, count);
#elif defined (_SC_NPROCESSORS_ONLN)
  return sysconf(_SC_NPROCESSORS_ONLN);
#else
  return 1;
#endif
}

#ifdef __cplusplus
extern "C"
#endif
void compiler_barrier(void *p);

#endif
