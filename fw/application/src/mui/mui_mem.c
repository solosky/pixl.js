#include "mui_mem.h"
#include "stdlib.h"
#include "tlsf.h"

#include <string.h>

#ifdef MUI_MEM_USE_TLSF

static uint8_t m_mem[MUI_MEM_BUFF_SIZE];
static tlsf_t m_tlsf = NULL;

static void mui_mem_walker(void * ptr, size_t size, int used, void * user)
{
    mui_mem_monitor_t * mon_p = user;
    if(used) {
        mon_p->used_cnt++;
    }
    else {
        mon_p->free_cnt++;
        mon_p->free_size += size;
        if(size > mon_p->free_biggest_size)
            mon_p->free_biggest_size = size;
    }
}

void mui_mem_init() { m_tlsf = tlsf_create_with_pool(m_mem, sizeof(m_mem)); }
void mui_mem_deinit() {
    tlsf_destroy(m_tlsf);
    m_tlsf = NULL;
}
void *mui_mem_malloc(size_t size) { return tlsf_malloc(m_tlsf, size); }
void *mui_mem_realloc(void *p, size_t new_size) { return tlsf_realloc(m_tlsf, p, new_size); }
void mui_mem_free(void *p) { tlsf_free(m_tlsf, p); }

void mui_mem_monitor(mui_mem_monitor_t *mon_p) {
    memset(mon_p, 0, sizeof(mui_mem_monitor_t));
    tlsf_walk_pool(tlsf_get_pool(m_tlsf), mui_mem_walker, mon_p);

    mon_p->total_size = sizeof(m_mem);
    mon_p->used_pct = 100 - ((100U * mon_p->free_size) / mon_p->total_size);
    if (mon_p->free_size > 0) {
        mon_p->frag_pct = mon_p->free_biggest_size * 100U / mon_p->free_size;
        mon_p->frag_pct = 100 - mon_p->frag_pct;
    } else {
        mon_p->frag_pct = 0; /*no fragmentation if all the RAM is used*/
    }

    // mon_p->max_used = max_used;
}

#else

void mui_mem_init() {
    // nop
}
void mui_mem_deinit() {
    // nop
}

void *mui_mem_malloc(size_t size) { return malloc(size); }
void *mui_mem_realloc(void *p, size_t new_size) { return realloc(p, new_size); }
void mui_mem_free(void *p) { return free(p); }
void mui_mem_monitor(mui_mem_monitor_t *p_mon) {}

#endif