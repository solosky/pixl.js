#include "m-serial-json.h"
#include "m-tuple.h"
#include "m-array.h"

// Let's define a 'person' composed of several fields.
TUPLE_DEF2(person,
           (surname, string_t),
           (age, int),
           (present, bool)
           )
// Register the oplist of this tuple globaly
#define M_OPL_person_t() TUPLE_OPLIST(person, STRING_OPLIST, M_BASIC_OPLIST, M_BOOL_OPLIST )

// Let's define an array of person
ARRAY_DEF(base, person_t)
// Register the oplist of this array globaly
#define M_OPL_base_t() ARRAY_OPLIST(base, M_OPL_person_t())

// Let's read an array of person from the given JSON file
static void read(base_t base, const char filename[])
{
  m_serial_read_t  in;
  m_serial_return_code_t ret;

  // Open the file
  FILE *f = fopen(filename, "rt");
  if (!f) {
    fprintf(stderr, "ERROR: Cannot open file '%s'.\n", filename);
    exit(2);
  }

  // Initialize the serializer with the file
  m_serial_json_read_init(in, f);
  // Read the array
  ret = base_in_serial(base, in);
  if (ret != M_SERIAL_OK_DONE) {
    fprintf(stderr, "ERROR: Cannot read JSON data from file '%s'.\n", filename);
    exit(2);
  }
  // Let's close & clear all
  m_serial_json_read_clear(in);
  fclose(f);
}

int main(void)
{
  // Let's have base as a base_t
  M_LET(base, base_t) {
    // Read the JSON file and fill-in base
    read(base, "ex11-json01.json");
    base_emplace_back(base, STRING_CTE("John"), 40, false);
    // Print the contents of base:
    printf ("List of presents:\n");
    for M_EACH(el, base, base_t) {
      if ((*el)->present) {
        printf("%s\n", string_get_cstr((*el)->surname));
      }
    }
  }
  exit(0);    
}
