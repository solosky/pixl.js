#include "db_header.h"

const db_amiibo_t * get_amiibo_by_id(uint32_t head, uint32_t tail){
    int left = 0;
    int right = amiibo_list_size - 1;
    while (left <= right) {
        int mid_index = (left + right) / 2;
        const db_amiibo_t *mid = &amiibo_list[mid_index];

        // Compare by (head, tail)
        if (mid->head < head || (mid->head == head && mid->tail < tail)) {
            left = mid_index + 1;
        } else if (mid->head == head && mid->tail == tail) {
            return mid;
        } else {
            right = mid_index - 1;
        }
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