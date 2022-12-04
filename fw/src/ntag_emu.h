/*
 * ntag_emu.h
 *
 *  Created on: 2021年9月1日
 *      Author: solos
 */

#ifndef NTAG_EMU_H_
#define NTAG_EMU_H_

#define NTAG_EMU_V2

#include "ntag_def.h"
#include "nrfx.h"

typedef void (*ntag_update_cb_t)(void* context, ntag_t* ntag);
ret_code_t ntag_emu_init(ntag_t* tag);
void ntag_emu_uninit();
void ntag_emu_set_tag(ntag_t* tag);
void ntag_emu_set_uuid_only(ntag_t* ntag);
void ntag_emu_set_update_cb(ntag_update_cb_t cb, void* context);
ntag_t* ntag_emu_get_current_tag();




#endif /* NTAG_EMU_H_ */
