#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "m-list.h"
#include "m-string.h"
#include "m-algo.h"

// Let's create a list of string.
LIST_DEF(list_string, string_t)

// Let's register the oplist globally so that other m*lib macros
// can get the oplist without us giving it explictly to them.
#define M_OPL_list_string_t() LIST_OPLIST(list_string, STRING_OPLIST)

// Let's define some basic algorithms over this list of string.
ALGO_DEF(astring, M_OPL_list_string_t())

int main(void)
{
  // Let's create a variable named 'grow' which is a list of string_t
  // This variable is only defined within the associated '{' '}' block.
  // The constructor/destructor of the variable will be called within this block
  // You can also define the variable (list_string_t) and use explicitly
  // its constructor (list_string_init) / destructor (list_string_clear).
  M_LET (grow, list_string_t)
    M_LET (tmpstr, string_t) { // And let's create also a string_t

    // add string elements to the list
    list_string_push_back (grow, STRING_CTE("AB"));
    string_printf(tmpstr, "%d", 12);
    list_string_push_back (grow, tmpstr);
    list_string_push_back (grow, STRING_CTE("CD"));

    // Serialize the list of string into a big string.
    list_string_get_str (tmpstr, grow, false);

    // print out
    printf("Number of elements = %zu\n", list_string_size(grow));
    printf("Final string = %s\n", string_get_cstr(tmpstr));

    // Let's iterate over each element of the list
    // using the macro for iteration over a container.
    printf ("Using macro EACH :\n");
    for M_EACH (item, grow, list_string_t) {
        printf ("Item = ");
        string_fputs (stdout, *item);
        printf ("\n");
      }

    // You can also use classic iterator:
    list_string_it_t it;
    printf ("Using iterators :\n");
    for(list_string_it(it, grow) ; !list_string_end_p(it) ; list_string_next(it)) {
      const string_t *item = list_string_cref (it);
      printf ("Item = ");
      string_fputs (stdout, *item);
      printf ("\n");
    }

    // Get the min and the max of the list
    string_t *p = astring_min(grow);
    printf ("Min string is %s\n", string_get_cstr(*p));
    p = astring_max(grow);
    printf ("Max string is %s\n", string_get_cstr(*p));

    // We can even sort the list.
    astring_sort(grow);
    for M_EACH (item, grow, list_string_t) {
        printf ("Sort Item = %s\n", string_get_cstr(*item) );
      }
    
    // Split a string into a list.
    string_set_str(tmpstr, "HELLO;JOHN;HOW;ARE;YOU");
    astring_split(grow, tmpstr, ';');
    for M_EACH (item, grow, list_string_t) {
        printf ("Split Item = %s\n", string_get_cstr(*item) );
      }
    
  } // Everything is cleared after this point.
  
  return 0;
}
