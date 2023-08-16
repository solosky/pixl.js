//
// Created by solos on 8/19/2023.
//

#ifndef FW_AMIIDB_API_SLOT_H
#define FW_AMIIDB_API_SLOT_H


#include <stdint.h>
#include "ntag_def.h"

typedef struct {
    uint8_t slot;
    uint8_t is_empty;
    uint32_t amiibo_head;
    uint32_t amiibo_tail;
} amiidb_slot_info_t;

typedef void (*amiibo_slot_info_cb_t)(amiidb_slot_info_t* p_info);

int32_t amiidb_api_slot_read(uint8_t slot, ntag_t* p_ntag);
int32_t amiidb_api_slot_write(uint8_t slot, ntag_t* p_ntag);
int32_t amiidb_api_slot_remove(uint8_t slot);


int32_t amiidb_api_slot_info(uint8_t slot, amiidb_slot_info_t * p_info);
int32_t amiidb_api_slot_list(amiibo_slot_info_cb_t cb);

#endif // FW_AMIIDB_API_SLOT_H
