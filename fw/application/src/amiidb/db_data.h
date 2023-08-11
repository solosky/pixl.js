#ifndef AMIIDB_DATA_H
#define AMIIDB_DATA_H

#include <stdint.h>

typedef struct {
    uint16_t amiibo_id;
    uint32_t head;
    uint32_t tail;
    const char *name_en;
    const char *name_cn;
} db_amiibo_t;

struct _db_link_t;

typedef struct _db_game_t {
    uint16_t game_id;
    uint16_t parent_game_id;
    const char *name_en;
    const char *name_cn;
    uint16_t link_cnt;
    struct _db_link_t *link;
} db_game_t;

typedef struct _db_link_t {
    uint16_t amiibo_id;
    const char *usage_en;
    const char *usage_cn;
} db_link_t;

#endif