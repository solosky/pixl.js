#ifndef AMIIDB_DATA_H
#define AMIIDB_DATA_H

#include <stdint.h>

typedef struct {
    uint32_t head;
    uint32_t tail;
    const char *name_en;
    const char *name_cn;
} db_amiibo_t;

struct _db_link_t;

typedef struct _db_game_t {
    uint8_t game_id;
    uint8_t parent_game_id;
    const char *name_en;
    const char *name_cn;
    uint8_t order;
    uint16_t link_cnt;
} db_game_t;

typedef struct _db_link_t {
    uint8_t  game_id;
    uint32_t head;
    uint32_t tail;
    const char *note_en;
    const char *note_cn;
} db_link_t;

extern const db_amiibo_t amiibo_list[];
extern const db_game_t game_list[];
extern const db_link_t link_list[];

const db_amiibo_t * get_amiibo_by_id(uint32_t head, uint32_t tail);
const db_link_t* get_link_by_id(uint8_t game_id, uint32_t head, uint32_t tail);

#endif