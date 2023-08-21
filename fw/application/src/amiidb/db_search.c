#include "db_header.h"

const db_amiibo_t * get_amiibo_by_id(uint32_t head, uint32_t tail){
    const  db_amiibo_t *amiibo = amiibo_list;
    while(amiibo->name_en){
        if(amiibo->head == head && amiibo->tail == tail){
            return amiibo;
        }
        amiibo += 1;
    }
    return 0;
}

const db_link_t* get_link_by_id(uint8_t game_id, uint32_t head, uint32_t tail){
    const db_link_t *link = link_list;
    while(link->game_id > 0){
        if(link->game_id == game_id && link->head == head && link->tail == tail){
            return link;
        }
        link += 1;
    }
    return 0;
}