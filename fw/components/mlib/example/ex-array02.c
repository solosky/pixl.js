// This is the same example as ex-array01.c
// It uses some macros to simplify the writing of the code.
// So it is a shorter version of the first example.
#include "m-array.h"

// Let's define an array of 'char *' named 'array_charp'
ARRAY_DEF (array_charp, char *)

// Let's register globaly the oplist of array_charp
// This is done by defining a macro named M_OPL_ and the type
// of the containern, like this:
#define M_OPL_array_charp_t() ARRAY_OPLIST(array_charp)
// An oplist is the interface published by the type.
// It contains all exported methods of the type
// (and which functions shall be called to handle them).
// It is both an input of the ARRAY_DEF macro (the last argument)
// if the type used to create the container has non-standard
// requirements (like pretty much any C structure) and an output
// (using ARRAY_OPLIST) which provides the updated oplist
// which will handle the container itself.

int main(void)
{
  // Create an array list 'al' of type 'array_charp_t'
  // M_LET will check if a global oplist has been registered
  // with the name array_charp_t, and if it finds it, will
  // define the type, initializes it and clears it after
  // it disappears from the bracket:
  M_LET (al, array_charp_t) {
    // add elements to the array list
    array_charp_push_back (al,"C");
    array_charp_push_back (al,"A");
    array_charp_push_back (al,"E");
    array_charp_push_back (al,"B");
    array_charp_push_back (al,"D");
    array_charp_push_back (al,"F");

    // Display contents of al
    printf("Original contents of al: ");
    // M_EACH is a macro which iterates over a container.
    // It first argument is the name of the created variable
    // which will be a pointer to one element of the container,
    // al is the container to iterate,
    // the last argument is either the type of the container
    // (if a global oplist has been registered) or is the oplist
    // of the container (if none).
    // Indeed the macro needs to access the methods of the container
    // to iterate over the container.
    // So for each element of the array al
    for M_EACH(element, al, array_charp_t) {
        printf ("%s ", *element);
    }
    printf("\n");
  } // After this point al is automatically cleared
  return 0;
}
