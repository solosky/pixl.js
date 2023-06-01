#ifndef MUI_MEM_H
#define MUI_MEM_H

#include <stddef.h>
#include <stdint.h>
#include "mui_conf.h"



typedef struct {
    uint32_t total_size; /**< Total heap size*/
    uint32_t free_cnt;
    uint32_t free_size; /**< Size of available memory*/
    uint32_t free_biggest_size;
    uint32_t used_cnt;
    uint32_t max_used; /**< Max size of Heap memory used*/
    uint8_t used_pct; /**< Percentage used*/
    uint8_t frag_pct; /**< Amount of fragmentation*/
} mui_mem_monitor_t;


void mui_mem_init();
void mui_mem_deinit();

void * mui_mem_malloc(size_t size);
void * mui_mem_realloc(void * p, size_t new_size);
void mui_mem_free(void *);

void mui_mem_monitor(mui_mem_monitor_t* p_mon);


#endif 