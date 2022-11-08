#include "m-bptree.h"
#include "m-string.h"

// Define a map int -> string named map_employee_id
// The number 5 is the number of elements in a node of the B+TREE
BPTREE_DEF2(map_employee_id, 5, int, string_t)
// Register the oplist of this map globally.
#define M_OPL_map_employee_id_t() BPTREE_OPLIST2(map_employee_id, M_BASIC_OPLIST, STRING_OPLIST)

// Define a map string -> int named map_employee_name
// The number 5 is the number of elements in a node of the B+TREE
BPTREE_DEF2(map_employee_name, 5, string_t, int)
// Register the oplist of this map globally.
#define M_OPL_map_employee_name_t() BPTREE_OPLIST2(map_employee_name, STRING_OPLIST, M_BASIC_OPLIST)

int main(void)
{
  map_employee_id_t Employees;
  map_employee_id_init(Employees);

  // Assign the map with some data
  map_employee_id_set_at(Employees, 17425, STRING_CTE("Mike"));
  map_employee_id_set_at(Employees, 7147, STRING_CTE("Tango"));
  map_employee_id_set_at(Employees, 841, STRING_CTE("Charlie"));
  map_employee_id_set_at(Employees, 11632, STRING_CTE("John Do"));
  map_employee_id_set_at(Employees, 852, STRING_CTE("Peter"));
  map_employee_id_set_at(Employees, 530, STRING_CTE("Amanda"));
  map_employee_id_set_at(Employees, 4258, STRING_CTE("Elisabeth"));

  printf("Number of employees: %zu\n", map_employee_id_size(Employees));

  // Iterate over the employee list
  printf("\nList of employees (sorted by id):\n");
  map_employee_id_it_t it;
  for(map_employee_id_it(it, Employees); !map_employee_id_end_p(it); map_employee_id_next(it)) {
    // Get a pointer to this employee from the iterator
    const map_employee_id_itref_t *e = map_employee_id_cref(it);
    // Print it
    printf ("%d: %s\n", *(e->key_ptr), string_get_cstr(*(e->value_ptr)));
  }

  // Now, let's go using some macros!

  // Let's create a reverse map
  M_LET(Employees_r, map_employee_name_t) {

    // Let's fill in the reverse map with each employee
    // (using macro M_EACH)
    for M_EACH(e, Employees, map_employee_id_t) {
        map_employee_name_set_at(Employees_r, *(e->value_ptr), *(e->key_ptr));
      }

    // Iterate over the employee list
    printf("\nList of employees (sorted by name):\n");
    for M_EACH(e, Employees_r, map_employee_name_t) {
        printf ("%d: %s\n", *(e->value_ptr), string_get_cstr(*(e->key_ptr)));
      }

    // Print
    printf("\nElisabeth has id = %d\n", *map_employee_name_get(Employees_r, STRING_CTE("Elisabeth")));    
  } /* Employees_r is cleared beyond this point */

  map_employee_id_clear(Employees);
  return 0;
}
