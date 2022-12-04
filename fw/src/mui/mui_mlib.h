#ifndef MUI_MLIB_H
#define MUI_MLIB_H

#include "mui_mem.h"

# define M_MEMORY_ALLOC(type) mui_mem_malloc (sizeof (type))
# define M_MEMORY_DEL(ptr)  mui_mem_free(ptr)
# define M_MEMORY_REALLOC(type, ptr, n) (M_UNLIKELY ((n) > SIZE_MAX / sizeof(type)) ? NULL : mui_mem_realloc ((ptr), (n)*sizeof (type)))
# define M_MEMORY_FREE(ptr) mui_mem_free(ptr)


#endif