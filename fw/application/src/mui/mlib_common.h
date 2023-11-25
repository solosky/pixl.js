#ifndef MLIB_COMMON_H
#define MLIB_COMMON_H

#include "mui_mem.h"

#define M_MEMORY_ALLOC(type) mui_mem_malloc (sizeof (type))
#define M_MEMORY_DEL(ptr)  mui_mem_free(ptr)
#define M_MEMORY_REALLOC(type, ptr, n) (M_UNLIKELY ((n) > SIZE_MAX / sizeof(type)) ? NULL : mui_mem_realloc ((ptr), (n)*sizeof (type)))
#define M_MEMORY_FREE(ptr) mui_mem_free(ptr)


#include "m-array.h"
#include "m-string.h"
#include "m-dict.h"
#include "m-deque.h"

ARRAY_DEF(string_array, string_t, STRING_OPLIST)

#endif