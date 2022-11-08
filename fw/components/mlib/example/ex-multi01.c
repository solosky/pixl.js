/* Based on:
   https://github.com/kostya/benchmarks/blob/master/brainfuck/brainfuck.cpp */
#include <stdio.h>
#include "m-array.h"
#include "m-string.h"

// Define an array of integer
ARRAY_DEF(array_int, int)

typedef struct {
  int pos;
  array_int_t tape;
} tape_t[1];

static void tape_init(tape_t tape) {
  tape->pos =0;
  array_int_init(tape->tape);
  array_int_push_back(tape->tape, 0);
}
static void tape_clear(tape_t tape) { array_int_clear(tape->tape); }
static int  tape_get(tape_t tape) { return *array_int_cget(tape->tape, tape->pos); }
static void tape_inc(tape_t tape) { (*array_int_get(tape->tape, tape->pos)) ++; }
static void tape_dec(tape_t tape) { (*array_int_get(tape->tape, tape->pos)) --; }
static void tape_advance(tape_t tape) { tape->pos++; if (array_int_size(tape->tape) <= (unsigned int) tape->pos) array_int_push_back(tape->tape, 0); }
static void tape_devance(tape_t tape) { if (tape->pos > 0) tape->pos--; }

typedef struct {
  string_t code;
  array_int_t bracket_map;
} program_t[1];

static void program_init(program_t p, string_t text) {
  string_init (p->code);
  array_int_init (p->bracket_map);
  array_int_t leftstack;
  string_t symbols;
  array_int_init(leftstack);
  string_init_set_str(symbols, "[]<>+-,.");
  int pc = 0;
  for (size_t i = 0; i < string_size(text); i++) {
    char c = string_get_char(text, i);
    
    if (string_search_char(symbols, c) == STRING_FAILURE) continue;

    if (c == '[') array_int_push_back (leftstack, pc);
    else
      if (c == ']' && array_int_size (leftstack) != 0) {
        int left;
        array_int_pop_back(&left, leftstack);
        int right = pc;
        *array_int_safe_get (p->bracket_map, left) = right;
        *array_int_safe_get (p->bracket_map, right) = left;
      }

    pc++;
    string_push_back(p->code, c);
  }
  array_int_clear(leftstack);
  string_clear(symbols);
}
static void program_clear(program_t p) {
  string_clear(p->code);
  array_int_clear(p->bracket_map);
}
static void program_run(program_t p){
  tape_t tape;
  tape_init(tape);
  for(size_t pc = 0; pc < string_size(p->code); pc++) {
    switch (string_get_char(p->code, pc)) {
    case '+':
      tape_inc(tape);
      break;
    case '-':
      tape_dec(tape);
      break;
    case '>':
      tape_advance(tape);
      break;
    case '<':
      tape_devance(tape);
      break;
    case '[':
      if (tape_get(tape) == 0) pc = *array_int_cget(p->bracket_map, pc);
      break;
    case ']':
      if (tape_get(tape) != 0) pc = *array_int_cget(p->bracket_map, pc);
      break;
    case '.':
      printf("%c", tape_get(tape));
      fflush(stdout);
      break;
    default:
      abort();
      break;
    }
  }
  tape_clear(tape);
}

static void read_file(string_t str, const string_t filename) {
  FILE *f = fopen(string_get_cstr(filename), "rt");
  if (!f) {
    fprintf(stderr, "ERROR: Cannot open %s\n", string_get_cstr(filename));
    exit(2); 
  }
  // Read the full FILE in the string
  string_fgets (str, f, STRING_READ_FILE);
  fclose(f);
}

int main(int argc, const char*argv[])
{
  string_t filename, text;
  assert (argc >= 2);
  string_init_set_str(filename, argv[1]);
  string_init(text);
  read_file(text, filename);
  program_t p;
  program_init(p, text);
  program_run(p);
  program_clear(p);
  string_clear(text);
  string_clear(filename);
  return 0;
}
