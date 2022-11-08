/* Include M*LIB header */
#include "m-array.h"
#include "m-string.h"
#include "m-tuple.h"

/* Provide several examples of different types usage with array,
   with the oplist to use for each case.

   Each time it creates and initializes an array,
   fills it with some number,
   computes its sum,
   and returns it.

   This is done for several basic C types:
   - integers,
   - floats, 
   - pointers, 
   - structures,
   - tuples
*/



/* Generate prototypes and inline functions
   for basic types.
   For integer and floats, there is no need to specify
   an oplist. M_BASIC_OPLIST will be used.
 */
ARRAY_DEF(r_bool, bool)

/* Basic usage of the type */
static int
test_bool(int n)
{
  r_bool_t array;

  r_bool_init(array);
  for(int i = 0; i < n; i++) {
    r_bool_push_back(array, i & 1);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    s += *r_bool_get(array, i);
  }
  
  r_bool_clear(array);

  return s;
}



/* Same with type char */
ARRAY_DEF(r_char, char)

static int
test_char(int n)
{
  r_char_t array;

  r_char_init(array);
  for(int i = 0; i < n; i++) {
    r_char_push_back(array, i & 255);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    s += *r_char_get(array, i);
  }
  
  r_char_clear(array);

  return s;
}



/* Same with type signed int. */
ARRAY_DEF(r_int, signed int)

static int
test_int(int n)
{
  r_int_t array;

  r_int_init(array);
  for(int i = 0; i < n; i++) {
    r_int_push_back(array, i);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    s += *r_int_get(array, i);
  }
  
  r_int_clear(array);

  return s;
}



/* Same with type signed int.
   But with an explicit oplist.
   This generates exactly the same code than r_int
*/
ARRAY_DEF(r_int2, signed int, M_BASIC_OPLIST)

static int
test_int2(int n)
{
  r_int2_t array;

  r_int2_init(array);
  for(int i = 0; i < n; i++) {
    r_int2_push_back(array, i);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    s += *r_int2_get(array, i);
  }
  
  r_int2_clear(array);

  return s;
}



/* Same with type unsigned long long */
ARRAY_DEF(r_ullong, unsigned long long)

static int
test_ullong(int n)
{
  r_ullong_t array;

  r_ullong_init(array);
  for(int i = 0; i < n; i++) {
    r_ullong_push_back(array, i);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    s += *r_ullong_get(array, i);
  }
  
  r_ullong_clear(array);

  return s;
}



/* Same with type float */
ARRAY_DEF(r_float, float)

static int
test_float(int n)
{
  r_float_t array;

  r_float_init(array);
  for(int i = 0; i < n; i++) {
    r_float_push_back(array, i/4.0);
  }

  float s = 0;
  for(int i = 0; i < n; i++) {
    s += *r_float_get(array, i);
  }
  
  r_float_clear(array);

  return s;
}



/* Same with type double and an explicit oplist*/
ARRAY_DEF(r_double, double, M_BASIC_OPLIST)

static int
test_double(int n)
{
  r_double_t array;

  r_double_init(array);
  for(int i = 0; i < n; i++) {
    r_double_push_back(array, i/4.0);
  }

  float s = 0;
  for(int i = 0; i < n; i++) {
    s += *r_double_get(array, i);
  }
  
  r_double_clear(array);

  return s;
}



/* Helper function strdup */
static char *my_strdup(const char *p)
{
  size_t s = strlen(p);
  char *d= malloc(s+1);
  if(!d) abort();
  strcpy(d, p);
  return d;
}

/* Same with type const char *, representing a contant string.
   We *** need *** an explicit oplist to tell M*LIB
   to consider the type as a constant string.
   The oplist to use is M_CSTR_OPLIST.
   We could have used M_PTR_OPLIST too,
   but in this case, we won't get proper string order or
   string equality.
*/
ARRAY_DEF(r_cstring, const char *, M_CSTR_OPLIST)

static int
test_cstring(int n)
{
  r_cstring_t array;

  r_cstring_init(array);
  for(int i = 0; i < n; i++) {
    char buffer[16];
    // Create a string from the integer
    sprintf(buffer, "%d", i);
    // We need to make an explicit copy of the string
    // since it is not supported by the basic type.
    char *p = my_strdup(buffer);
    // Push the copy in the container.
    r_cstring_push_back(array, p);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    // Get back the string
    const char *p = *r_cstring_get(array, i);
    // Convert it to integer
    int j = atoi(p);
    s += j;
  }

  // We also need a special unalloc pass
  // to free all constant strings we have allocated.
  for(int i = 0; i < n; i++) {
    const char *p = *r_cstring_get(array, i);
    free((void*)(uintptr_t)p);
  }
  
  r_cstring_clear(array);

  return s;
}



