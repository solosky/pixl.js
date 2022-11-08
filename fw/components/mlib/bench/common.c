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

#include <string.h>
#include "common.h"

// Maximum supported value for repeat argument
#define MAX_REPEAT 10000

// The global result
unsigned long g_result;

// The rand value
#if defined(MULTI_THREAD_MEASURE)
M_THREAD_ATTR
#endif
unsigned int randValue = 0;

void compiler_barrier(void *p)
{
  // Do nothing, but let the compiler thinks that we did.
  (void) p;
  // Can be used for FLTO building
  //  asm volatile("": "+r" (p) : :"memory");
}

struct parse_opt_s {
  int test_function;
  double from, to, step, grow;
  double within;
  unsigned repeat;
  bool graph, best, best_within, average, quiet;
};

static void
parse_config(struct parse_opt_s *opt, int argc, const char *argv[])
{
  char *end;
  
  opt->test_function = 0;
  opt->from = 0.0;
  opt->to   = 0.0;
  opt->step = 0.0;
  opt->grow = 1.1;
  opt->graph = false;
  opt->best = false;
  opt->best_within = false;
  opt->average = false;
  opt->quiet = false;
  opt->repeat = 1;
  opt->within = .05;
  
  for(int i = 1; i < argc ; i++)
    {
      if (argv[i][0] == '-') {
	if (strcmp(argv[i], "--from") == 0){
	  i++;
	  opt->from = strtod(argv[i], &end);
	  opt->to   = opt->from;
	} else if (strcmp(argv[i], "--to") == 0) {
	  i++;
	  opt->to = strtod(argv[i], &end);
	  opt->grow = 1.1;
	} else if (strcmp(argv[i], "--step") == 0) {
	  i++;
	  opt->step = strtod(argv[i], &end);
	  opt->grow = 0;
	} else if (strcmp(argv[i], "--grow") == 0) {
	  i++;
	  opt->grow = strtod(argv[i], &end);
	  opt->step = 0;
	} else if (strcmp(argv[i], "--repeat") == 0) {
	  i++;
	  opt->repeat = strtol(argv[i], &end, 10);
	  opt->repeat = (opt->repeat > MAX_REPEAT) ? MAX_REPEAT : opt->repeat;
	} else if (strcmp(argv[i], "--graph") == 0) {
	  opt->graph = true;
	} else if (strcmp(argv[i], "--best") == 0) {
	  opt->best = true;
	  opt->average = false;
	  opt->best_within = false;
	} else if (strcmp(argv[i], "--best-within") == 0) {
	  opt->best_within = true;
	  opt->best = false;
	  opt->average = false;
	  i++;
	  opt->within = strtod(argv[i], &end) / 100.0;
	} else if (strcmp(argv[i], "--average") == 0) {
	  opt->average = true;
	  opt->best = false;
	  opt->best_within = false;
	} else if (strcmp(argv[i], "--quiet") == 0) {
	  opt->quiet = true;
	} else {
	  fprintf(stderr, "ERROR: Option unkown: %s.\n",
		  argv[i]);
	  exit(-1);
	}
	if (*end != 0) {
	  fprintf(stderr, "ERROR: Cannot parse %s. Expected float\n",
		  argv[i]);
	  exit(-1);
	}

      } else {
	opt->test_function = strtol (argv[i], &end, 10);
	if (*end != 0) {
	  fprintf(stderr, "ERROR: Cannot parse %s. Expected function number\n",
		  argv[i]);
	  exit(-1);
	}
      }
    }
}

static int
select_config(int func, size_t n, const config_func_t functions[])
{
  for(size_t i = 0; i < n;i++) {
    if (functions[i].num == func)
      return i;
  }
  fprintf(stderr, "ERROR: Function number %d not found. Available functions are:\n", func);
  for(size_t i = 0; i < n;i++) {
    fprintf(stderr, "  %d: %s\n", functions[i].num, functions[i].funcname);
  }
  fprintf(stderr,
	  "USAGE: FUNC_NUMBER [--from number --to number (--grow number | --step number)] [--graph]\n"
	  "[--repeat number] [--best|--average|--best-within] [--quiet]\n");
  exit(-1);
}

// Utility function for qsort
static int cmp_double(const void *pa, const void *pb)
{
  const double *a = (const double *) pa, *b = (const double *)pb;
  return (*a < *b) ? -1 : *a > *b;
}

/* Get the longest sequence of values within 'f'% */
static int
get_sequence(int i, int n, double tab[], double f)
{
  int j;
  for(j = i + 1; j < n; j++) {
    if (tab[j] > f * tab[i])
      break;
  }
  return j-i;
}

