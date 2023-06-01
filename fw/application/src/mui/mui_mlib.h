#ifndef MUI_MLIB_H
#define MUI_MLIB_H

#include "mui_mem.h"

#ifdef M_MEMORY_ALLOC
#undef M_MEMORY_ALLOC
#define M_MEMORY_ALLOC(type) mui_mem_malloc (sizeof (type))
#endif

#ifdef M_MEMORY_DEL
#undef M_MEMORY_DEL
#define M_MEMORY_DEL(ptr)  mui_mem_free(ptr)
#endif

#ifdef M_MEMORY_REALLOC
#undef M_MEMORY_REALLOC
#define M_MEMORY_REALLOC(type, ptr, n) (M_UNLIKELY ((n) > SIZE_MAX / sizeof(type)) ? NULL : mui_mem_realloc ((ptr), (n)*sizeof (type)))
#endif

#ifdef M_MEMORY_FREE
#undef M_MEMORY_FREE
#define M_MEMORY_FREE(ptr) mui_mem_free(ptr)
#endif

#endif