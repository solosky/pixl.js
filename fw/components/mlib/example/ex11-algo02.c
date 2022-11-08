#include "m-tuple.h"
#include "m-array.h"
#include "m-algo.h"
#include "m-funcobj.h"
#include "m-serial-json.h"
#include "m-string.h"

/* Define the employee class */
TUPLE_DEF2(employee,
           (name, string_t),
           (age, int),
           (idnum, int))
/* Register globaly the employee_t oplist */
#define M_OPL_employee_t() TUPLE_OPLIST(employee, STRING_OPLIST, M_BASIC_OPLIST, M_BASIC_OPLIST)

/* Define an array of employee, register the oplist and define further algorithms on it */
ARRAY_DEF(vector_employee, employee_t)
#define M_OPL_vector_employee_t() ARRAY_OPLIST(vector_employee, M_OPL_employee_t())
ALGO_DEF(vector_employee, vector_employee_t)

/* Define the function object to select the sort order.
 * It is named 'compare_by' and is an instance of the interface
 * 'vector_employee_cmp_obj' created by ALGO_DEF.
 */
FUNC_OBJ_INS_DEF(compare_by /* name of the instance */,
                 vector_employee_cmp_obj /* name of the interface */,
                 (a, b) /* name of the input parameters of the function like object. The type are inherited from the interface. */, {
                   /* code of the function object */
                   if (string_equal_str_p(self->sort_field, "name"))
                     return string_cmp(a->name, b->name);
                   else if (string_equal_str_p(self->sort_field, "age"))
                     return a->age < b->age ? -1 : a->age > b->age;
                   else
                     return a->idnum < b->idnum ? -1 : a->idnum > b->idnum;
                 },
                 /* Additional fields stored in the function object */
                 (sort_field, string_t) )
#define M_OPL_compare_by_t() FUNC_OBJ_INS_OPLIST(compare_by, STRING_OPLIST)

// Let's read an array of person from the given JSON file
static void read(vector_employee_t base, const char filename[])
{
  m_serial_return_code_t ret;

  // Open the file
  FILE *f = fopen(filename, "rt");
  if (!f) {
    // Handle errors
    fprintf(stderr, "ERROR: Cannot open file '%s'.\n", filename);
    exit(2);
  }

  // Initialize the JSON serializer with this file
  M_LET( (in,f), m_serial_json_read_t)
    // Read the JSON file and fill in 'base' with it.
    ret = vector_employee_in_serial(base, in);

  // Let's handle serialization error
  if (ret != M_SERIAL_OK_DONE) {
    fprintf(stderr, "ERROR: Cannot read JSON data from file '%s'.\nParsing stops at:\n", filename);
    while (!feof(f)) { fputc(fgetc(f), stdout); }
    fclose(f);
    exit(2);
  }

  // Close the FILE
  fclose(f);
}

int main(void)
{
  M_LET(emps, vector_employee_t) {
    /* Code to populate database from JSON */
    read(emps, "ex11-algo02.json");
    
    // Sort the database by employee ID number
    M_LET( (cmp, STRING_CTE("idnum")), compare_by_t )
      vector_employee_sort_fo(emps, compare_by_as_interface(cmp));
    M_PRINT("Employees sorted by idnum are: ", (emps, vector_employee_t), "\n" );

    // Sort the database by employee name
    M_LET( (cmp, STRING_CTE("name")), compare_by_t )
      vector_employee_sort_fo(emps, compare_by_as_interface(cmp));
    M_PRINT("Employees sorted by name are: ", (emps, vector_employee_t), "\n" );

    // End
  }
  return 0;
}
