#include "db_header.h"

const db_amiibo_t * get_amiibo_by_id(uint32_t head, uint32_t tail){
    const  db_amiibo_t *amiibo = amiibo_list;
    while(amiibo->head > 0){
        if(amiibo->head == head && amiibo->tail == tail){
            return amiibo;
        }
        amiibo += 1;
    }
    return 0;
}