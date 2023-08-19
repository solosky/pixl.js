//
// Created by solos on 8/19/2023.
//

#ifndef FW_AMIIDB_API_SLOT_H
#define FW_AMIIDB_API_SLOT_H


#include <stdint.h>
#include "ntag_def.h"

int32_t amiidb_api_slot_read(uint8_t slot, ntag_t* p_ntag);
int32_t amiidb_api_slot_write(uint8_t slot, ntag_t* p_ntag);

#endif // FW_AMIIDB_API_SLOT_H
