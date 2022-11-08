/*
 * All the definition are included in the file m-array.h
 * You don't need to link with any library.
 */
#include "m-array.h"

// First you need to instanciate your code for the type you want.
// This is done by calling the ARRAY_DEF macro with the name
// of the instance (here array_charp) and the type of the instance
// (here a pointer to char).
// This will create all types and functions to handle an
// array of char pointers.
// So let's define an array of 'char *' named 'array_charp'
ARRAY_DEF (array_charp, char *)
// Ok everything has been defined. We can use everything like
// standard C function.

int main(void)
{
  // Declare an array of 'char *'.
  // The used name for the container is the name concatened with '_t'.
  array_charp_t al;

  // Like a good POD it needs to be explicitly initialized.
  // Notice that we don't to pass the address of the variable as input
  // to the function, but the variable.
  // In fact, the array is automatically passed by reference to any
  // function.
  array_charp_init(al);
      
  // Let's do some basic stuff:
  // add some elements to the array list
  array_charp_push_back (al,"C");
  array_charp_push_back (al,"A");
  array_charp_push_back (al,"E");
  array_charp_push_back (al,"B");
  array_charp_push_back (al,"D");
  array_charp_push_back (al,"F");

  // Use iterator to display contents of al
  printf("Original contents of al: ");
  // The iterator has also been defined with the ARRAY_DEF macro.
  // All iterators of all containers follow the same logic,
  // so that you can change the type of the container without
  // changing your code.
  array_charp_it_t itr;
  // You initialized the iterator to the first element of the container,
  // iterate until not the end of the container is reached,
  // while moving the iterator to the next element of the array.
  // This is pretty overkill for an array, but this works for any container.
  for (array_charp_it(itr, al) ; !array_charp_end_p (itr); array_charp_next(itr)) {
    // Let's get a reference to the data pointed by the iterator.
    // You'll always get a pointer to this data, not the data itself.
    // So you need an extra '*' to read the data.
    char *element = *array_charp_ref(itr);
    printf ("%s ", element);
  }
  printf("\n");
      
  // Now, display the list backwards
  // As the iterator as reached the end of the array,
  // let's go back by one element,
  // iterate until not the end of the container is reached,
  // while moving the iterator to the previous element of the array.
  printf("Array backwards: ");
  for (array_charp_previous (itr) ; !array_charp_end_p (itr); array_charp_previous(itr)) {
    char *element = *array_charp_ref(itr);
    printf ("%s ", element);
  }
  printf("\n");

  // As it is an array, we can also iterate using a standard integer:
  printf("Array Direct: ");
  for(size_t i = 0 ; i < array_charp_size(al); ++i) {
    // Like for getting a reference of an iterator
    // the get operator returns a pointer to the data.
    // This allows handling non-copying type within the container
    // (like array).
    char *element = *array_charp_get(al, i);
    printf ("%s ", element);
  }
  printf("\n");

  // Like POD, we need to clear the array explicitly
  array_charp_clear(al);
  return 0;
}
