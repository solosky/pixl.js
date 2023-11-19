#ifndef MUI_MLIB_H
#define MUI_MLIB_H

#include "mui_mem.h"

#ifdef M_MEMORY_ALLOC
#define M_MEMORY_ALLOC(type) mui_mem_malloc (sizeof (type))
#error "include mui_mlib.h before mui_mem.h"

#else
#define M_MEMORY_ALLOC(type) mui_mem_malloc (sizeof (type))
#endif

#ifdef M_MEMORY_DEL
#error "include mui_mlib.h before mui_mem.h"
#else
#define M_MEMORY_DEL(ptr)  mui_mem_free(ptr)
#endif

#ifdef M_MEMORY_REALLOC
#error "include mui_mlib.h before mui_mem.h"
#else
#define M_MEMORY_REALLOC(type, ptr, n) (M_UNLIKELY ((n) > SIZE_MAX / sizeof(type)) ? NULL : mui_mem_realloc ((ptr), (n)*sizeof (type)))
#endif

#ifdef M_MEMORY_FREE
#error "include mui_mlib.h before mui_mem.h"
#else
#define M_MEMORY_FREE(ptr) mui_mem_free(ptr)
#endif

#endif