/* Return the minimum of the longest sequence of values with 'f'% */
static double
get_best_within(int n, double tab[], double f)
{
  qsort(tab, n, sizeof(double), cmp_double);
  int best_i   = 0;
  int best_seq = get_sequence(0, n, tab, f);
  for(int i = 1; i < n && (n-i) > best_seq ; i++) {
    int seq = get_sequence(i, n, tab, f);
    if (seq > best_seq) {
      best_i = i;
      best_seq = seq;
    }
  }
  return tab[best_i];
}

// Integer sqrt function (to avoid depending on libm)
static unsigned integer_sqrt(unsigned n)
{
  // Find greatest shift.
  int shift = 2;
  unsigned nShifted = n >> shift;
  // We check for nShifted being n, since some implementations
  // perform shift operations modulo the word size.
  while (nShifted != 0 && nShifted != n){
    shift += 2;
    nShifted = n >> shift;
  }
  shift -= 2;
  // Find digits of result.
  unsigned result = 0;
  while (shift >= 0) {
    result = result << 1;
    unsigned candidateResult = result + 1;
    if (candidateResult*candidateResult <= n >> shift) {
      result = candidateResult;
    }
    shift -= 2;
  }
  return result;
}

void
test(const char library[], size_t n, const config_func_t functions[], int argc, const char *argv[])
{
  // Init & parse arguments
  // NUMBER
  // --from NUMBER --to NUMBER --step NUMBER --grow NUMBER
  // --graph --best --average --quiet
  // --repeat N
  struct parse_opt_s arg;
  parse_config(&arg, argc, argv);
  int i = select_config(arg.test_function, n, functions);
  double from = (arg.from == 0) ? functions[i].default_n : arg.from;
  double to   = (arg.to   == 0) ? functions[i].default_n : arg.to;

  // Open plot file if needed
  FILE *graph_file = NULL;
  if (arg.graph) {
    char filename[100];
    sprintf(filename, "plot-%s-%d.dat", library, arg.test_function);
    graph_file = fopen(filename, "wt");
    if (!graph_file) {
      fprintf(stderr, "ERROR: Cannot create plot.dat\n");
      exit(-2);
    }
    fprintf(graph_file, "# plotting %s-%d : %s\n# N T", library, arg.test_function, functions[i].funcname);
  }
  
  // Do the bench
  for(double n = from; n <= to ; n = ((arg.grow == 0) ? n + arg.step : n * arg.grow))
    {
      double best = (1.0/0.0);
      double avg  = 0.0;
      double variance = 0.0;
      static double measure[MAX_REPEAT];
      double ba = 0.0;
      
      if (functions[i].init != NULL)
	functions[i].init(n);

      // Measure the time of the test_function
      for(unsigned r = 0; r < arg.repeat; r++) {
	double t0 = test_function( (arg.graph|arg.best|arg.average|arg.quiet) ? NULL : functions[i].funcname, (size_t) n, functions[i].func);
	best = (t0 < best) ? t0 : best;
	avg += t0;
	variance += t0 * t0;
	measure[r] = t0;
      }

      // Closure of the tests
      if (functions[i].clear != NULL)
	functions[i].clear();

      // Finish computing the different times
      if (arg.repeat > 1) {
	avg /= arg.repeat;
	variance = (variance - arg.repeat * avg * avg) / (arg.repeat - 1);
	ba = get_best_within(arg.repeat, measure, 1. + arg.within);
      }
      
      // The average value shall be around best_within (and greater)
      // otherwise the result doesn't seem reliable.
      bool reliable = (ba <= avg && ba * (1.0+arg.within/2) + 0.5 > avg);
      
      // Print the result
      if (arg.graph) {
	fprintf(graph_file, "%f %f\n", n, arg.average ? avg : arg.best ? best : ba);
      } else if (arg.quiet) {
	printf("%ld\n", (long)(arg.average ? avg : arg.best ? best : reliable ? ba : -1));
      } else if (arg.repeat > 1) {
	if (arg.average == false && arg.best_within == false)
	  printf ("%20.20s time %lu ms for n = %lu ***   BEST  ***\n", functions[i].funcname, (unsigned long) best, (unsigned long) n);
	if (arg.best == false  && arg.best_within == false)
	  printf ("%20.20s time %lu ms +/- %u ms for n = %lu *** AVERAGE ***\n", functions[i].funcname, (unsigned long) avg, 2*integer_sqrt(variance), (unsigned long) n);
	if (arg.best == false  && arg.average == false)
	  printf ("%20.20s time %lu ms for n = %lu *** BEST within %d%% *** %s\n", functions[i].funcname, (unsigned long) ba, (unsigned long) n, (int)(100*arg.within), reliable ? "" : "(unreliable result)");
      }
    }
  
  // Close the FILE if needed
  if (arg.graph) {
    fclose(graph_file);
    printf("File plot-%s-%d.dat generated.\n"
	   "Run in gnuplot the following command:\n"
	   "\tplot 'plot-%s-%d.dat' with linespoints linestyle 1\n",
	   library, arg.test_function, library, arg.test_function);
  }
  return;
}