/* Same with type string_t of M*LIB, representing a variable string.
   We don't need explicit oplist to tell M*LIB!
   The oplist to use is STRING_OPLIST,
   but it has been registered globaly, so we don't need to explicit
   give it to array_def! */
ARRAY_DEF(r_string, string_t)

static int
test_string(int n)
{
  r_string_t array;
  string_t str;
  
  r_string_init(array);
  string_init (str);
  
  for(int i = 0; i < n; i++) {
    // create a string from an integer
    string_printf(str, "%d", i);
    // No need to make a copy in the container!
    r_string_push_back(array, str);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    // Get back the string and convert it back to an integer
    int j = atoi(string_get_cstr(*r_string_get(array, i)));
    s += j;
  }

  // No need to explicit dealloc !
  string_clear(str);
  r_string_clear(array);

  return s;
}



/* Same with type string_t of M*LIB, representing a variable string
   and its explicit oplist. And said above, it generates exactly the
   same code. */
ARRAY_DEF(r_string2, string_t, STRING_OPLIST)



/* Same with type 'volatile unsigned int *'
   The array stores pointers to another integer,
   allocated somewhere else.
   We need to tell M*LIB to handle the basic type
   as pointers without caring for what the value of
   the pointer is.
   So here, we **need** to specify the oplist of the type
   as M_PTR_OPLIST.
 */
ARRAY_DEF(r_vintptr, volatile unsigned int *, M_PTR_OPLIST)

static int
test_vintptr(int n)
{
  r_vintptr_t array;

  // Allocate a secondary table to store the value.
  unsigned *tab = calloc(n, sizeof(volatile unsigned int));
  if (!tab) abort();
  
  r_vintptr_init(array);
  for(int i = 0; i < n; i++) {
    tab[i] = i*i - i;
    // Push the pointer to this integer in the array
    r_vintptr_push_back(array, &tab[i]);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    // Get the pointer to the integer.
    volatile unsigned *p = *r_vintptr_get(array, i);
    s += *p;
  }
  
  r_vintptr_clear(array);
  free(tab);
  
  return s;
}



/* Same with a structure type.
   We need to tell M*LIB to handle the basic type
   as POD;
   So here, we **need** to specify the oplist of the type
   as M_POD_OPLIST.
 */
struct rock_me_out {
  int n;
  float other;
};

ARRAY_DEF(r_rockme, struct rock_me_out, M_POD_OPLIST)

static int
test_rockme(int n)
{
  r_rockme_t array;

  r_rockme_init(array);
  for(int i = 0; i < n; i++) {
    struct rock_me_out rock;
    rock.n = i*i - i;
    // Push the struct in the array
    r_rockme_push_back(array, rock);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    // Get the pointer to the struct
    struct rock_me_out *p = r_rockme_get(array, i);
    s += p->n;
  }
  
  r_rockme_clear(array);
  
  return s;
}



/* Same with a structure type defined with [1]:
   We need to tell M*LIB to handle the basic type
   as a special array;
   So here, we **need** to specify the oplist of the type
   as M_A1_OPLIST.
 */
typedef struct rock_me_in {
  int n;
  float other;
} rock_me_in[1];

ARRAY_DEF(r_rockme2, rock_me_in, M_A1_OPLIST)

static int
test_rockme2(int n)
{
  r_rockme2_t array;

  r_rockme2_init(array);
  for(int i = 0; i < n; i++) {
    rock_me_in rock;
    rock->n = i*i - i;
    r_rockme2_push_back(array, rock);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    // Get the pointer to the struct.
    rock_me_in *p = r_rockme2_get(array, i);
    s += (*p)->n;
  }
  
  r_rockme2_clear(array);

  return s;
}



/* Same with a structure type defined with [1] and a global registeration.
   We need to tell M*LIB to handle the basic type
   as a special array.

   To do so, we register the oplist of rock_us_t by defining a macro based
   on M_OPL_ + type name + () which expands with the oplist of the type.

   Then we define the array, giving it only the type.
   M*LIB will look at the global registered oplist of the type, we don't
   need to give it explictly.

   Global registeration makes easier code.
   Registeration should be done when the type is defined.
 */
