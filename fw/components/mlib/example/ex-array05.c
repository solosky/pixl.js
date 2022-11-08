#include "m-array.h"
#include "m-string.h"

/* This example show how to use complex structure with array 
   embedding another library */

/* This is a typical trivial library with its interface */
typedef struct lib_ext_struct {
  int id;
  // Other data may be added
} lib_ext_struct;

/* Method to create a duplicate of the object.
   The new object is heap allocated */
static lib_ext_struct *lib_ext_struct_Duplicate(const lib_ext_struct *obj)
{
  lib_ext_struct *p = malloc(sizeof(lib_ext_struct));
  if (!p) abort();
  p->id = obj->id;
  return p;
}

/* Method to delete any object */
static void lib_ext_struct_Delete(lib_ext_struct *obj)
{
  free(obj);
}

/* This is the complex structure */
typedef struct  {
  uint32_t id;
  string_t type;
  lib_ext_struct* properties;
} data_node;

/* This method shall initialize the object data_node to
   the init state.
   It takes as first argument the object to initialize,
   and since it takes by reference through a pointer,
   it is compatible with 'API_2' convention for oplist */
static void data_node_init(data_node *obj)
{
  obj->id = 0;
  string_init(obj->type);
  obj->properties = NULL;
}

/* This method shall initialize the object data_node to
   the same state as the 'src' object (creating a copy, a duplicate) .
   It takes as first argument the object to initialize,
   and as second argument the source object.
   It takes both object by reference through pointers,
   as such it is compatible with 'API_6' convention for oplist */
static void data_node_init_set(data_node *obj, const data_node *src)
{
  obj->id = src->id;
  string_init_set(obj->type, src->type);
  if (src->properties)
    obj->properties = lib_ext_struct_Duplicate(src->properties);
  else
    obj->properties = NULL;
}

/* This method shall set the object data_node to
   the same state as the 'src' object (creating a copy, a duplicate) .
   THe object was already initialized.
   It takes as first argument the object to initialize,
   and as second argument the source object.
   It takes both object by reference through pointers,
   as such it is compatible with 'API_6' convention for oplist */
static void data_node_set(data_node *obj, const data_node *src)
{
  obj->id = src->id;
  string_set(obj->type, src->type);
  if (obj->properties)
    lib_ext_struct_Delete(obj->properties);    
  if (src->properties)
    obj->properties = lib_ext_struct_Duplicate(src->properties);
  else
    obj->properties = NULL;
}

/* This method shall clear the object data_node to
   the cleared state (aka it shall free any resource).
   It takes as first argument the object to initialize,
   and since it takes by reference through a pointer,
   it is compatible with 'API_2' convention for oplist */
static void data_node_clear(data_node *obj)
{
  string_clear(obj->type);
  if (obj->properties)
    lib_ext_struct_Delete(obj->properties);
}

/* Create a dynamic array of 'data_node' named array_data_node_t
   and gives the minimum oplist allowing M*LIB to handle
   properly the object 'data_node' with the API convention
   to respect for calling the methods.
*/
ARRAY_DEF(/* the array prefix */ array_data_node,
	  /* the object type */ data_node,
	  /* the object oplist */ (INIT(API_2(data_node_init)),
				   SET(API_6(data_node_set)),
				   INIT_SET(API_6(data_node_init_set)),
				   CLEAR(API_2(data_node_clear))))

/* Define a global array with the created type */
array_data_node_t global_array;

int main(void)
{
  /* Call the created method */
  array_data_node_init(global_array);
  /* Do stuff... */
  array_data_node_clear(global_array);
}