#define M_OPL_rock_me_in() M_A1_OPLIST

ARRAY_DEF(r_rockme2b, rock_me_in)

static int
test_rockme2b(int n)
{
  r_rockme2b_t array;

  r_rockme2b_init(array);
  for(int i = 0; i < n; i++) {
    rock_me_in rock;
    rock->n = i*i - i;
    r_rockme2b_push_back(array, rock);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    // Get the pointer to the struct.
    rock_me_in *p = r_rockme2b_get(array, i);
    s += (*p)->n;
  }
  
  r_rockme2b_clear(array);
  
  return s;
}



/* Same with a structure type defined as a tuple.

   We first define a tuple. No need to specify the oplist as it uses basic C types
   so M_BASIC_OPLIST is used.

   Then we define the array, giving it the oplist definition of the tuple:
   the macro TUPLE_OPLIST is used to build it based on the tuple name,
   and the oplists of the tuple.
 */
TUPLE_DEF2(rock_you, (n, int), (other, float))

ARRAY_DEF(r_rockme3, rock_you_t, TUPLE_OPLIST(rock_you, M_BASIC_OPLIST, M_BASIC_OPLIST))

static int
test_rockme3(int n)
{
  r_rockme3_t array;
  rock_you_t  x;
  
  r_rockme3_init(array);
  rock_you_init(x);
  
  for(int i = 0; i < n; i++) {
    rock_you_set_n(x, i*i - i);
    r_rockme3_push_back(array, x);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    // Get the pointer to the structure containing the integer.
    rock_you_t *p = r_rockme3_get(array, i);
    s += (*p)->n;
  }
  
  r_rockme3_clear(array);
  rock_you_clear(x);
  
  return s;
}



/* Same with a structure type defined as a tuple using global registeration.

   We first define a tuple. No need to specify the oplist as it uses basic C types
   so M_BASIC_OPLIST is used.

   Then we register the oplist of rock_us_t by defining a macro based
   on M_OPL_ + type name + () which expands with the oplist of the type.

   Then we define the array, giving it only the type.
   M*LIB will look at the global registered oplist of the type, we don't
   need to give it explictly.

   Global registeration makes easier code.
 */
TUPLE_DEF2(rock_us, (n, int), (other, float))

#define M_OPL_rock_us_t() TUPLE_OPLIST(rock_us, M_BASIC_OPLIST, M_BASIC_OPLIST)

ARRAY_DEF(r_rockme4, rock_us_t)

static int
test_rockme4(int n)
{
  r_rockme4_t array;
  rock_us_t  x;
  
  r_rockme4_init(array);
  rock_us_init(x);
  
  for(int i = 0; i < n; i++) {
    rock_us_set_n(x, i*i - i);
    r_rockme4_push_back(array, x);
  }

  int s = 0;
  for(int i = 0; i < n; i++) {
    // Get the pointer to the structure containing the integer.
    rock_us_t *p = r_rockme4_get(array, i);
    s += (*p)->n;
  }
  
  r_rockme4_clear(array);
  rock_us_clear(x);
  
  return s;
}


int main (int argc, const char *argv[])
{
  int n = argc == 1 ? 10 : atoi(argv[1]);
  int s;

  s = test_bool(n);
  printf ("S[bool] = %d\n", s);

  s = test_char(n);
  printf ("S[char] = %d\n", s);

  s = test_int(n);
  printf ("S[int] = %d\n", s);

  s = test_int2(n);
  printf ("S[int2] = %d\n", s);

  s = test_ullong(n);
  printf ("S[ullong] = %d\n", s);

  s = test_float(n);
  printf ("S[float] = %d\n", s);

  s = test_double(n);
  printf ("S[double] = %d\n", s);

  s = test_cstring(n);
  printf ("S[cstring] = %d\n", s);

  s = test_string(n);
  printf ("S[string] = %d\n", s);

  s = test_vintptr(n);
  printf ("S[vintptr] = %d\n", s);

  s = test_rockme(n);
  printf ("S[rockme] = %d\n", s);

  s = test_rockme2(n);
  printf ("S[rockme2] = %d\n", s);

  s = test_rockme2b(n);
  printf ("S[rockme2b] = %d\n", s);

  s = test_rockme3(n);
  printf ("S[rockme3] = %d\n", s);

  s = test_rockme4(n);
  printf ("S[rockme4] = %d\n", s);

  exit(0);
}
